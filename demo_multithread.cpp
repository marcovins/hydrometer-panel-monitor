#include <iostream>
#include <thread>
#include <chrono>
#include "src/simulator/hidrometer_manager.hpp"
#include "src/usuarios/domain/usuario.hpp"
#include "src/utils/logger.hpp"

int main() {
    Logger::setDebugMode(false);
    Logger::setRuntimeMode(true);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  DEMO: HIDROMETROS MULTITHREAD" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Cria gerenciador
    HidrometerManager manager;
    
    std::cout << "[1] Adicionando hidrometros para 3 usuarios...\n" << std::endl;
    
    // Usuario 1: 3 hidrômetros
    manager.adicionarHidrometro(1, "SHA-001");
    manager.adicionarHidrometro(1, "SHA-002");
    manager.adicionarHidrometro(1, "SHA-003");
    std::cout << "   Usuario 1: 3 hidrometros (SHA-001, SHA-002, SHA-003)" << std::endl;
    
    // Usuario 2: 2 hidrômetros
    manager.adicionarHidrometro(2, "SHA-101");
    manager.adicionarHidrometro(2, "SHA-102");
    std::cout << "   Usuario 2: 2 hidrometros (SHA-101, SHA-102)" << std::endl;
    
    // Usuario 3: 1 hidrômetro
    manager.adicionarHidrometro(3, "SHA-201");
    std::cout << "   Usuario 3: 1 hidrometro (SHA-201)" << std::endl;
    
    std::cout << "\n[2] Iniciando todos os hidrometros (6 threads)...\n" << std::endl;
    manager.iniciarTodos();
    
    std::cout << "[3] Status inicial:" << std::endl;
    manager.exibirStatus();
    
    std::cout << "\n[4] Aguardando 5 segundos para acumular leituras..." << std::endl;
    for (int i = 5; i > 0; i--) {
        std::cout << "   " << i << "..." << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << " Leituras: U1=" << manager.obterLeitura(1, "SHA-001") 
                  << "L, U2=" << manager.obterLeitura(2, "SHA-101") 
                  << "L, U3=" << manager.obterLeitura(3, "SHA-201") << "L" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    std::cout << "\n[5] Status apos 5 segundos:" << std::endl;
    manager.exibirStatus();
    
    std::cout << "\n[6] Leituras individuais:" << std::endl;
    std::cout << "   Usuario 1, SHA-001: " << manager.obterLeitura(1, "SHA-001") << " litros" << std::endl;
    std::cout << "   Usuario 2, SHA-101: " << manager.obterLeitura(2, "SHA-101") << " litros" << std::endl;
    std::cout << "   Usuario 3, SHA-201: " << manager.obterLeitura(3, "SHA-201") << " litros" << std::endl;
    
    std::cout << "\n[7] Testando limite: tentando adicionar 6º hidrometro ao usuario 1..." << std::endl;
    if (!manager.adicionarHidrometro(1, "SHA-004")) {
        std::cout << "   [CORRETO] Usuario 1 ja tem 3 hidrometros (nao adicionado)" << std::endl;
    }
    if (!manager.adicionarHidrometro(1, "SHA-005")) {
        std::cout << "   [CORRETO] Usuario 1 ja tem 3 hidrometros (nao adicionado)" << std::endl;
    }
    if (!manager.adicionarHidrometro(1, "SHA-006")) {
        std::cout << "   [CORRETO] Usuario 1 ja tem 3 hidrometros (nao adicionado)" << std::endl;
    }
    
    std::cout << "\n[8] Adicionando mais hidrometros aos outros usuarios..." << std::endl;
    manager.adicionarHidrometro(2, "SHA-103");
    manager.adicionarHidrometro(2, "SHA-104");
    manager.adicionarHidrometro(2, "SHA-105");
    std::cout << "   Usuario 2 agora tem 5 hidrometros (limite maximo)" << std::endl;
    
    manager.adicionarHidrometro(3, "SHA-202");
    std::cout << "   Usuario 3 agora tem 2 hidrometros" << std::endl;
    
    std::cout << "\n[9] Status final (11 hidrometros ativos):" << std::endl;
    manager.exibirStatus();
    
    std::cout << "\n[10] Aguardando mais 3 segundos..." << std::endl;
    for (int i = 3; i > 0; i--) {
        std::cout << "   " << i << "..." << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << std::endl;
    
    std::cout << "\n[11] Leituras finais:" << std::endl;
    std::cout << "   Usuario 1, SHA-002: " << manager.obterLeitura(1, "SHA-002") << " litros" << std::endl;
    std::cout << "   Usuario 2, SHA-103: " << manager.obterLeitura(2, "SHA-103") << " litros" << std::endl;
    std::cout << "   Usuario 3, SHA-202: " << manager.obterLeitura(3, "SHA-202") << " litros" << std::endl;
    
    std::cout << "\n[12] Parando todos os hidrometros..." << std::endl;
    manager.pararTodos();
    
    std::cout << "\n[13] Aguardando finalizacao das threads internas..." << std::flush;
    // As threads internas dos hidrômetros serão finalizadas quando o manager for destruído
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << " [OK]" << std::endl;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  DEMO CONCLUIDA COM SUCESSO!" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    return 0;
}
