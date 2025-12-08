#include "logger.hpp"

bool Logger::showDebug = false;
bool Logger::runtimeStarted = false;
Logger* Logger::instance_ = nullptr;
std::mutex Logger::mutex_;

// Construtor privado
Logger::Logger() : usarArquivo_(false) {
}

// Destrutor
Logger::~Logger() {
    fecharArquivo();
}

// Método Singleton
Logger& Logger::getInstance() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ == nullptr) {
        instance_ = new Logger();
    }
    return *instance_;
}

void Logger::setArquivoLog(const std::string& caminho) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Fecha arquivo anterior se houver
    if (arquivoLog_.is_open()) {
        arquivoLog_.close();
    }
    
    caminhoArquivo_ = caminho;
    arquivoLog_.open(caminhoArquivo_, std::ios::app);
    usarArquivo_ = arquivoLog_.is_open();
    
    if (usarArquivo_) {
        arquivoLog_ << "\n========== Nova Sessão: " << getTimestamp() << " ==========\n";
        arquivoLog_.flush();
    }
}

void Logger::log(LogLevel level, const std::string& contexto, const std::string& mensagem) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string logLine = "[" + getTimestamp() + "] [" + getLevelString(level) + "] " +
                          "[" + contexto + "] " + mensagem;
    
    // Log em arquivo
    if (usarArquivo_ && arquivoLog_.is_open()) {
        arquivoLog_ << logLine << std::endl;
        arquivoLog_.flush();
    }
    
    // Log no console (apenas para INFO, WARNING, ERROR se não estiver em runtime)
    if (!runtimeStarted && (level == LogLevel::INFO || level == LogLevel::WARNING || level == LogLevel::ERROR)) {
        std::cout << logLine << std::endl;
    }
}

void Logger::fecharArquivo() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (arquivoLog_.is_open()) {
        arquivoLog_ << "========== Sessão Finalizada: " << getTimestamp() << " ==========\n\n";
        arquivoLog_.close();
    }
    usarArquivo_ = false;
}

std::string Logger::getLevelString(LogLevel level) const {
    switch (level) {
        case LogLevel::STARTUP:  return "STARTUP";
        case LogLevel::SHUTDOWN: return "SHUTDOWN";
        case LogLevel::RUNTIME:  return "RUNTIME";
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARNING";
        case LogLevel::ERROR:    return "ERROR";
        default:                 return "UNKNOWN";
    }
}

std::string Logger::getTimestamp() const {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
    return std::string(buffer);
}

// Métodos estáticos originais (mantidos para compatibilidade)
void Logger::setDebugMode(bool enabled) {
    showDebug = enabled;
}

void Logger::setRuntimeMode(bool started) {
    runtimeStarted = started;
}

void Logger::log(LogLevel level, const std::string& message) {
    switch (level) {
        case LogLevel::STARTUP:
        case LogLevel::SHUTDOWN:
            std::cout << message << std::endl;
            break;
            
        case LogLevel::RUNTIME:
            if (runtimeStarted) {
                std::cout << message << std::endl;
            }
            break;
            
        case LogLevel::DEBUG:
            if (showDebug && !runtimeStarted) {
                std::cout << message << std::endl;
            }
            break;
    }
}

void Logger::logRuntime(const std::string& status, float flowIN, float flowOUT, int newCounter, int pos) {
    if (!runtimeStarted) return;
    
    // Converte m³/s para m³/h para melhor visualização
    float flowIN_m3h = flowIN * 3600.0f;  // m³/s * 3600s/h
    float flowOUT_m3h = flowOUT * 3600.0f;
    
    // Converte contador de litros para m³
    float counter_m3 = newCounter / 1000.0f; // L para m³
    
    // Limpa completamente a tela antes de desenhar
    std::cout << "\033[2J\033[H"; // Clear screen + move to top
    
    // Exibe as 4 linhas sempre atualizadas
    std::cout << "┌─────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Hidrometro "<< pos << ":" << std::setw(10) << status 
              << " │ Contador: " << std::fixed << std::setprecision(3) << std::setw(8) << counter_m3 << " m³              │" << std::endl;
    std::cout << "│ Vazão IN: " << std::fixed << std::setprecision(2) << std::setw(8) << flowIN_m3h 
              << " m³/h │ Vazão OUT: " << std::setw(8) << flowOUT_m3h << " m³/h      │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────┘" << std::endl;
    std::cout << std::flush;
}

void Logger::clearRuntimeArea() {
    // Limpa a tela completamente
    std::cout << "\033[2J\033[H" << std::flush;
}
// Métodos de conveniência para a Fachada
void Logger::registrarInfo(const std::string& contexto, const std::string& mensagem) {
    log(LogLevel::INFO, contexto, mensagem);
}

void Logger::registrarErro(const std::string& contexto, const std::string& mensagem) {
    log(LogLevel::ERROR, contexto, mensagem);
}

void Logger::registrarDebug(const std::string& contexto, const std::string& mensagem) {
    if (showDebug) {
        log(LogLevel::DEBUG, contexto, mensagem);
    }
}

void Logger::registrarAviso(const std::string& contexto, const std::string& mensagem) {
    log(LogLevel::WARNING, contexto, mensagem);
}
