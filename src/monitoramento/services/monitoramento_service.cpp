#include "monitoramento_service.hpp"
#include "../../utils/logger.hpp"
#include <stdexcept>

MonitoramentoService::MonitoramentoService(
    std::shared_ptr<ProcessadorOCR> ocr,
    std::shared_ptr<LeituraDAO> repositorio)
    : ocr_(ocr), repositorio_(repositorio) {
    
    if (!ocr_) {
        throw std::invalid_argument("ProcessadorOCR não pode ser nulo");
    }
    
    if (!repositorio_) {
        throw std::invalid_argument("LeituraDAO não pode ser nulo");
    }
    
    Logger::getInstance().log(LogLevel::INFO, 
        "MonitoramentoService::MonitoramentoService", 
        "Serviço de monitoramento inicializado");
}

int MonitoramentoService::processarLeitura(
    const std::string& idSha, 
    const std::string& caminhoImagem) {
    
    Logger::getInstance().log(LogLevel::INFO, 
        "MonitoramentoService::processarLeitura", 
        "Processando leitura para SHA " + idSha);
    
    try {
        // Usa o OCR para extrair o valor da imagem (padrão Adapter)
        int valor = ocr_->extrairNumeros(caminhoImagem);
        
        // Cria objeto Leitura
        Leitura leitura(0, idSha, valor, std::time(nullptr));
        
        // Persiste no repositório
        if (repositorio_->salvarLeitura(leitura)) {
            Logger::getInstance().log(LogLevel::INFO, 
                "MonitoramentoService::processarLeitura", 
                "Leitura processada com sucesso: " + std::to_string(valor) + "L");
            return leitura.getId();
        }
        
        Logger::getInstance().log(LogLevel::ERROR, 
            "MonitoramentoService::processarLeitura", 
            "Falha ao salvar leitura");
        return 0;
        
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, 
            "MonitoramentoService::processarLeitura", 
            "Erro ao processar leitura: " + std::string(e.what()));
        return 0;
    }
}

int MonitoramentoService::registrarLeituraManual(const std::string& idSha, int valor) {
    Logger::getInstance().log(LogLevel::INFO, 
        "MonitoramentoService::registrarLeituraManual", 
        "Registrando leitura manual para SHA " + idSha + ": " + std::to_string(valor) + "L");
    
    Leitura leitura(0, idSha, valor, std::time(nullptr));
    
    if (repositorio_->salvarLeitura(leitura)) {
        Logger::getInstance().log(LogLevel::INFO, 
            "MonitoramentoService::registrarLeituraManual", 
            "Leitura manual registrada com sucesso");
        return leitura.getId();
    }
    
    return 0;
}

std::shared_ptr<ConsumoMonitoravel> MonitoramentoService::construirConsumoHidrometro(
    const std::string& idSha) {
    
    Logger::getInstance().log(LogLevel::DEBUG, 
        "MonitoramentoService::construirConsumoHidrometro", 
        "Construindo ConsumoHidrometro para SHA " + idSha);
    
    return std::make_shared<ConsumoHidrometro>(idSha, repositorio_);
}

std::shared_ptr<ConsumoMonitoravel> MonitoramentoService::construirConsumoUsuario(
    int idUsuario, 
    const std::vector<std::string>& listaShas) {
    
    Logger::getInstance().log(LogLevel::INFO, 
        "MonitoramentoService::construirConsumoUsuario", 
        "Construindo ConsumoUsuario para usuário " + std::to_string(idUsuario) + 
        " com " + std::to_string(listaShas.size()) + " hidrômetros");
    
    // Cria o Composite
    auto consumoUsuario = std::make_shared<ConsumoUsuario>(idUsuario);
    
    // Adiciona cada hidrômetro ao Composite
    for (const auto& idSha : listaShas) {
        auto consumoHidrometro = construirConsumoHidrometro(idSha);
        consumoUsuario->adicionarHidrometro(consumoHidrometro);
    }
    
    return consumoUsuario;
}

double MonitoramentoService::consultarConsumo(
    std::shared_ptr<ConsumoMonitoravel> monitoravel,
    std::time_t dataInicio,
    std::time_t dataFim) {
    
    if (!monitoravel) {
        Logger::getInstance().log(LogLevel::ERROR, 
            "MonitoramentoService::consultarConsumo", 
            "ConsumoMonitoravel nulo");
        return 0.0;
    }
    
    Logger::getInstance().log(LogLevel::INFO, 
        "MonitoramentoService::consultarConsumo", 
        "Consultando consumo de " + monitoravel->obterDescricao());
    
    // Chama o método polimórfico (funciona para Leaf ou Composite)
    return monitoravel->calcularConsumo(dataInicio, dataFim);
}

double MonitoramentoService::consultarConsumoHidrometro(
    const std::string& idSha,
    std::time_t dataInicio,
    std::time_t dataFim) {
    
    return repositorio_->consultarConsumo(idSha, dataInicio, dataFim);
}

double MonitoramentoService::consultarConsumoAgregado(
    const std::vector<std::string>& listaShas,
    std::time_t dataInicio,
    std::time_t dataFim) {
    
    return repositorio_->consultarConsumoAgregado(listaShas, dataInicio, dataFim);
}

std::vector<Leitura> MonitoramentoService::obterLeituras(
    const std::string& idSha,
    std::time_t dataInicio,
    std::time_t dataFim) {
    
    return repositorio_->consultarLeituras(idSha, dataInicio, dataFim);
}

double MonitoramentoService::calcularConsumoRecente(
    const std::string& idSha, 
    int periodoHoras) {
    
    // Calcula timestamps
    std::time_t agora = std::time(nullptr);
    std::time_t inicio = agora - (periodoHoras * 3600);
    
    Logger::getInstance().log(LogLevel::DEBUG, 
        "MonitoramentoService::calcularConsumoRecente", 
        "Calculando consumo das últimas " + std::to_string(periodoHoras) + 
        "h para SHA " + idSha);
    
    return repositorio_->consultarConsumo(idSha, inicio, agora);
}

int MonitoramentoService::removerLeituras(const std::string& idSha) {
    Logger::getInstance().log(LogLevel::WARNING, 
        "MonitoramentoService::removerLeituras", 
        "Removendo leituras do SHA " + idSha);
    
    return repositorio_->removerLeituras(idSha);
}

int MonitoramentoService::contarLeituras(const std::string& idSha) {
    return repositorio_->contarLeituras(idSha);
}
