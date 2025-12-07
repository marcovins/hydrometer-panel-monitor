#include "notificacao_console_log.hpp"
#include <ctime>
#include <iomanip>

bool NotificacaoConsoleLog::enviar(const std::string& mensagem, const std::string& destinatario) {
    // Obter timestamp atual
    time_t now = std::time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Formatar e imprimir mensagem
    std::cout << "\n"
              << "╔═════════════════════════════════════════════════════════════╗\n"
              << "║                    🔔 ALERTA DE CONSUMO                    ║\n"
              << "╠═════════════════════════════════════════════════════════════╣\n"
              << "║ Data/Hora: " << std::setw(47) << std::left << buffer << "║\n"
              << "║ Destinatário: " << std::setw(44) << std::left << destinatario << "║\n"
              << "╠═════════════════════════════════════════════════════════════╣\n"
              << "║ Mensagem:                                                  ║\n"
              << "║ " << std::setw(58) << std::left << mensagem << "║\n"
              << "╚═════════════════════════════════════════════════════════════╝\n"
              << std::endl;

    return true;
}

std::string NotificacaoConsoleLog::getNomeCanal() const {
    return "CONSOLE_LOG";
}

bool NotificacaoConsoleLog::isDisponivel() const {
    return true; // Console sempre está disponível
}
