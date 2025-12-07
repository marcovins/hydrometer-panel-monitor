#ifndef NOTIFICACAO_EMAIL_HPP
#define NOTIFICACAO_EMAIL_HPP

#include "notificacao_strategy.hpp"

/**
 * @brief Estratégia que envia notificações por email
 * 
 * Em produção, integraria com biblioteca SMTP para envio real.
 * Nesta implementação, simula o envio registrando em log.
 */
class NotificacaoEmail : public NotificacaoStrategy {
private:
    std::string servidor;
    int porta;
    std::string remetente;
    bool configurado;

public:
    NotificacaoEmail();
    NotificacaoEmail(const std::string& servidor, int porta, const std::string& remetente);

    bool enviar(const std::string& mensagem, const std::string& destinatario) override;
    std::string getNomeCanal() const override;
    bool isDisponivel() const override;

    // Configuração
    void configurar(const std::string& servidor, int porta, const std::string& remetente);

private:
    bool validarEmail(const std::string& email) const;
    bool enviarSMTP(const std::string& mensagem, const std::string& destinatario);
};

#endif // NOTIFICACAO_EMAIL_HPP
