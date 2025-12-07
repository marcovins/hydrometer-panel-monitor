/**
 * @file test_monitoramento.cpp
 * @brief Testes do subsistema de monitoramento
 * 
 * Demonstra o funcionamento dos padrões Composite e Adapter
 * no contexto de coleta e monitoramento de consumo de água.
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <ctime>
#include "src/monitoramento/services/monitoramento_service_factory.hpp"
#include "src/monitoramento/domain/leitura.hpp"
#include "src/utils/logger.hpp"

using namespace std;

// Funções auxiliares
void imprimirSeparador() {
    cout << "\n" << string(70, '=') << "\n";
}

void imprimirTitulo(const string& titulo) {
    imprimirSeparador();
    cout << "  " << titulo << "\n";
    imprimirSeparador();
}

time_t obterDataHoraAtual() {
    return time(nullptr);
}

time_t obterDataHoraPassado(int horas) {
    return time(nullptr) - (horas * 3600);
}

void testarProcessamentoOCR() {
    imprimirTitulo("TESTE 1: Processamento OCR (Padrão Adapter)");
    
    cout << "\nCriando serviço de monitoramento...\n";
    auto servico = MonitoramentoServiceFactory::criar();
    
    cout << "\nSimulando processamento de imagens de hidrômetros:\n";
    
    // Simula processamento de várias imagens
    vector<string> imagens = {
        "medicoes_202311250013/leitura_sha001_100.jpg",
        "medicoes_202311250013/leitura_sha001_125.jpg",
        "medicoes_202311250013/leitura_sha001_150.jpg"
    };
    
    string idSha = "SHA001";
    
    for (const auto& imagem : imagens) {
        cout << "\n  → Processando: " << imagem << "\n";
        
        // O AdaptadorOCR (Adapter) converte a interface da biblioteca
        // OCR externa para a interface esperada pelo sistema
        try {
            int idLeitura = servico->processarLeitura(idSha, imagem);
            cout << "    ✓ Leitura processada com ID: " << idLeitura << "\n";
        } catch (const exception& e) {
            cout << "    ✗ Erro: " << e.what() << "\n";
        }
    }
    
    cout << "\n📊 Total de leituras para " << idSha << ": " 
         << servico->contarLeituras(idSha) << "\n";
}

void testarLeiturasManuais() {
    imprimirTitulo("TESTE 2: Registro de Leituras Manuais");
    
    auto servico = MonitoramentoServiceFactory::criar();
    
    cout << "\nRegistrando leituras manuais para múltiplos hidrômetros:\n";
    
    // SHA001 - Residência A
    cout << "\n  🏠 Hidrômetro SHA001 (Residência A):\n";
    servico->registrarLeituraManual("SHA001", 100);
    servico->registrarLeituraManual("SHA001", 125);
    servico->registrarLeituraManual("SHA001", 155);
    cout << "    → 3 leituras registradas\n";
    
    // SHA002 - Residência B
    cout << "\n  🏠 Hidrômetro SHA002 (Residência B):\n";
    servico->registrarLeituraManual("SHA002", 200);
    servico->registrarLeituraManual("SHA002", 240);
    servico->registrarLeituraManual("SHA002", 290);
    cout << "    → 3 leituras registradas\n";
    
    // SHA003 - Comércio
    cout << "\n  🏢 Hidrômetro SHA003 (Comércio):\n";
    servico->registrarLeituraManual("SHA003", 500);
    servico->registrarLeituraManual("SHA003", 620);
    servico->registrarLeituraManual("SHA003", 755);
    cout << "    → 3 leituras registradas\n";
}

void testarCompositeHidrometro() {
    imprimirTitulo("TESTE 3: Consumo de Hidrômetro Individual (Composite - Leaf)");
    
    auto servico = MonitoramentoServiceFactory::criar();
    
    // Registra leituras para teste
    string idSha = "SHA100";
    servico->registrarLeituraManual(idSha, 100);
    servico->registrarLeituraManual(idSha, 150);
    servico->registrarLeituraManual(idSha, 200);
    
    cout << "\nConstruindo objeto ConsumoHidrometro (Leaf)...\n";
    
    // Constrói o objeto Composite (neste caso, um Leaf)
    auto consumoHidrometro = servico->construirConsumoHidrometro(idSha);
    
    cout << "  → Identificador: " << consumoHidrometro->obterIdentificador() << "\n";
    cout << "  → Descrição: " << consumoHidrometro->obterDescricao() << "\n";
    
    // Calcula consumo
    time_t inicio = obterDataHoraPassado(24);
    time_t fim = obterDataHoraAtual();
    
    double consumo = consumoHidrometro->calcularConsumo(inicio, fim);
    
    cout << "\n📊 Consumo calculado: " << fixed << setprecision(2) 
         << consumo << " litros\n";
}

void testarCompositeUsuario() {
    imprimirTitulo("TESTE 4: Consumo de Usuário (Composite - Composite)");
    
    auto servico = MonitoramentoServiceFactory::criar();
    
    cout << "\nCenário: Usuário #42 possui 3 hidrômetros\n";
    
    // Registra leituras para cada hidrômetro do usuário
    vector<string> hidrometrosUsuario = {"SHA201", "SHA202", "SHA203"};
    
    cout << "\nRegistrando leituras:\n";
    
    // SHA201 - Casa principal
    servico->registrarLeituraManual("SHA201", 100);
    servico->registrarLeituraManual("SHA201", 150);
    cout << "  → SHA201 (Casa): 50L\n";
    
    // SHA202 - Edícula
    servico->registrarLeituraManual("SHA202", 200);
    servico->registrarLeituraManual("SHA202", 230);
    cout << "  → SHA202 (Edícula): 30L\n";
    
    // SHA203 - Jardim
    servico->registrarLeituraManual("SHA203", 300);
    servico->registrarLeituraManual("SHA203", 350);
    cout << "  → SHA203 (Jardim): 50L\n";
    
    cout << "\nConstruindo objeto ConsumoUsuario (Composite)...\n";
    
    // Constrói o Composite que agrega os consumos
    auto consumoUsuario = servico->construirConsumoUsuario(42, hidrometrosUsuario);
    
    cout << "  → Identificador: " << consumoUsuario->obterIdentificador() << "\n";
    cout << "  → Descrição: " << consumoUsuario->obterDescricao() << "\n";
    
    // O padrão Composite permite tratar um conjunto como se fosse um único elemento
    time_t inicio = obterDataHoraPassado(24);
    time_t fim = obterDataHoraAtual();
    
    double consumoTotal = consumoUsuario->calcularConsumo(inicio, fim);
    
    cout << "\n📊 Consumo total do usuário: " << fixed << setprecision(2) 
         << consumoTotal << " litros\n";
    cout << "    (Soma automática de todos os hidrômetros)\n";
}

void testarConsultasAvancadas() {
    imprimirTitulo("TESTE 5: Consultas Avançadas");
    
    auto servico = MonitoramentoServiceFactory::criar();
    
    // Prepara dados
    string sha1 = "SHA301";
    string sha2 = "SHA302";
    
    cout << "\nPreparando dados de teste...\n";
    
    // Simulando consumo ao longo do tempo
    servico->registrarLeituraManual(sha1, 1000);
    servico->registrarLeituraManual(sha1, 1025);  // +25L
    servico->registrarLeituraManual(sha1, 1055);  // +30L
    servico->registrarLeituraManual(sha1, 1100);  // +45L
    
    servico->registrarLeituraManual(sha2, 2000);
    servico->registrarLeituraManual(sha2, 2040);  // +40L
    servico->registrarLeituraManual(sha2, 2095);  // +55L
    
    time_t inicio = obterDataHoraPassado(24);
    time_t fim = obterDataHoraAtual();
    
    // Teste 1: Consulta individual
    cout << "\n1. Consumo individual:\n";
    double consumo1 = servico->consultarConsumoHidrometro(sha1, inicio, fim);
    double consumo2 = servico->consultarConsumoHidrometro(sha2, inicio, fim);
    
    cout << "   " << sha1 << ": " << consumo1 << "L\n";
    cout << "   " << sha2 << ": " << consumo2 << "L\n";
    
    // Teste 2: Consulta agregada
    cout << "\n2. Consumo agregado (múltiplos hidrômetros):\n";
    vector<string> shas = {sha1, sha2};
    double consumoAgregado = servico->consultarConsumoAgregado(shas, inicio, fim);
    
    cout << "   Total: " << consumoAgregado << "L\n";
    
    // Teste 3: Consumo recente
    cout << "\n3. Consumo recente (últimas 24h):\n";
    double consumoRecente1 = servico->calcularConsumoRecente(sha1, 24);
    double consumoRecente2 = servico->calcularConsumoRecente(sha2, 24);
    
    cout << "   " << sha1 << ": " << consumoRecente1 << "L\n";
    cout << "   " << sha2 << ": " << consumoRecente2 << "L\n";
    
    // Teste 4: Obter leituras detalhadas
    cout << "\n4. Histórico de leituras (" << sha1 << "):\n";
    auto leituras = servico->obterLeituras(sha1, inicio, fim);
    
    for (const auto& leitura : leituras) {
        cout << "   • " << leitura.getDataHoraFormatada() 
             << " - " << leitura.getValor() << "L\n";
    }
}

void testarPadroesIntegrados() {
    imprimirTitulo("TESTE 6: Integração dos Padrões (Adapter + Composite)");
    
    auto servico = MonitoramentoServiceFactory::criar();
    
    cout << "\nDemonstrando uso integrado dos padrões:\n";
    cout << "\n1️⃣  Adapter: Processamento OCR\n";
    cout << "   → Converte imagem em valor numérico\n";
    
    cout << "\n2️⃣  Composite: Agregação transparente\n";
    cout << "   → Trata hidrômetro individual e grupo de forma uniforme\n";
    
    cout << "\n3️⃣  Factory: Criação simplificada\n";
    cout << "   → Configura todas as dependências automaticamente\n";
    
    cout << "\nExemplo prático:\n";
    
    // Cria dados para 2 usuários
    cout << "\n👤 Usuário 1 (residencial):\n";
    servico->registrarLeituraManual("U1-SHA1", 100);
    servico->registrarLeituraManual("U1-SHA1", 150);
    
    auto consumoU1 = servico->construirConsumoUsuario(1, {"U1-SHA1"});
    double totalU1 = consumoU1->calcularConsumo(obterDataHoraPassado(24), obterDataHoraAtual());
    cout << "   Consumo: " << totalU1 << "L\n";
    
    cout << "\n👤 Usuário 2 (comercial - 2 hidrômetros):\n";
    servico->registrarLeituraManual("U2-SHA1", 500);
    servico->registrarLeituraManual("U2-SHA1", 620);
    servico->registrarLeituraManual("U2-SHA2", 300);
    servico->registrarLeituraManual("U2-SHA2", 380);
    
    auto consumoU2 = servico->construirConsumoUsuario(2, {"U2-SHA1", "U2-SHA2"});
    double totalU2 = consumoU2->calcularConsumo(obterDataHoraPassado(24), obterDataHoraAtual());
    cout << "   Consumo: " << totalU2 << "L (soma de 2 hidrômetros)\n";
    
    cout << "\n✅ Mesma interface para ambos os casos (ConsumoMonitoravel)!\n";
}

void exibirResumo() {
    imprimirTitulo("RESUMO DOS PADRÕES IMPLEMENTADOS");
    
    cout << "\n📐 PADRÃO ADAPTER:\n";
    cout << "   ├─ Interface: ProcessadorOCR\n";
    cout << "   ├─ Adapter: AdaptadorOCR\n";
    cout << "   └─ Finalidade: Adaptar biblioteca OCR externa\n";
    
    cout << "\n🌳 PADRÃO COMPOSITE:\n";
    cout << "   ├─ Component: ConsumoMonitoravel\n";
    cout << "   ├─ Leaf: ConsumoHidrometro\n";
    cout << "   ├─ Composite: ConsumoUsuario\n";
    cout << "   └─ Finalidade: Agregação transparente de consumo\n";
    
    cout << "\n🏭 PADRÃO FACTORY:\n";
    cout << "   ├─ Factory: MonitoramentoServiceFactory\n";
    cout << "   └─ Finalidade: Criação simplificada do serviço\n";
    
    cout << "\n💾 PERSISTÊNCIA:\n";
    cout << "   ├─ Interface: LeituraDAO\n";
    cout << "   ├─ Implementação: LeituraDAOMemoria\n";
    cout << "   └─ Entidade: Leitura\n";
    
    cout << "\n🎯 SERVIÇO PRINCIPAL:\n";
    cout << "   └─ MonitoramentoService\n";
    cout << "      ├─ Coordena OCR e persistência\n";
    cout << "      ├─ Constrói estruturas Composite\n";
    cout << "      └─ Interface para a Fachada\n";
    
    imprimirSeparador();
}

int main() {
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    cout << "║       TESTE DO SUBSISTEMA DE COLETA E MONITORAMENTO              ║\n";
    cout << "║                                                                    ║\n";
    cout << "║  Padrões: Adapter, Composite, Factory                            ║\n";
    cout << "║  Autor: Marcos Belo                                              ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    
    try {
        // Configura o logger
        Logger::getInstance().setArquivoLog("test_monitoramento.log");
        
        // Executa testes
        testarProcessamentoOCR();
        testarLeiturasManuais();
        testarCompositeHidrometro();
        testarCompositeUsuario();
        testarConsultasAvancadas();
        testarPadroesIntegrados();
        exibirResumo();
        
        imprimirTitulo("TODOS OS TESTES CONCLUÍDOS COM SUCESSO! ✅");
        cout << "\n";
        
    } catch (const exception& e) {
        cerr << "\n❌ ERRO: " << e.what() << "\n\n";
        return 1;
    }
    
    return 0;
}
