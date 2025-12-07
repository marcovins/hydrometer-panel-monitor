#include "notificacao_observer.hpp"
#include "../notifications/notificacao_console_log.hpp"
#include <iostream>
#include <sstream>

NotificacaoObserver::NotificacaoObserver() 
    : strategy(std::make_shared<NotificacaoConsoleLog>()), 
      destinatarioPadrao("admin@cagepa.com.br") {}

NotificacaoObserver::NotificacaoObserver(std::shared_ptr<NotificacaoStrategy> strategy) 
    : strategy(strategy), destinatarioPadrao("admin@cagepa.com.br") {}

NotificacaoObserver::NotificacaoObserver(std::shared_ptr<NotificacaoStrategy> strategy, 
                                         const std::string& destinatarioPadrao)
    : strategy(strategy), destinatarioPadrao(destinatarioPadrao) {}

void NotificacaoObserver::atualizar(const AlertaAtivo& alerta) {
    if (!strategy) {
        std::cerr << "[NOTIFICACAO_OBSERVER] Erro: Estratégia não configurada" << std::endl;
        return;
    }

    if (!strategy->isDisponivel()) {
        std::cerr << "[NOTIFICACAO_OBSERVER] Aviso: Canal " 
                  << strategy->getNomeCanal() << " não disponível" << std::endl;
        return;
    }

    std::string destinatario = obterDestinatario(alerta);
    std::string mensagem = formatarMensagem(alerta);

    bool sucesso = strategy->enviar(mensagem, destinatario);
    
    if (sucesso) {
        std::cout << "[NOTIFICACAO_OBSERVER] Notificação enviada via " 
                  << strategy->getNomeCanal() << std::endl;
    } else {
        std::cerr << "[NOTIFICACAO_OBSERVER] Falha ao enviar notificação via " 
                  << strategy->getNomeCanal() << std::endl;
    }
}

std::string NotificacaoObserver::getNome() const {
    return "NotificacaoObserver";
}

void NotificacaoObserver::setStrategy(std::shared_ptr<NotificacaoStrategy> strategy) {
    this->strategy = strategy;
}

void NotificacaoObserver::setDestinatarioPadrao(const std::string& destinatario) {
    this->destinatarioPadrao = destinatario;
}

std::string NotificacaoObserver::obterDestinatario(const AlertaAtivo& alerta) const {
    // Em produção, buscaria o email do usuário do banco de dados
    // Por ora, usa um formato simulado
    return "usuario_" + std::to_string(alerta.getUsuarioId()) + "@cagepa.com.br";
}

std::string NotificacaoObserver::formatarMensagem(const AlertaAtivo& alerta) const {
    std::stringstream ss;
    ss << "[" << alerta.getSeveridadeString() << "] "
       << alerta.getMensagem()
       << " (Consumo: " << alerta.getValorConsumo() << "L)";
    return ss.str();
}
