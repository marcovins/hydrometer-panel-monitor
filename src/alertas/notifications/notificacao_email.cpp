#include "notificacao_email.hpp"
#include <iostream>
#include <regex>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <curl/curl.h>

// Tenta carregar configurações customizadas, se não existir usa modo simulação
#ifdef EMAIL_CONFIG_EXISTS
    #include "../../../config/email_config.hpp"
#endif

NotificacaoEmail::NotificacaoEmail() 
    : servidor("smtp.gmail.com"), 
      porta(587), 
      remetente("alertas@cagepa.com.br"), 
      nomeRemetente("Sistema SSMH - Alertas"),
      configurado(false),
      tokenExpiry(0),
      verifySSL(true),
      verbose(false),
      timeout(30) {
    
    // Tenta carregar configuração do arquivo
    carregarConfiguracao();
}

NotificacaoEmail::NotificacaoEmail(const std::string& servidor, int porta, const std::string& remetente)
    : servidor(servidor),
      porta(porta),
      remetente(remetente),
      nomeRemetente("Sistema SSMH - Alertas"),
      configurado(false),
      tokenExpiry(0),
      verifySSL(true),
      verbose(false),
      timeout(30) {
    
    // Tenta carregar configuração OAuth2
    carregarConfiguracao();
}

NotificacaoEmail::NotificacaoEmail(const std::string& remetente, 
                                   const std::string& clientId,
                                   const std::string& clientSecret, 
                                   const std::string& refreshToken)
    : servidor("smtp.gmail.com"),
      porta(587),
      remetente(remetente),
      nomeRemetente("Sistema SSMH - Alertas"),
      configurado(true),
      clientId(clientId),
      clientSecret(clientSecret),
      refreshToken(refreshToken),
      tokenExpiry(0),
      verifySSL(true),
      verbose(false),
      timeout(30) {}

NotificacaoEmail::~NotificacaoEmail() {
    // Limpeza se necessário
}

bool NotificacaoEmail::carregarConfiguracao() {
#ifdef EMAIL_CONFIG_EXISTS
    try {
        servidor = EmailConfig::Gmail::SMTP_SERVER;
        porta = EmailConfig::Gmail::SMTP_PORT;
        remetente = EmailConfig::Gmail::FROM_EMAIL;
        nomeRemetente = EmailConfig::Gmail::FROM_NAME;
        
        if (!EmailConfig::Gmail::CLIENT_ID.empty() && 
            !EmailConfig::Gmail::REFRESH_TOKEN.empty()) {
            clientId = EmailConfig::Gmail::CLIENT_ID;
            clientSecret = EmailConfig::Gmail::CLIENT_SECRET;
            refreshToken = EmailConfig::Gmail::REFRESH_TOKEN;
            configurado = true;
            
            std::cout << "[EMAIL] ✓ Configuração OAuth2 carregada com sucesso" << std::endl;
            std::cout << "[EMAIL] Remetente: " << remetente << std::endl;
        } else {
            std::cout << "[EMAIL] ⚠ Credenciais OAuth2 não configuradas" << std::endl;
            configurado = false;
        }
        
        verifySSL = EmailConfig::VERIFY_SSL;
        verbose = EmailConfig::VERBOSE;
        timeout = EmailConfig::TIMEOUT;
        
        return configurado;
    } catch (...) {
        std::cerr << "[EMAIL] Erro ao carregar configuração" << std::endl;
        return false;
    }
#else
    std::cout << "[EMAIL] ⚠ Arquivo de configuração não encontrado" << std::endl;
    std::cout << "[EMAIL] Para envio real:" << std::endl;
    std::cout << "[EMAIL]   1. cp config/email_config.example.hpp config/email_config.hpp" << std::endl;
    std::cout << "[EMAIL]   2. Configure suas credenciais Gmail OAuth2" << std::endl;
    std::cout << "[EMAIL]   3. Descomente EMAIL_CONFIG_FLAG no Makefile" << std::endl;
    std::cout << "[EMAIL]   4. Recompile: make clean && make" << std::endl;
    return false;
#endif
}

void NotificacaoEmail::configurarOAuth2(const std::string& clientId, 
                                        const std::string& clientSecret, 
                                        const std::string& refreshToken) {
    this->clientId = clientId;
    this->clientSecret = clientSecret;
    this->refreshToken = refreshToken;
    this->configurado = true;
    
    std::cout << "[EMAIL] ✓ OAuth2 configurado manualmente" << std::endl;
}

void NotificacaoEmail::configurarRemetente(const std::string& email, const std::string& nome) {
    this->remetente = email;
    this->nomeRemetente = nome;
}

bool NotificacaoEmail::enviar(const std::string& mensagem, const std::string& destinatario) {
    if (!isDisponivel()) {
        std::cerr << "[EMAIL] ⚠ Email não configurado - usando modo simulação" << std::endl;
        
        // Fallback para simulação
        std::cout << "\n"
                  << "╔═════════════════════════════════════════════════════════════╗\n"
                  << "║           📧 EMAIL (MODO SIMULAÇÃO)                        ║\n"
                  << "╠═════════════════════════════════════════════════════════════╣\n"
                  << "║ Para: " << destinatario << "\n"
                  << "║ De: " << remetente << "\n"
                  << "║ Mensagem: " << mensagem << "\n"
                  << "╠═════════════════════════════════════════════════════════════╣\n"
                  << "║ ℹ Configure OAuth2 Gmail para envio real                  ║\n"
                  << "║ Veja: docs/EMAIL_OAUTH2_SETUP.md                          ║\n"
                  << "╚═════════════════════════════════════════════════════════════╝\n"
                  << std::endl;
        return true;
    }

    if (!validarEmail(destinatario)) {
        std::cerr << "[EMAIL] ✗ Email inválido: " << destinatario << std::endl;
        return false;
    }

    return enviarSMTP(mensagem, destinatario);
}

std::string NotificacaoEmail::getNomeCanal() const {
    return "EMAIL";
}

bool NotificacaoEmail::isDisponivel() const {
    return configurado && !clientId.empty() && !clientSecret.empty() && !refreshToken.empty();
}

bool NotificacaoEmail::validarEmail(const std::string& email) const {
    const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
}

bool NotificacaoEmail::tokenValido() const {
    if (accessToken.empty()) return false;
    
    time_t agora = time(nullptr);
    return agora < tokenExpiry;
}

bool NotificacaoEmail::obterAccessToken() {
    if (tokenValido()) {
        if (verbose) {
            std::cout << "[EMAIL] ℹ Access token ainda válido" << std::endl;
        }
        return true;
    }
    
    if (verbose) {
        std::cout << "[EMAIL] ℹ Obtendo novo access token..." << std::endl;
    }
    
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[EMAIL] ✗ Falha ao inicializar CURL" << std::endl;
        return false;
    }
    
    std::string responseData;
    std::string postFields = 
        "client_id=" + clientId +
        "&client_secret=" + clientSecret +
        "&refresh_token=" + refreshToken +
        "&grant_type=refresh_token";
    
    curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
        +[](char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t {
            std::string* str = static_cast<std::string*>(userdata);
            str->append(ptr, size * nmemb);
            return size * nmemb;
        });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
    
    if (verbose) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        std::cerr << "[EMAIL] ✗ Erro ao obter access token: " << curl_easy_strerror(res) << std::endl;
        return false;
    }
    
    // Parse JSON simples (procura "access_token":"...")
    size_t pos = responseData.find("\"access_token\"");
    if (pos != std::string::npos) {
        size_t start = responseData.find("\"", pos + 15) + 1;
        size_t end = responseData.find("\"", start);
        accessToken = responseData.substr(start, end - start);
        tokenExpiry = time(nullptr) + 3300; // 55 minutos (tokens válidos por 1h)
        
        if (verbose) {
            std::cout << "[EMAIL] ✓ Access token obtido com sucesso" << std::endl;
        }
        return true;
    }
    
    std::cerr << "[EMAIL] ✗ Falha ao parsear access token" << std::endl;
    if (verbose) {
        std::cerr << "[EMAIL] Resposta: " << responseData << std::endl;
    }
    return false;
}

std::string NotificacaoEmail::construirMensagemEmail(const std::string& mensagem,
                                                      const std::string& destinatario) const {
    std::ostringstream email;
    
    // Headers
    email << "From: " << nomeRemetente << " <" << remetente << ">\r\n";
    email << "To: <" << destinatario << ">\r\n";
    email << "Subject: =?UTF-8?B?" << base64Encode("Alerta de Consumo - CAGEPA") << "?=\r\n";
    email << "MIME-Version: 1.0\r\n";
    email << "Content-Type: text/plain; charset=UTF-8\r\n";
    email << "Content-Transfer-Encoding: quoted-printable\r\n";
    email << "\r\n";
    
    // Body
    email << mensagem << "\r\n";
    email << "\r\n";
    email << "---\r\n";
    email << "Sistema de Monitoramento de Hidrômetros - SSMH\r\n";
    email << "CAGEPA - Companhia de Água e Esgotos da Paraíba\r\n";
    
    return email.str();
}

std::string NotificacaoEmail::base64Encode(const std::string& input) const {
    static const char base64_chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    size_t in_len = input.size();
    const unsigned char* bytes_to_encode = reinterpret_cast<const unsigned char*>(input.c_str());
    
    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for(i = 0; i < 4; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    
    if (i) {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';
        
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        
        for (j = 0; j < i + 1; j++)
            ret += base64_chars[char_array_4[j]];
        
        while(i++ < 3)
            ret += '=';
    }
    
    return ret;
}

size_t NotificacaoEmail::readCallback(char *ptr, size_t size, size_t nmemb, void *userp) {
    EmailPayload *payload = static_cast<EmailPayload*>(userp);
    
    size_t room = size * nmemb;
    size_t len = payload->totalSize - payload->bytesRead;
    
    if (len > room)
        len = room;
    
    if (len) {
        memcpy(ptr, payload->data + payload->bytesRead, len);
        payload->bytesRead += len;
        return len;
    }
    
    return 0;
}

bool NotificacaoEmail::enviarSMTP(const std::string& mensagem, const std::string& destinatario) {
    // Obtém access token
    if (!obterAccessToken()) {
        std::cerr << "[EMAIL] ✗ Não foi possível obter access token" << std::endl;
        return false;
    }
    
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[EMAIL] ✗ Falha ao inicializar CURL" << std::endl;
        return false;
    }
    
    // Construir URL e mensagem
    std::string url = "smtp://" + servidor + ":" + std::to_string(porta);
    std::string emailData = construirMensagemEmail(mensagem, destinatario);
    
    EmailPayload payload;
    payload.data = emailData.c_str();
    payload.bytesRead = 0;
    payload.totalSize = emailData.size();
    
    // Configurar CURL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    
    // OAuth2 XOAUTH2
    curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, accessToken.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, remetente.c_str());
    
    // Remetente e destinatário
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, remetente.c_str());
    
    struct curl_slist *recipients = nullptr;
    recipients = curl_slist_append(recipients, destinatario.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    
    // Dados do email
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, readCallback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &payload);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    
    // Timeout e SSL
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, verifySSL ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, verifySSL ? 2L : 0L);
    
    // Verbose
    if (verbose) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }
    
    // Executar
    std::cout << "[EMAIL] 📧 Enviando email para " << destinatario << "..." << std::endl;
    CURLcode res = curl_easy_perform(curl);
    
    // Limpeza
    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        std::cerr << "[EMAIL] ✗ Erro ao enviar: " << curl_easy_strerror(res) << std::endl;
        return false;
    }
    
    std::cout << "[EMAIL] ✓ Email enviado com sucesso!" << std::endl;
    return true;
}
