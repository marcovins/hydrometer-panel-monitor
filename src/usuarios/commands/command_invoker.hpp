#ifndef COMMAND_INVOKER_HPP
#define COMMAND_INVOKER_HPP

#include "user_command.hpp"
#include <memory>
#include <stack>

// CommandInvoker - Invoker do padrão Command
// Gerencia a execução e o histórico de comandos
class CommandInvoker {
private:
    std::stack<std::unique_ptr<UserCommand>> historico;
    std::stack<std::unique_ptr<UserCommand>> historicoDesfeito; // Para redo

public:
    CommandInvoker() = default;
    
    // Executa um comando e adiciona ao histórico
    void executarComando(std::unique_ptr<UserCommand> comando);
    
    // Desfaz o último comando executado
    bool desfazer();
    
    // Refaz o último comando desfeito
    bool refazer();
    
    // Verifica se há comandos para desfazer
    bool podeDesfazer() const;
    
    // Verifica se há comandos para refazer
    bool podeRefazer() const;
    
    // Limpa o histórico
    void limparHistorico();
    
    // Retorna o tamanho do histórico
    size_t tamanhoHistorico() const;
};

#endif
