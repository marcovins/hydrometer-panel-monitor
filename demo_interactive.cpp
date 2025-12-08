#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <limits>
#include "src/simulator/hidrometer_manager.hpp"
#include "src/usuarios/domain/usuario.hpp"
#include "src/utils/logger.hpp"

void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausar() {
    std::cout << "\nPressione ENTER para continuar...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void exibirCabecalho() {
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║    MONITOR INTERATIVO DE HIDROMETROS              ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝\n" << std::endl;
}

void exibirMenuPrincipal() {
    std::cout << "\n┌─────────────── MENU PRINCIPAL ────────────────┐" << std::endl;
    std::cout << "│                                                │" << std::endl;
    std::cout << "│  1. Listar todos os hidrometros               │" << std::endl;
    std::cout << "│  2. Monitorar hidrometro especifico           │" << std::endl;
    std::cout << "│  3. Adicionar novo hidrometro                 │" << std::endl;
    std::cout << "│  4. Remover hidrometro                        │" << std::endl;
    std::cout << "│  5. Monitoramento em tempo real               │" << std::endl;
    std::cout << "│  0. Sair                                       │" << std::endl;
    std::cout << "│                                                │" << std::endl;
    std::cout << "└────────────────────────────────────────────────┘" << std::endl;
    std::cout << "\nEscolha uma opcao: ";
}

void listarTodosHidrometros(const HidrometerManager& manager) {
    limparTela();
    exibirCabecalho();
    std::cout << "═══ LISTA DE TODOS OS HIDROMETROS ═══\n" << std::endl;
    manager.exibirStatus();
    pausar();
}

void monitorarHidrometroEspecifico(const HidrometerManager& manager) {
    limparTela();
    exibirCabecalho();
    
    int idUsuario;
    std::string idSha;
    
    std::cout << "═══ MONITORAR HIDROMETRO ESPECIFICO ═══\n" << std::endl;
    std::cout << "ID do Usuario: ";
    std::cin >> idUsuario;
    
    auto hidrometros = manager.listarHidrometrosUsuario(idUsuario);
    if (hidrometros.empty()) {
        std::cout << "\n[ERRO] Usuario " << idUsuario << " nao possui hidrometros!" << std::endl;
        pausar();
        return;
    }
    
    std::cout << "\nHidrometros disponiveis para o usuario " << idUsuario << ":" << std::endl;
    for (size_t i = 0; i < hidrometros.size(); i++) {
        std::cout << "  " << (i+1) << ". " << hidrometros[i] << std::endl;
    }
    
    std::cout << "\nEscolha o numero do hidrometro: ";
    int escolha;
    std::cin >> escolha;
    
    if (escolha < 1 || escolha > static_cast<int>(hidrometros.size())) {
        std::cout << "\n[ERRO] Opcao invalida!" << std::endl;
        pausar();
        return;
    }
    
    idSha = hidrometros[escolha - 1];
    
    limparTela();
    exibirCabecalho();
    std::cout << "═══ MONITORANDO HIDROMETRO ═══\n" << std::endl;
    std::cout << "Usuario: " << idUsuario << std::endl;
    std::cout << "Hidrometro: " << idSha << std::endl;
    std::cout << "\nAtualizando a cada 1 segundo (Pressione Ctrl+C para voltar)...\n" << std::endl;
    
    for (int i = 0; i < 20; i++) {
        int leitura = manager.obterLeitura(idUsuario, idSha);
        
        std::cout << "\r[" << std::setfill('0') << std::setw(2) << (i+1) << "] "
                  << "Leitura atual: " << std::setfill(' ') << std::setw(6) << leitura 
                  << " litros" << std::flush;
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    std::cout << "\n\nMonitoramento concluido!" << std::endl;
    pausar();
}

void adicionarHidrometro(HidrometerManager& manager) {
    limparTela();
    exibirCabecalho();
    
    int idUsuario;
    std::string idSha;
    
    std::cout << "═══ ADICIONAR NOVO HIDROMETRO ═══\n" << std::endl;
    std::cout << "ID do Usuario: ";
    std::cin >> idUsuario;
    std::cout << "ID SHA do Hidrometro: ";
    std::cin >> idSha;
    
    if (manager.adicionarHidrometro(idUsuario, idSha)) {
        std::cout << "\n[SUCESSO] Hidrometro " << idSha 
                  << " adicionado para usuario " << idUsuario << "!" << std::endl;
    } else {
        std::cout << "\n[ERRO] Nao foi possivel adicionar o hidrometro!" << std::endl;
        std::cout << "Verifique se o usuario nao atingiu o limite ou se o hidrometro ja existe." << std::endl;
    }
    
    pausar();
}

void removerHidrometro(HidrometerManager& manager) {
    limparTela();
    exibirCabecalho();
    
    int idUsuario;
    
    std::cout << "═══ REMOVER HIDROMETRO ═══\n" << std::endl;
    std::cout << "ID do Usuario: ";
    std::cin >> idUsuario;
    
    auto hidrometros = manager.listarHidrometrosUsuario(idUsuario);
    if (hidrometros.empty()) {
        std::cout << "\n[ERRO] Usuario " << idUsuario << " nao possui hidrometros!" << std::endl;
        pausar();
        return;
    }
    
    std::cout << "\nHidrometros do usuario " << idUsuario << ":" << std::endl;
    for (size_t i = 0; i < hidrometros.size(); i++) {
        std::cout << "  " << (i+1) << ". " << hidrometros[i] << std::endl;
    }
    
    std::cout << "\nEscolha o numero do hidrometro a remover: ";
    int escolha;
    std::cin >> escolha;
    
    if (escolha < 1 || escolha > static_cast<int>(hidrometros.size())) {
        std::cout << "\n[ERRO] Opcao invalida!" << std::endl;
        pausar();
        return;
    }
    
    std::string idSha = hidrometros[escolha - 1];
    
    if (manager.removerHidrometro(idUsuario, idSha)) {
        std::cout << "\n[SUCESSO] Hidrometro " << idSha << " removido!" << std::endl;
    } else {
        std::cout << "\n[ERRO] Nao foi possivel remover o hidrometro!" << std::endl;
    }
    
    pausar();
}

void monitoramentoTempoReal(const HidrometerManager& manager) {
    limparTela();
    
    std::cout << "═══ MONITORAMENTO EM TEMPO REAL ═══\n" << std::endl;
    std::cout << "Atualizando a cada 2 segundos (Pressione Ctrl+C para voltar)...\n" << std::endl;
    
    for (int i = 0; i < 30; i++) {
        limparTela();
        exibirCabecalho();
        std::cout << "═══ MONITORAMENTO EM TEMPO REAL ═══" << std::endl;
        std::cout << "Atualizacao #" << (i+1) << " | Total ativos: " 
                  << manager.getTotalHidrometrosAtivos() << " hidrometros\n" << std::endl;
        
        manager.exibirStatus();
        
        std::cout << "\n[Atualizando em 2 segundos... Pressione Ctrl+C para voltar]" << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    std::cout << "\n\nMonitoramento concluido!" << std::endl;
    pausar();
}

int main() {
    Logger::setDebugMode(false);
    Logger::setRuntimeMode(true);
    
    limparTela();
    exibirCabecalho();
    
    std::cout << "Inicializando sistema..." << std::endl;
    
    HidrometerManager manager;
    
    // Adiciona hidrometros de exemplo
    std::cout << "\nAdicionando hidrometros de exemplo..." << std::endl;
    manager.adicionarHidrometro(1, "SHA-001");
    manager.adicionarHidrometro(1, "SHA-002");
    manager.adicionarHidrometro(2, "SHA-101");
    manager.adicionarHidrometro(3, "SHA-201");
    
    std::cout << "Iniciando todos os hidrometros..." << std::endl;
    manager.iniciarTodos();
    
    std::cout << "\n[OK] Sistema pronto!" << std::endl;
    pausar();
    
    bool executando = true;
    while (executando) {
        limparTela();
        exibirCabecalho();
        
        std::cout << "Status: " << manager.getTotalHidrometrosAtivos() 
                  << " hidrometros ativos" << std::endl;
        
        exibirMenuPrincipal();
        
        int opcao;
        std::cin >> opcao;
        
        switch (opcao) {
            case 1:
                listarTodosHidrometros(manager);
                break;
            case 2:
                monitorarHidrometroEspecifico(manager);
                break;
            case 3:
                adicionarHidrometro(manager);
                break;
            case 4:
                removerHidrometro(manager);
                break;
            case 5:
                monitoramentoTempoReal(manager);
                break;
            case 0:
                executando = false;
                break;
            default:
                std::cout << "\n[ERRO] Opcao invalida!" << std::endl;
                pausar();
        }
    }
    
    limparTela();
    exibirCabecalho();
    std::cout << "Finalizando sistema..." << std::endl;
    manager.pararTodos();
    std::cout << "\n[OK] Sistema encerrado com sucesso!" << std::endl;
    
    return 0;
}
