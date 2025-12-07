#ifndef NOTIFICACAO_CONSOLE_LOG_HPP
#define NOTIFICACAO_CONSOLE_LOG_HPP

#include "notificacao_strategy.hpp"
#include <iostream>

/**
 * @brief Estratégia que envia notificações para o console
 * 
 * Útil para debug e testes rápidos. Simplesmente imprime
 * a mensagem no console padrão.
 */
class NotificacaoConsoleLog : public NotificacaoStrategy {
public:
    bool enviar(const std::string& mensagem, const std::string& destinatario) override;
    std::string getNomeCanal() const override;
    bool isDisponivel() const override;
};

#endif // NOTIFICACAO_CONSOLE_LOG_HPP
