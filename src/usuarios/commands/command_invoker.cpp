#include "command_invoker.hpp"

void CommandInvoker::executarComando(std::unique_ptr<UserCommand> comando) {
    if (comando) {
        // Executa o comando
        comando->execute();
        
        // Adiciona ao histórico
        historico.push(std::move(comando));
        
        // Limpa o histórico de refazer ao executar um novo comando
        while (!historicoDesfeito.empty()) {
            historicoDesfeito.pop();
        }
    }
}

bool CommandInvoker::desfazer() {
    if (historico.empty()) {
        return false;
    }
    
    // Pega o último comando do histórico
    std::unique_ptr<UserCommand> comando = std::move(historico.top());
    historico.pop();
    
    // Desfaz o comando
    comando->undo();
    
    // Move para o histórico de desfeitos (para permitir refazer)
    historicoDesfeito.push(std::move(comando));
    
    return true;
}

bool CommandInvoker::refazer() {
    if (historicoDesfeito.empty()) {
        return false;
    }
    
    // Pega o último comando desfeito
    std::unique_ptr<UserCommand> comando = std::move(historicoDesfeito.top());
    historicoDesfeito.pop();
    
    // Re-executa o comando
    comando->execute();
    
    // Move de volta para o histórico
    historico.push(std::move(comando));
    
    return true;
}

bool CommandInvoker::podeDesfazer() const {
    return !historico.empty();
}

bool CommandInvoker::podeRefazer() const {
    return !historicoDesfeito.empty();
}

void CommandInvoker::limparHistorico() {
    while (!historico.empty()) {
        historico.pop();
    }
    while (!historicoDesfeito.empty()) {
        historicoDesfeito.pop();
    }
}

size_t CommandInvoker::tamanhoHistorico() const {
    return historico.size();
}
