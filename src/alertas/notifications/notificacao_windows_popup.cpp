#include "notificacao_windows_popup.hpp"
#include <iostream>
#include <cstdlib>

bool NotificacaoWindowsPopup::enviar(const std::string& mensagem, const std::string& destinatario) {
    if (isWindows()) {
        return enviarWindows(mensagem, destinatario);
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
