#ifndef NOTIFICACAO_OBSERVER_HPP
#define NOTIFICACAO_OBSERVER_HPP

#include "alert_observer.hpp"
#include "../notifications/notificacao_strategy.hpp"
#include <memory>

/**
 * @brief Observer que envia notificações usando a estratégia configurada
 * 
 * Este observer atua como ponte entre o padrão Observer e o padrão Strategy,
 * permitindo que alertas sejam enviados através do canal de notificação
 * configurado (Console, Email, Popup, etc).
 */
class NotificacaoObserver : public AlertObserver {
private:
    std::shared_ptr<NotificacaoStrategy> strategy;
    std::string destinatarioPadrao;

public:
    NotificacaoObserver();
    explicit NotificacaoObserver(std::shared_ptr<NotificacaoStrategy> strategy);
    NotificacaoObserver(std::shared_ptr<NotificacaoStrategy> strategy, 
                       const std::string& destinatarioPadrao);

    void atualizar(const AlertaAtivo& alerta) override;
    std::string getNome() const override;

    // Configuração
    void setStrategy(std::shared_ptr<NotificacaoStrategy> strategy);
    void setDestinatarioPadrao(const std::string& destinatario);

private:
    std::string obterDestinatario(const AlertaAtivo& alerta) const;
    std::string formatarMensagem(const AlertaAtivo& alerta) const;
};

#endif // NOTIFICACAO_OBSERVER_HPP
