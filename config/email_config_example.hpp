/**
 * @file email_config.hpp
 * @brief Configuração de credenciais Gmail OAuth2 para envio de emails
 * 
 * ⚠️ IMPORTANTE: Este arquivo contém credenciais sensíveis!
 * - NUNCA faça commit deste arquivo (já está no .gitignore)
 * - Mantenha suas credenciais em segurança
 * 
 * COMO OBTER CREDENCIAIS GMAIL OAUTH2:
 * ====================================
 * 1. Acesse: https://console.cloud.google.com/
 * 2. Crie um projeto ou use existente
 * 3. Ative Gmail API (APIs & Services > Enable APIs)
 * 4. Configure OAuth consent screen (External)
 * 5. Crie credenciais OAuth 2.0 (Desktop app)
 * 6. Obtenha Refresh Token em: https://developers.google.com/oauthplayground/
 * 
 * 📖 Guia completo passo a passo: docs/GMAIL_OAUTH2_QUICKSTART.md
 * 
 * @author Marcos Belo
 * @date 16/12/2025
 */

#ifndef EMAIL_CONFIG_HPP
#define EMAIL_CONFIG_HPP

#include <string>

namespace EmailConfig {
    
    // =========================================================================
    // CONFIGURAÇÕES GMAIL OAUTH2
    // =========================================================================
    
    namespace Gmail {
        // Servidor SMTP do Gmail (não alterar)
        const std::string SMTP_SERVER = "smtp.gmail.com";
        const int SMTP_PORT = 587; // TLS/STARTTLS
        
        // ✏️ SUBSTITUA COM SEU EMAIL GMAIL
        const std::string FROM_EMAIL = "";
        const std::string FROM_NAME = "Sistema SSMH - Alertas CAGEPA";
        
        // ✏️ SUBSTITUA COM SUAS CREDENCIAIS OAUTH2 (do Google Cloud Console)
        const std::string CLIENT_ID = "";
        const std::string CLIENT_SECRET = "";
        
        // ✏️ SUBSTITUA COM SEU REFRESH TOKEN (do OAuth2 Playground)
        const std::string REFRESH_TOKEN = "";
    }
    
    // =========================================================================
    // CONFIGURAÇÕES AVANÇADAS (opcional)
    // =========================================================================
    
    // Timeout em segundos para operações SMTP
    const long TIMEOUT = 30;
    
    // Modo verbose para debug (mostra logs detalhados do CURL)
    // ⚠️ Use apenas para testes! Desabilite em produção
    const bool VERBOSE = false;
    
    // Verificar certificado SSL (recomendado: true em produção)
    const bool VERIFY_SSL = true;
    
} // namespace EmailConfig

#endif // EMAIL_CONFIG_HPP
