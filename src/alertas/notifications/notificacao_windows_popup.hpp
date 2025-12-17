#ifndef NOTIFICACAO_WINDOWS_POPUP_HPP
#define NOTIFICACAO_WINDOWS_POPUP_HPP

#include "notificacao_strategy.hpp"

/**
 * @brief Estratégia que exibe notificações como popup do Windows
 * 
 * Em ambientes Windows, usa a API nativa para exibir notificações.
 * Em outros sistemas, faz fallback para console com mensagem especial.
 */
class NotificacaoWindowsPopup : public NotificacaoStrategy {
public:
    bool enviar(const std::string& mensagem, const std::string& destinatario) override;
    std::string getNomeCanal() const override;
    bool isDisponivel() const override;

private:
    bool isWindows() const;
    bool isLinux() const;
    bool enviarWindows(const std::string& mensagem, const std::string& destinatario);
    bool enviarLinux(const std::string& mensagem, const std::string& destinatario);
    bool enviarFallback(const std::string& mensagem, const std::string& destinatario);
    bool notifySendDisponivel() const;
};

#endif // NOTIFICACAO_WINDOWS_POPUP_HPP
