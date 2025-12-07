#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include "src/simulator/hidrometer_manager.hpp"
#include "src/usuarios/services/usuario_service_factory.hpp"
#include "src/usuarios/commands/user_commands.hpp"
#include "src/usuarios/commands/command_invoker.hpp"
#include "src/utils/logger.hpp"

// Variáveis globais para controle
HidrometerManager* globalManager = nullptr;
std::atomic<bool> shutdownRequested(false);

// Handler para Ctrl+C
void signalHandler(int signal) {
    if (signal == SIGINT) {
        bool expected = false;
        if (!shutdownRequested.compare_exchange_strong(expected, true)) {
            return; // Já está finalizando
        }
        
        std::cout << "\n\n[INFO] Ctrl+C detectado - Finalizando sistema...\n" << std::endl;
        
        if (globalManager != nullptr) {
            globalManager->pararTodos();
        }
        
        exit(0);
    }
}

void exibirMenu() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  SISTEMA DE MONITORAMENTO - MENU" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. Exibir status dos hidrometros" << std::endl;
    std::cout << "2. Adicionar hidrometro a um usuario" << std::endl;
    std::cout << "3. Remover hidrometro de um usuario" << std::endl;
    std::cout << "4. Listar usuarios cadastrados" << std::endl;
    std::cout << "5. Ver leitura de um hidrometro" << std::endl;
    std::cout << "0. Sair" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Escolha uma opcao: ";
}

int main() {
    // Configura signal handler
    signal(SIGINT, signalHandler);
    
    Logger::setDebugMode(false);
    Logger::setRuntimeMode(true);
    
    std::cout << "========================================" << std::endl;
    std::cout << "SISTEMA DE MONITORAMENTO DE HIDROMETROS" << std::endl;
    std::cout << "     COM MULTIPLOS USUARIOS" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Cria serviço de usuários (em memória - sem precisar do SQLite)
    auto armazenamento = std::make_unique<ArmazenamentoVolatil>();
    auto usuarioService = std::make_unique<UsuarioService>(std::move(armazenamento));
    CommandInvoker invoker;
    
    // Cria gerenciador de hidrômetros
    HidrometerManager manager;
    globalManager = &manager;
    
    // Cria usuários de teste
    std::cout << "[SETUP] Criando usuarios de teste...\n" << std::endl;
    
    std::map<std::string, std::string> dadosUser1;
    dadosUser1["nome"] = "Maria Silva";
    dadosUser1["email"] = "maria@exemplo.com";
    dadosUser1["tipoPerfil"] = "ADMIN";
    auto cmdUser1 = std::make_unique<CriarUsuarioCommand>(usuarioService.get(), dadosUser1);
    invoker.executarComando(std::move(cmdUser1));
    
    std::map<std::string, std::string> dadosUser2;
    dadosUser2["nome"] = "Joao Santos";
    dadosUser2["email"] = "joao@exemplo.com";
    dadosUser2["tipoPerfil"] = "LEITOR";
    auto cmdUser2 = std::make_unique<CriarUsuarioCommand>(usuarioService.get(), dadosUser2);
    invoker.executarComando(std::move(cmdUser2));
    
    std::map<std::string, std::string> dadosUser3;
    dadosUser3["nome"] = "Ana Costa";
    dadosUser3["email"] = "ana@exemplo.com";
    dadosUser3["tipoPerfil"] = "LEITOR";
    auto cmdUser3 = std::make_unique<CriarUsuarioCommand>(usuarioService.get(), dadosUser3);
    invoker.executarComando(std::move(cmdUser3));
    
    std::cout << "[SETUP] Usuarios criados com sucesso!\n" << std::endl;
    
    // Adiciona hidrômetros para os usuários
    std::cout << "[SETUP] Configurando hidrometros...\n" << std::endl;
    
    // Usuario 1: 3 hidrômetros
    manager.adicionarHidrometro(1, "SHA-001");
    manager.adicionarHidrometro(1, "SHA-002");
    manager.adicionarHidrometro(1, "SHA-003");
    
    // Usuario 2: 2 hidrômetros
    manager.adicionarHidrometro(2, "SHA-101");
    manager.adicionarHidrometro(2, "SHA-102");
    
    // Usuario 3: 1 hidrômetro
    manager.adicionarHidrometro(3, "SHA-201");
    
    std::cout << "[SETUP] Hidrometros configurados!\n" << std::endl;
    
    // Inicia todos os hidrômetros
    std::cout << "[SISTEMA] Iniciando todos os hidrometros...\n" << std::endl;
    manager.iniciarTodos();
    
    std::cout << "\n[SISTEMA] Sistema operacional! Use Ctrl+C para sair.\n" << std::endl;
    
    // Loop principal
    bool running = true;
    while (running && !shutdownRequested) {
        exibirMenu();
        
        int opcao;
        std::cin >> opcao;
        
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\n[ERRO] Opcao invalida!\n" << std::endl;
            continue;
        }
        
        switch (opcao) {
            case 0:
                running = false;
                break;
                
            case 1: {
                manager.exibirStatus();
                break;
            }
            
            case 2: {
                int idUsuario;
                std::string idSha;
                
                std::cout << "\nID do usuario: ";
                std::cin >> idUsuario;
                std::cout << "ID do hidrometro (ex: SHA-XXX): ";
                std::cin >> idSha;
                
                if (manager.adicionarHidrometro(idUsuario, idSha)) {
                    std::cout << "\n[OK] Hidrometro adicionado com sucesso!\n" << std::endl;
                } else {
                    std::cout << "\n[ERRO] Nao foi possivel adicionar o hidrometro!\n" << std::endl;
                }
                break;
            }
            
            case 3: {
                int idUsuario;
                std::string idSha;
                
                std::cout << "\nID do usuario: ";
                std::cin >> idUsuario;
                std::cout << "ID do hidrometro: ";
                std::cin >> idSha;
                
                if (manager.removerHidrometro(idUsuario, idSha)) {
                    std::cout << "\n[OK] Hidrometro removido com sucesso!\n" << std::endl;
                } else {
                    std::cout << "\n[ERRO] Hidrometro nao encontrado!\n" << std::endl;
                }
                break;
            }
            
            case 4: {
                auto usuarios = usuarioService->listarUsuarios();
                std::cout << "\n========== USUARIOS CADASTRADOS ==========" << std::endl;
                for (const auto& user : usuarios) {
                    std::cout << "ID: " << user.getId() 
                              << " | Nome: " << user.getNome()
                              << " | Email: " << user.getEmail()
                              << " | Perfil: " << Usuario::tipoPerfilToString(user.getTipoPerfil())
                              << std::endl;
                }
                std::cout << "===========================================\n" << std::endl;
                break;
            }
            
            case 5: {
                int idUsuario;
                std::string idSha;
                
                std::cout << "\nID do usuario: ";
                std::cin >> idUsuario;
                std::cout << "ID do hidrometro: ";
                std::cin >> idSha;
                
                int leitura = manager.obterLeitura(idUsuario, idSha);
                if (leitura >= 0) {
                    std::cout << "\n[LEITURA] Hidrometro " << idSha 
                              << " do usuario " << idUsuario 
                              << ": " << leitura << " litros\n" << std::endl;
                } else {
                    std::cout << "\n[ERRO] Hidrometro nao encontrado!\n" << std::endl;
                }
                break;
            }
            
            default:
                std::cout << "\n[ERRO] Opcao invalida!\n" << std::endl;
                break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Finalização
    std::cout << "\n[SISTEMA] Parando todos os hidrometros..." << std::endl;
    manager.pararTodos();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  SISTEMA FINALIZADO COM SUCESSO" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    return 0;
}
