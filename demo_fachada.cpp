/**
 * @file demo_fachada.cpp
 * @brief Cliente CLI Interativo da Fachada SSMH
 * 
 * Interface de linha de comando completa para o Sistema de Monitoramento
 * de Hidrômetros (SSMH) com acesso a todas as funcionalidades via menu.
 */

#include "src/core/fachada_ssmh.hpp"
#include "src/usuarios/storage/armazenamento_sqlite.hpp"
#include "src/usuarios/commands/user_commands.hpp"
#include "src/monitoramento/adapter/adaptador_ocr.hpp"
#include "src/monitoramento/storage/leitura_dao_memoria.hpp"
#include "src/alertas/services/alerta_service_factory.hpp"
#include "src/alertas/observers/painel_observer.hpp"
#include "src/alertas/observers/logger_observer.hpp"
#include "src/simulator/hidrometer.hpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <limits>
#include <map>

using namespace std;

// Ponteiro global para a fachada
unique_ptr<FachadaSSMH> g_fachada;

// Mapa de hidrômetros simulados (SHA -> Hidrômetro)
map<string, unique_ptr<Hidrometer>> g_hidrometros;

// Ponteiro global para acesso ao armazenamento (para persistir hidrômetros)
ArmazenamentoSqlite* g_armazenamento = nullptr;

void limparTela() { cout << "\033[2J\033[H"; }

void pausar() { 
    cout << "\n[ENTER para continuar...]"; 
    cin.get(); 
}

void limparBuffer() { 
    cin.clear(); 
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
}

void imprimirCabecalho() {
    cout << "╔═══════════════════════════════════════════════════════════════════╗\n";
    cout << "║        SISTEMA DE MONITORAMENTO DE HIDRÔMETROS (SSMH)            ║\n";
    cout << "║                      CAGEPA - Cliente CLI                         ║\n";
    cout << "╚═══════════════════════════════════════════════════════════════════╝\n";
}

void imprimirSecao(const string& titulo) {
    cout << "\n" << string(70, '=') << "\n  " << titulo << "\n" << string(70, '=') << "\n\n";
}

string lerLinha(const string& prompt) {
    cout << prompt;
    string linha;
    getline(cin, linha);
    return linha;
}

int lerInteiro(const string& prompt) {
    int valor;
    while (true) {
        cout << prompt;
        if (cin >> valor) { limparBuffer(); return valor; }
        cout << "❌ Entrada inválida!\n";
        limparBuffer();
    }
}

double lerDouble(const string& prompt) {
    double valor;
    while (true) {
        cout << prompt;
        if (cin >> valor) { limparBuffer(); return valor; }
        cout << "❌ Entrada inválida!\n";
        limparBuffer();
    }
}

// SIMULAÇÃO DE HIDRÔMETROS
void criarHidrometroSimulado(const string& sha, double vazaoInicial = -1.0) {
    auto hidro = make_unique<Hidrometer>();
    
    // Se não especificar vazão, usa a vazão máxima do hidrômetro
    if (vazaoInicial < 0) {
        vazaoInicial = hidro->getPipeIN()->getMaxFlow();
    }
    
    hidro->getPipeIN()->setFlowRate(vazaoInicial); // Define vazão inicial (m³/s)
    hidro->activate(); // Ativa para começar a contar
    g_hidrometros[sha] = move(hidro);
    
    // Salva no banco de dados
    if (g_armazenamento) {
        g_armazenamento->salvarHidrometroSimulado(sha, vazaoInicial, true, 0);
    }
    
    cout << "✅ Hidrômetro simulado criado: " << sha << " (vazão: " << fixed << setprecision(6) << vazaoInicial << " m³/s = " << (vazaoInicial * 1000 * 60) << " L/min)\n";
}

void carregarHidrometrosSimulados() {
    if (!g_armazenamento) return;
    
    try {
        auto hidrometros = g_armazenamento->listarHidrometrosSimulados();
        
        if (!hidrometros.empty()) {
            cout << "\n🔄 Carregando hidrômetros salvos (" << hidrometros.size() << ")...\n";
            
            for (const auto& info : hidrometros) {
                auto hidro = make_unique<Hidrometer>();
                
                // Restaura vazão
                hidro->getPipeIN()->setFlowRate(info.vazao);
                
                // Restaura contador
                hidro->setCounter(info.contador);
                
                // Restaura estado (ativo/inativo)
                if (info.ativo) {
                    hidro->activate();
                } else {
                    hidro->deactivate();
                }
                
                g_hidrometros[info.idSha] = move(hidro);
                
                cout << "  ✅ " << info.idSha 
                     << " | Vazão: " << fixed << setprecision(6) << info.vazao << " m³/s (" 
                     << (info.vazao * 1000 * 60) << " L/min)"
                     << " | Contador: " << info.contador << "L"
                     << " | Status: " << (info.ativo ? "ATIVO ✓" : "INATIVO ✗") << "\n";
            }
        }
    } catch (const exception& e) {
        cout << "⚠️  Aviso ao carregar hidrômetros: " << e.what() << "\n";
    }
}

void salvarEstadoHidrometros() {
    if (!g_armazenamento) return;
    
    for (const auto& [sha, hidro] : g_hidrometros) {
        try {
            double vazao = hidro->getPipeIN()->getFlowRate();
            bool ativo = hidro->getStatus();
            int contador = hidro->getCounter();
            
            if (g_armazenamento->hidrometroSimuladoExiste(sha)) {
                g_armazenamento->atualizarHidrometroSimulado(sha, vazao, ativo, contador);
            } else {
                g_armazenamento->salvarHidrometroSimulado(sha, vazao, ativo, contador);
            }
        } catch (const exception& e) {
            cout << "  ❌ Erro ao salvar " << sha << ": " << e.what() << "\n";
        }
    }
}

void atualizarLeiturasSimuladas() {
    // Atualiza as leituras no sistema com os valores dos hidrômetros simulados
    for (const auto& [sha, hidro] : g_hidrometros) {
        try {
            int leitura = hidro->getCounter();
            g_fachada->registrarLeituraManual(sha, leitura);
        } catch (...) {
            // Ignora erros (hidrômetro pode não estar vinculado ainda)
        }
    }
}

// GESTÃO DE USUÁRIOS
void menuCriarUsuario() {
    imprimirSecao("CRIAR NOVO USUÁRIO");
    string nome = lerLinha("Nome: ");
    string cpf = lerLinha("CPF: ");
    string email = lerLinha("E-mail: ");
    string endereco = lerLinha("Endereço: ");
    try {
        Usuario u = g_fachada->criarUsuario({{"nome",nome},{"cpf",cpf},{"email",email},{"endereco",endereco}});
        cout << "\n✅ Usuário criado! ID: " << u.getId() << "\n";
    } catch (const exception& e) {
        cout << "\n❌ Erro: " << e.what() << "\n";
    }
    pausar();
}

void menuListarUsuarios() {
    imprimirSecao("LISTA DE USUÁRIOS");
    try {
        auto usuarios = g_fachada->listarUsuarios();
        if (usuarios.empty()) {
            cout << "ℹ️  Nenhum usuário cadastrado.\n";
        } else {
            for (const auto& u : usuarios) {
                auto hidros = g_fachada->listarHidrometros(u.getId());
                cout << "• ID " << u.getId() << ": " << u.getNome() << " - " << hidros.size() << " hidrômetro(s)\n";
            }
        }
    } catch (const exception& e) {
        cout << "❌ Erro: " << e.what() << "\n";
    }
    pausar();
}

void menuVincularHidrometro() {
    imprimirSecao("VINCULAR HIDRÔMETRO");
    int id = lerInteiro("ID do usuário: ");
    string sha = lerLinha("ID do hidrômetro (SHA): ");
    
    // Cria hidrômetro simulado se não existir
    if (g_hidrometros.find(sha) == g_hidrometros.end()) {
        cout << "\nℹ️  Hidrômetro não existe. Criando simulação...\n";
        char opcao;
        cout << "Usar vazão máxima? (s/n): ";
        cin >> opcao;
        limparBuffer();
        
        if (opcao == 's' || opcao == 'S') {
            criarHidrometroSimulado(sha); // Usa vazão máxima (padrão)
        } else {
            double vazao = lerDouble("Vazão (m³/s, ex: 0.0001): ");
            criarHidrometroSimulado(sha, vazao);
        }
    }
    
    try {
        g_fachada->vincularHidrometro(id, sha);
        cout << "\n✅ Hidrômetro vinculado!\n";
    } catch (const exception& e) {
        cout << "\n❌ Erro: " << e.what() << "\n";
    }
    pausar();
}

// MONITORAMENTO
void menuRegistrarLeitura() {
    imprimirSecao("REGISTRAR LEITURA");
    string sha = lerLinha("ID do hidrômetro (SHA): ");
    int valor = lerInteiro("Valor (litros): ");
    try {
        g_fachada->registrarLeituraManual(sha, valor);
        cout << "\n✅ Leitura registrada!\n";
    } catch (const exception& e) {
        cout << "\n❌ Erro: " << e.what() << "\n";
    }
    pausar();
}

void menuAjustarVazao() {
    imprimirSecao("AJUSTAR VAZÃO DO HIDRÔMETRO");
    
    // Lista hidrômetros disponíveis
    cout << "\n📋 Hidrômetros simulados disponíveis:\n";
    for (const auto& [sha, hidro] : g_hidrometros) {
        cout << "   • " << sha 
             << " | Vazão: " << fixed << setprecision(6) << hidro->getPipeIN()->getFlowRate() << " m³/s"
             << " | Contador: " << hidro->getCounter() << "L"
             << " | Status: " << (hidro->getStatus() ? "ATIVO ✓" : "INATIVO ✗") << "\n";
    }
    cout << "\n";
    
    string sha = lerLinha("ID do hidrômetro (SHA, ex: HIDRO-001 ou 001): ");
    
    // Tenta encontrar diretamente
    if (g_hidrometros.find(sha) == g_hidrometros.end()) {
        // Se não encontrou, tenta adicionar prefixo HIDRO-
        string shaComPrefixo = "HIDRO-" + sha;
        if (g_hidrometros.find(shaComPrefixo) != g_hidrometros.end()) {
            sha = shaComPrefixo;
        } else {
            cout << "\n❌ Hidrômetro simulado não encontrado!\n";
            pausar();
            return;
        }
    }
    
    auto& hidro = g_hidrometros[sha];
    int leituraAtual = hidro->getCounter();
    bool statusAtual = hidro->getStatus();
    
    cout << "\n📊 Status atual:\n";
    cout << "   Leitura: " << leituraAtual << " L\n";
    cout << "   Status: " << (statusAtual ? "ATIVO" : "INATIVO") << "\n\n";
    
    double novaVazao = lerDouble("Nova vazão (m³/s, ex: 0.0001 ou 0 para parar): ");
    
    hidro->getPipeIN()->setFlowRate(novaVazao);
    if (novaVazao > 0 && !statusAtual) {
        hidro->activate();
        cout << "\n✅ Hidrômetro ativado com vazão: " << novaVazao << " m³/s\n";
    } else if (novaVazao == 0) {
        hidro->deactivate();
        cout << "\n✅ Hidrômetro desativado!\n";
    } else {
        cout << "\n✅ Vazão ajustada para: " << novaVazao << " m³/s\n";
    }
    
    pausar();
}

void menuConsultarConsumo() {
    imprimirSecao("CONSULTAR CONSUMO");
    
    // Atualiza leituras dos hidrômetros simulados
    atualizarLeiturasSimuladas();
    
    cout << "1 - Por hidrômetro\n2 - Por usuário\n";
    int op = lerInteiro("Opção: ");
    time_t agora = time(nullptr);
    time_t inicio = agora - (24*3600);
    try {
        if (op == 1) {
            string sha = lerLinha("ID do hidrômetro: ");
            
            // Mostra leitura atual do simulador se existir
            if (g_hidrometros.find(sha) != g_hidrometros.end()) {
                int leituraAtual = g_hidrometros[sha]->getCounter();
                cout << "📟 Leitura atual do hidrômetro: " << leituraAtual << " L\n";
            }
            
            double consumo = g_fachada->monitorarConsumoPorHidrometro(sha, inicio, agora);
            cout << "\n📊 Consumo (24h): " << fixed << setprecision(2) << consumo << " L\n";
        } else {
            int id = lerInteiro("ID do usuário: ");
            double consumo = g_fachada->monitorarConsumoPorUsuario(id, inicio, agora);
            cout << "\n📊 Consumo (24h): " << fixed << setprecision(2) << consumo << " L\n";
        }
    } catch (const exception& e) {
        cout << "\n❌ Erro: " << e.what() << "\n";
    }
    pausar();
}

// ALERTAS
void menuConfigurarAlerta() {
    imprimirSecao("CONFIGURAR ALERTA");
    int id = lerInteiro("ID do usuário: ");
    cout << "1 - Limite diário\n2 - Média móvel\n";
    int op = lerInteiro("Tipo: ");
    string tipo = (op == 1) ? "LIMITE_DIARIO" : "MEDIA_MOVEL";
    string param = lerLinha("Valor do parâmetro: ");
    try {
        g_fachada->configurarRegraDeAlerta(id, tipo, param);
        cout << "\n✅ Regra configurada!\n";
    } catch (const exception& e) {
        cout << "\n❌ Erro: " << e.what() << "\n";
    }
    pausar();
}

void menuListarAlertas() {
    imprimirSecao("ALERTAS ATIVOS");
    try {
        auto alertas = g_fachada->listarAlertasAtivos();
        if (alertas.empty()) {
            cout << "✅ Nenhum alerta ativo.\n";
        } else {
            for (const auto& a : alertas) {
                cout << "🚨 ID " << a.getId() << ": " << a.getMensagem() << "\n";
            }
        }
    } catch (const exception& e) {
        cout << "❌ Erro: " << e.what() << "\n";
    }
    pausar();
}

// MENUS
void menuUsuarios() {
    while (true) {
        limparTela(); imprimirCabecalho(); imprimirSecao("GESTÃO DE USUÁRIOS");
        cout << "1 - Criar usuário\n2 - Listar usuários\n3 - Vincular hidrômetro\n0 - Voltar\n";
        int op = lerInteiro("\nOpção: ");
        if (op == 0) return;
        limparTela(); imprimirCabecalho();
        if (op == 1) menuCriarUsuario();
        else if (op == 2) menuListarUsuarios();
        else if (op == 3) menuVincularHidrometro();
    }
}

void menuMonitoramento() {
    while (true) {
        limparTela(); imprimirCabecalho(); imprimirSecao("MONITORAMENTO");
        cout << "1 - Registrar leitura manual\n";
        cout << "2 - Consultar consumo\n";
        cout << "3 - Ajustar vazão do hidrômetro\n";
        cout << "0 - Voltar\n";
        int op = lerInteiro("\nOpção: ");
        if (op == 0) return;
        limparTela(); imprimirCabecalho();
        if (op == 1) menuRegistrarLeitura();
        else if (op == 2) menuConsultarConsumo();
        else if (op == 3) menuAjustarVazao();
    }
}

void menuAlertas() {
    while (true) {
        limparTela(); imprimirCabecalho(); imprimirSecao("ALERTAS");
        cout << "1 - Configurar regra\n2 - Listar alertas\n0 - Voltar\n";
        int op = lerInteiro("\nOpção: ");
        if (op == 0) return;
        limparTela(); imprimirCabecalho();
        if (op == 1) menuConfigurarAlerta();
        else if (op == 2) menuListarAlertas();
    }
}

int main() {
    limparTela(); imprimirCabecalho();
    cout << "\n�� Inicializando Sistema...\n\n";
    try {
        auto armazenamento = make_unique<ArmazenamentoSqlite>("usuarios.db");
        g_armazenamento = armazenamento.get(); // Salva ponteiro para uso global
        
        auto usuarioService = make_shared<UsuarioService>(move(armazenamento));
        auto ocr = make_shared<AdaptadorOCR>();
        auto leituraDAO = make_shared<LeituraDAOMemoria>();
        auto monitoramentoService = make_shared<MonitoramentoService>(ocr, leituraDAO);
        auto alertaService = AlertaServiceFactory::criarPadrao();
        alertaService->anexarObserver(make_shared<PainelObserver>());
        alertaService->anexarObserver(make_shared<LoggerObserver>());
        
        g_fachada = make_unique<FachadaSSMH>(usuarioService, monitoramentoService, alertaService);
        Logger::getInstance().setArquivoLog("ssmh.log");
        Logger::setDebugMode(false);
        g_fachada->inicializar();
        
        // Carrega hidrômetros salvos ou cria novos
        carregarHidrometrosSimulados();
        
        if (g_hidrometros.empty()) {
            // Cria hidrômetros simulados iniciais apenas se não houver salvos
            // Usa SHAs numéricos para compatibilidade com vínculos
            cout << "\n💧 Criando hidrômetros simulados com vazão máxima...\n";
            criarHidrometroSimulado("1");  // Vazão máxima
            criarHidrometroSimulado("2");  // Vazão máxima
            criarHidrometroSimulado("3");  // Vazão máxima
        }
        
        cout << "\n✅ Sistema inicializado!\n";
        cout << "ℹ️  Os hidrômetros estão contando em tempo real!\n";
        this_thread::sleep_for(chrono::seconds(2));
        
        while (true) {
            limparTela(); imprimirCabecalho();
            cout << "\n📋 MENU PRINCIPAL\n\n";
            cout << "1 - 👤 Gestão de Usuários\n";
            cout << "2 - 💧 Monitoramento\n";
            cout << "3 - ⚠️  Alertas\n";
            cout << "0 - 🚪 Sair\n";
            int op = lerInteiro("\nOpção: ");
            
            if (op == 0) {
                limparTela(); imprimirCabecalho();
                cout << "\n🏁 Encerrando...\n";
                
                // Salva estado dos hidrômetros antes de desligar
                cout << "💾 Salvando estado dos hidrômetros...\n";
                salvarEstadoHidrometros();
                
                // Desativa todos os hidrômetros
                for (auto& [sha, hidro] : g_hidrometros) {
                    hidro->shutdown();
                }
                g_hidrometros.clear();
                
                g_fachada->finalizar();
                cout << "✅ Sistema finalizado!\n\n";
                return 0;
            }
            if (op == 1) menuUsuarios();
            else if (op == 2) menuMonitoramento();
            else if (op == 3) menuAlertas();
        }
    } catch (const exception& e) {
        cerr << "\n❌ Erro fatal: " << e.what() << "\n\n";
        return 1;
    }
}
