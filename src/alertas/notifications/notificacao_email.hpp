#ifndef NOTIFICACAO_EMAIL_HPP
#define NOTIFICACAO_EMAIL_HPP

#include "notificacao_strategy.hpp"
#include <memory>
#include <curl/curl.h>

/**
 * @brief Estratégia que envia notificações por email usando Gmail com OAuth2
 * 
 * Implementação real de envio de email usando libcurl com:
 * - OAuth2 Gmail API
 * - SMTP/STARTTLS (smtp.gmail.com:587)
 * - Refresh token automático
 * 
 * Configurações são carregadas do arquivo config/email_config.hpp
 */
class NotificacaoEmail : public NotificacaoStrategy {
private:
    // Configurações SMTP
    std::string servidor;
    int porta;
    std::string remetente;
    std::string nomeRemetente;
    bool configurado;
    
    // OAuth2 credentials (Gmail)
    std::string clientId;
    std::string clientSecret;
    std::string refreshToken;
    std::string accessToken;
    long tokenExpiry;
    
    // Configurações gerais
    bool verifySSL;
    bool verbose;
    long timeout;

public:
    NotificacaoEmail();
    NotificacaoEmail(const std::string& servidor, int porta, const std::string& remetente);
    NotificacaoEmail(const std::string& remetente, const std::string& clientId, 
                     const std::string& clientSecret, const std::string& refreshToken);
    ~NotificacaoEmail();

    bool enviar(const std::string& mensagem, const std::string& destinatario) override;
    std::string getNomeCanal() const override;
    bool isDisponivel() const override;

    // Configuração OAuth2
    void configurarOAuth2(const std::string& clientId, const std::string& clientSecret, 
                          const std::string& refreshToken);
    void configurarRemetente(const std::string& email, const std::string& nome);
    
    // Carrega configurações do arquivo config/email_config.hpp
    bool carregarConfiguracao();

private:
    bool validarEmail(const std::string& email) const;
    bool enviarSMTP(const std::string& mensagem, const std::string& destinatario);
    
    // OAuth2 methods
    bool obterAccessToken();
    bool tokenValido() const;
    
    // SMTP helpers
    std::string construirMensagemEmail(const std::string& mensagem, 
                                        const std::string& destinatario) const;
    std::string base64Encode(const std::string& input) const;
    
    // CURL callback
    static size_t readCallback(char *ptr, size_t size, size_t nmemb, void *userp);
    
    // Estrutura para payload do email
    struct EmailPayload {
        const char* data;
        size_t bytesRead;
        size_t totalSize;
    };
};

#endif // NOTIFICACAO_EMAIL_HPP
