#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <mutex>
#include <ctime>

enum class LogLevel {
    STARTUP,    // Logs de inicialização
    SHUTDOWN,   // Logs de finalização
    RUNTIME,    // Logs em tempo real (sempre visíveis)
    DEBUG,      // Logs de debug (opcionais)
    INFO,       // Logs informativos
    WARNING,    // Avisos
    ERROR       // Erros
};

class Logger {
private:
    static bool showDebug;
    static bool runtimeStarted;
    
    // Singleton
    static Logger* instance_;
    static std::mutex mutex_;
    std::ofstream arquivoLog_;
    std::string caminhoArquivo_;
    bool usarArquivo_;
    
    // Construtor privado para Singleton
    Logger();
    
    // Previne cópia
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

public:
    ~Logger();
    
    // Acesso à instância Singleton
    static Logger& getInstance();
    
    // Métodos estáticos originais (mantidos para compatibilidade)
    static void setDebugMode(bool enabled);
    static void setRuntimeMode(bool started);
    static void log(LogLevel level, const std::string& message);
    static void logRuntime(const std::string& status, float flowIN, float flowOUT, int newCounter, int pos);
    static void clearRuntimeArea();
    
    // Novos métodos para logging em arquivo (Singleton)
    void setArquivoLog(const std::string& caminho);
    void log(LogLevel level, const std::string& contexto, const std::string& mensagem);
    void fecharArquivo();
    
    // Métodos de conveniência para a Fachada
    void registrarInfo(const std::string& contexto, const std::string& mensagem);
    void registrarErro(const std::string& contexto, const std::string& mensagem);
    void registrarDebug(const std::string& contexto, const std::string& mensagem);
    void registrarAviso(const std::string& contexto, const std::string& mensagem);
    
private:
    std::string getLevelString(LogLevel level) const;
    std::string getTimestamp() const;
};

#endif
