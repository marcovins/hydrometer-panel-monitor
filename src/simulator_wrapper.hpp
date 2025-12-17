/**
 * @file simulator_wrapper.hpp
 * @brief Wrapper unificado para os diferentes simuladores disponíveis
 * 
 * Este arquivo fornece uma interface comum para trabalhar com diferentes
 * implementações de simuladores de hidrômetro, permitindo trocar facilmente
 * entre eles através da configuração do Makefile.
 * 
 * @author Marcos Belo
 * @date 16/12/2025
 * @version 1.0
 */

#ifndef SIMULATOR_WRAPPER_HPP
#define SIMULATOR_WRAPPER_HPP

// Seleciona o simulador baseado na flag de compilação
#ifdef USE_HYDROMETER_PROJECT_SIMULATOR
    // Usa o simulador completo com multi-threading e visualização Cairo
    #include "simulator/hydrometer-project-simulator/src/modules/simulator.hpp"
    #include "simulator/hydrometer-project-simulator/src/utils/logger.hpp"
    
    // Typedef para padronizar o nome
    using SimulatorImpl = Simulator;
    
    #define SIMULATOR_NAME "Hydrometer Project Simulator"
    #define SIMULATOR_VERSION "2.0"
    #define HAS_GRAPHICS true
    #define HAS_INTERACTIVE_CONTROL true
    #define USES_FACADE false
    
#elif defined(USE_SIMULADOR_HIDROMETRO)
    // Usa o simulador com Fachada SHA, CLI e OpenCV
    #include "fachada.hpp"
    #include "cli.hpp"
    #include "display.hpp"
    
    // Este simulador usa padrão Facade e CLI, não tem uma classe Simulator direta
    // mas sim a FachadaSHA que gerencia instâncias
    using SimulatorImpl = FachadaSHA;
    
    #define SIMULATOR_NAME "Simulador SHA (Fachada + CLI)"
    #define SIMULATOR_VERSION "1.0"
    #define HAS_GRAPHICS true
    #define HAS_INTERACTIVE_CONTROL true
    #define USES_FACADE true
    
#else
    #error "Nenhum simulador foi configurado! Defina USE_HYDROMETER_PROJECT_SIMULATOR ou USE_SIMULADOR_HIDROMETRO"
#endif

#include <string>
#include <iostream>

/**
 * @brief Namespace para funcionalidades comuns do wrapper do simulador
 */
namespace SimulatorWrapper {
    
    /**
     * @brief Retorna o nome do simulador atualmente configurado
     * @return std::string Nome do simulador
     */
    inline std::string getSimulatorName() {
        return SIMULATOR_NAME;
    }
    
    /**
     * @brief Retorna a versão do simulador atualmente configurado
     * @return std::string Versão do simulador
     */
    inline std::string getSimulatorVersion() {
        return SIMULATOR_VERSION;
    }
    
    /**
     * @brief Verifica se o simulador suporta gráficos
     * @return bool true se suporta gráficos, false caso contrário
     */
    inline bool hasGraphics() {
        return HAS_GRAPHICS;
    }
    
    /**
     * @brief Verifica se o simulador suporta controle interativo
     * @return bool true se suporta controle interativo, false caso contrário
     */
    inline bool hasInteractiveControl() {
        return HAS_INTERACTIVE_CONTROL;
    }
    
    /**
     * @brief Verifica se o simulador usa padrão Facade
     * @return bool true se usa Facade, false caso contrário
     */
    inline bool usesFacade() {
        return USES_FACADE;
    }
    
    /**
     * @brief Imprime informações sobre o simulador configurado
     */
    inline void printSimulatorInfo() {
        std::cout << "========================================" << std::endl;
        std::cout << "Informações do Simulador" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Nome: " << getSimulatorName() << std::endl;
        std::cout << "Versão: " << getSimulatorVersion() << std::endl;
        std::cout << "Gráficos: " << (hasGraphics() ? "Sim" : "Não") << std::endl;
        std::cout << "Controle Interativo: " << (hasInteractiveControl() ? "Sim" : "Não") << std::endl;
        std::cout << "Usa Facade: " << (usesFacade() ? "Sim" : "Não") << std::endl;
        std::cout << "========================================" << std::endl;
    }
    
} // namespace SimulatorWrapper

#endif // SIMULATOR_WRAPPER_HPP
