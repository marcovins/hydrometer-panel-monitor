#include "notificacao_windows_popup.hpp"
#include <iostream>
#include <cstdlib>

bool NotificacaoWindowsPopup::enviar(const std::string& mensagem, const std::string& destinatario) {
    if (isWindows()) {
        return enviarWindows(mensagem, destinatario);
    } else if (isLinux()) {
        return enviarLinux(mensagem, destinatario);
    } else {
        return enviarFallback(mensagem, destinatario);
    }
}

std::string NotificacaoWindowsPopup::getNomeCanal() const {
    return "WINDOWS_POPUP";
}

bool NotificacaoWindowsPopup::isDisponivel() const {
    return true; // Sempre disponível (com fallback)
}

bool NotificacaoWindowsPopup::isWindows() const {
#ifdef _WIN32
    return true;
#else
    return false;
#endif
}

bool NotificacaoWindowsPopup::isLinux() const {
#ifdef __linux__
    return true;
#else
    return false;
#endif
}

bool NotificacaoWindowsPopup::enviarWindows(const std::string& mensagem, const std::string& destinatario) {
#ifdef _WIN32
    // Em Windows, usa MessageBox
    std::string titulo = "Alerta de Consumo - Usuario: " + destinatario;
    std::string comando = "msg * \"" + titulo + ": " + mensagem + "\"";
    int resultado = system(comando.c_str());
    return resultado == 0;
#else
    return false;
#endif
}

bool NotificacaoWindowsPopup::enviarLinux(const std::string& mensagem, const std::string& destinatario) {
#ifdef __linux__
    if (!notifySendDisponivel()) {
        return enviarFallback(mensagem, destinatario);
    }
    
    // Usa notify-send para exibir notificação nativa do Linux
    std::string titulo = "⚠️ Alerta de Consumo";
    std::string corpo = "Usuário: " + destinatario + "\n" + mensagem;
    
    // Escapa aspas na mensagem
    std::string corpoEscapado = corpo;
    size_t pos = 0;
    while ((pos = corpoEscapado.find("\"", pos)) != std::string::npos) {
        corpoEscapado.replace(pos, 1, "'");
        pos += 1;
    }
    
    // Comando notify-send com urgência crítica e tempo de exibição de 10 segundos
    std::string comando = "notify-send -u critical -t 10000 \"" + titulo + "\" \"" + corpoEscapado + "\"";
    int resultado = system(comando.c_str());
    
    if (resultado == 0) {
        std::cout << "[NOTIFICAÇÃO] Popup enviado para " << destinatario << std::endl;
        return true;
    }
    
    // Se falhou, usa fallback
    return enviarFallback(mensagem, destinatario);
#else
    return false;
#endif
}

bool NotificacaoWindowsPopup::notifySendDisponivel() const {
    // Verifica se notify-send está disponível no sistema
    int resultado = system("which notify-send > /dev/null 2>&1");
    return resultado == 0;
}

bool NotificacaoWindowsPopup::enviarFallback(const std::string& mensagem, const std::string& destinatario) {
    // Em sistemas não-Windows, exibe no console de forma destacada
    std::cout << "\n"
              << "┌─────────────────────────────────────────────────────────────┐\n"
              << "│                   💬 POPUP SIMULADO                         │\n"
              << "├─────────────────────────────────────────────────────────────┤\n"
              << "│ Para: " << destinatario << "\n"
              << "│ Mensagem: " << mensagem << "\n"
              << "└─────────────────────────────────────────────────────────────┘\n"
              << std::endl;
    return true;
}
