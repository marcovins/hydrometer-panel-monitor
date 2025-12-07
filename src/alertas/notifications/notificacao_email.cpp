#include "notificacao_email.hpp"
#include <iostream>
#include <regex>

NotificacaoEmail::NotificacaoEmail() 
    : servidor("smtp.example.com"), porta(587), 
      remetente("alertas@cagepa.com.br"), configurado(false) {}

NotificacaoEmail::NotificacaoEmail(const std::string& servidor, int porta, const std::string& remetente)
    : servidor(servidor), porta(porta), remetente(remetente), configurado(true) {}

bool NotificacaoEmail::enviar(const std::string& mensagem, const std::string& destinatario) {
    if (!isDisponivel()) {
        std::cerr << "[EMAIL] Erro: Canal de email não configurado" << std::endl;
        return false;
    }

    if (!validarEmail(destinatario)) {
        std::cerr << "[EMAIL] Erro: Email inválido: " << destinatario << std::endl;
        return false;
    }

    return enviarSMTP(mensagem, destinatario);
}

std::string NotificacaoEmail::getNomeCanal() const {
    return "EMAIL";
}

bool NotificacaoEmail::isDisponivel() const {
    return configurado;
}

void NotificacaoEmail::configurar(const std::string& servidor, int porta, const std::string& remetente) {
    this->servidor = servidor;
    this->porta = porta;
    this->remetente = remetente;
    this->configurado = true;
}

bool NotificacaoEmail::validarEmail(const std::string& email) const {
    // Regex simples para validação de email
    const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
}

bool NotificacaoEmail::enviarSMTP(const std::string& mensagem, const std::string& destinatario) {
    // Simulação de envio de email
    // Em produção, aqui seria usado uma biblioteca SMTP real (ex: libcurl, cpp-smtp)
    
    std::cout << "\n"
              << "╔═════════════════════════════════════════════════════════════╗\n"
              << "║                     📧 EMAIL ENVIADO                       ║\n"
              << "╠═════════════════════════════════════════════════════════════╣\n"
              << "║ Servidor: " << servidor << ":" << porta << "\n"
              << "║ De: " << remetente << "\n"
              << "║ Para: " << destinatario << "\n"
              << "║ Assunto: Alerta de Consumo - CAGEPA\n"
              << "╠═════════════════════════════════════════════════════════════╣\n"
              << "║ Mensagem:\n"
              << "║ " << mensagem << "\n"
              << "╠═════════════════════════════════════════════════════════════╣\n"
              << "║ Status: ✓ ENVIADO COM SUCESSO (SIMULADO)                  ║\n"
              << "╚═════════════════════════════════════════════════════════════╝\n"
              << std::endl;

    return true;
}
