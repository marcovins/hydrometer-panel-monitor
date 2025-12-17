#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include <termios.h>
#include <unistd.h>

// Usa o wrapper que seleciona o simulador correto baseado na configuração
#include "simulator_wrapper.hpp"

// Variável global para controlar finalização
#ifdef USE_HYDROMETER_PROJECT_SIMULATOR
    Simulator* globalSimulator = nullptr;
#endif

static std::atomic<bool> shutdownInProgress(false);

// Handler para Ctrl+C
void signalHandler(int signal) {
    if (signal == SIGINT) {
        // Evita múltiplas execuções do handler
        bool expected = false;
        if (!shutdownInProgress.compare_exchange_strong(expected, true)) {
            // Já está finalizando, ignora
            return;
        }
        
        #ifdef USE_HYDROMETER_PROJECT_SIMULATOR
            Logger::log(LogLevel::SHUTDOWN, "\n[INFO] Ctrl+C detectado - Finalizando...");
            
            if (globalSimulator != nullptr) {
                globalSimulator->stop();
            }
        #else
            std::cout << "\n[INFO] Ctrl+C detectado - Finalizando..." << std::endl;
        #endif
        
        // Restaura terminal
        struct termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag |= (ICANON | ECHO | ISIG);
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
        
        exit(0);
    }
}

int main() {
    // Configura handler para Ctrl+C
    signal(SIGINT, signalHandler);
    
    // Exibe informações sobre o simulador configurado
    SimulatorWrapper::printSimulatorInfo();
    
    #ifdef USE_HYDROMETER_PROJECT_SIMULATOR
        // ===== HYDROMETER PROJECT SIMULATOR =====
        // Simulador completo com multi-threading e visualização
        
        // Ativa modo debug apenas no início
        Logger::setDebugMode(true);
        Logger::setRuntimeMode(false);
        
        Logger::log(LogLevel::STARTUP, "========================================");
        Logger::log(LogLevel::STARTUP, "[INFO] SIMULADOR DE HIDRÓMETRO INICIANDO");
        Logger::log(LogLevel::STARTUP, "========================================");
        Logger::log(LogLevel::STARTUP, "[INFO] Criando instância do simulador...");
        
        Simulator simulator;
        globalSimulator = &simulator; // Define ponteiro global para o handler
        
        Logger::log(LogLevel::STARTUP, "[INFO] Iniciando simulação...");
        simulator.run();
        
        Logger::log(LogLevel::STARTUP, "");
        Logger::log(LogLevel::STARTUP, "[INFO] ====== STATUS INICIAL ======");
        Logger::log(LogLevel::STARTUP, "[INFO] Hidrómetro status: Active");
        Logger::log(LogLevel::STARTUP, "[INFO] Contador inicial: 0 L");
        Logger::log(LogLevel::STARTUP, "[INFO] Simulação rodando: Sim");
        Logger::log(LogLevel::STARTUP, "[INFO] ==============================");
        Logger::log(LogLevel::STARTUP, "");
        Logger::log(LogLevel::STARTUP, "[INFO] === MODO MONITORAMENTO ===");
        Logger::log(LogLevel::STARTUP, "[INFO] Use as setas ↑↓←→ para ajustar vazão");
        Logger::log(LogLevel::STARTUP, "[INFO] Pressione ESC para sair");
        Logger::log(LogLevel::STARTUP, "");
        
        // Agora muda para modo runtime (apenas 4 linhas visíveis)
        Logger::setDebugMode(true);
        Logger::setRuntimeMode(true);
        Logger::clearRuntimeArea();
        
        // Loop de monitoramento - verifica se o simulador ainda está rodando
        while (simulator.isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Volta para modo de finalização
        Logger::setRuntimeMode(false);
        Logger::setDebugMode(true);
        
        Logger::log(LogLevel::SHUTDOWN, "");
        Logger::log(LogLevel::SHUTDOWN, "[INFO] Finalizando simulação...");
        simulator.stop();
        Logger::log(LogLevel::SHUTDOWN, "[INFO] Simulação finalizada com sucesso!");
        Logger::log(LogLevel::SHUTDOWN, "========================================");
        
    #elif defined(USE_SIMULADOR_HIDROMETRO)
        // ===== SIMULADOR SHA (FACHADA + CLI) =====
        // Simulador com interface CLI e padrão Facade
        
        std::cout << "\n[INFO] Iniciando Simulador SHA..." << std::endl;
        std::cout << "Digite 'help' para ver os comandos disponíveis\n" << std::endl;
        
        auto& fachada = FachadaSHA::instance();
        
        // Executa o loop CLI
        CLIClient::runCLI(fachada);
        
        std::cout << "\n[INFO] Simulação finalizada com sucesso!" << std::endl;
    #endif

    return 0;
}