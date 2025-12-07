#include "logger_observer.hpp"
#include <sstream>

LoggerObserver::LoggerObserver() {
    // Não precisa guardar referência, usaremos métodos estáticos
}

LoggerObserver::LoggerObserver(std::shared_ptr<Logger> logger) {
    // Mantém compatibilidade mas não usamos o parâmetro
    (void)logger;
}

void LoggerObserver::atualizar(const AlertaAtivo& alerta) {
    std::string mensagemLog = formatarMensagemLog(alerta);
    
    // Usa o Logger estático com nível apropriado baseado na severidade
    switch (alerta.getSeveridade()) {
        case AlertaAtivo::CRITICA:
            Logger::log(LogLevel::ERROR, mensagemLog);
            break;
        case AlertaAtivo::ALTA:
            Logger::log(LogLevel::WARNING, mensagemLog);
            break;
        case AlertaAtivo::MEDIA:
        case AlertaAtivo::BAIXA:
            Logger::log(LogLevel::INFO, mensagemLog);
            break;
    }
}

std::string LoggerObserver::getNome() const {
    return "LoggerObserver";
}

std::string LoggerObserver::formatarMensagemLog(const AlertaAtivo& alerta) const {
    std::stringstream ss;
    ss << "[ALERTA] "
       << "ID=" << alerta.getId()
       << " | Usuario=" << alerta.getUsuarioId()
       << " | Tipo=" << alerta.getTipoRegra()
       << " | Severidade=" << alerta.getSeveridadeString()
       << " | Consumo=" << alerta.getValorConsumo() << "L"
       << " | Msg: " << alerta.getMensagem();
    return ss.str();
}
