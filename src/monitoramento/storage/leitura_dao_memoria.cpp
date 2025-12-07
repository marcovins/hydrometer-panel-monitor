#include "leitura_dao_memoria.hpp"
#include "../../utils/logger.hpp"
#include <algorithm>

LeituraDAOMemoria::LeituraDAOMemoria() 
    : proximoId_(1) {
    Logger::getInstance().log(LogLevel::INFO, 
        "LeituraDAOMemoria::LeituraDAOMemoria", 
        "Repositório de leituras em memória inicializado");
}

bool LeituraDAOMemoria::salvarLeitura(const Leitura& leitura) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Cria uma cópia com ID gerado se necessário
    Leitura novaLeitura = leitura;
    if (novaLeitura.getId() == 0) {
        novaLeitura.setId(proximoId_++);
    }
    
    // Salva a leitura
    leituras_[novaLeitura.getId()] = novaLeitura;
    
    // Indexa por SHA
    leiturasporSha_[novaLeitura.getIdSha()].push_back(novaLeitura.getId());
    
    Logger::getInstance().log(LogLevel::DEBUG, 
        "LeituraDAOMemoria::salvarLeitura", 
        "Leitura ID " + std::to_string(novaLeitura.getId()) + 
        " salva para SHA " + novaLeitura.getIdSha());
    
    return true;
}

Leitura LeituraDAOMemoria::buscarLeitura(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = leituras_.find(id);
    if (it != leituras_.end()) {
        return it->second;
    }
    
    return Leitura(); // Retorna leitura vazia se não encontrada
}

std::vector<Leitura> LeituraDAOMemoria::consultarLeituras(
    const std::string& idSha, 
    std::time_t dataInicio, 
    std::time_t dataFim) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Leitura> resultado;
    
    auto it = leiturasporSha_.find(idSha);
    if (it == leiturasporSha_.end()) {
        return resultado;
    }
    
    // Filtra leituras por data
    for (int idLeitura : it->second) {
        const Leitura& leitura = leituras_[idLeitura];
        std::time_t dataLeitura = leitura.getDataHora();
        
        if (dataLeitura >= dataInicio && dataLeitura <= dataFim) {
            resultado.push_back(leitura);
        }
    }
    
    // Ordena por data
    std::sort(resultado.begin(), resultado.end(), 
        [](const Leitura& a, const Leitura& b) {
            return a.getDataHora() < b.getDataHora();
        });
    
    return resultado;
}

double LeituraDAOMemoria::consultarConsumo(
    const std::string& idSha, 
    std::time_t dataInicio, 
    std::time_t dataFim) {
    
    auto leituras = consultarLeituras(idSha, dataInicio, dataFim);
    
    if (leituras.empty()) {
        return 0.0;
    }
    
    // Calcula consumo como diferença entre última e primeira leitura
    int valorFinal = leituras.back().getValor();
    int valorInicial = leituras.front().getValor();
    double consumo = static_cast<double>(valorFinal - valorInicial);
    
    Logger::getInstance().log(LogLevel::DEBUG, 
        "LeituraDAOMemoria::consultarConsumo", 
        "SHA " + idSha + ": " + std::to_string(leituras.size()) + 
        " leituras, consumo = " + std::to_string(consumo) + "L");
    
    return consumo > 0 ? consumo : 0.0;
}

double LeituraDAOMemoria::consultarConsumoAgregado(
    const std::vector<std::string>& listaShas,
    std::time_t dataInicio,
    std::time_t dataFim) {
    
    double consumoTotal = 0.0;
    
    for (const auto& idSha : listaShas) {
        consumoTotal += consultarConsumo(idSha, dataInicio, dataFim);
    }
    
    Logger::getInstance().log(LogLevel::DEBUG, 
        "LeituraDAOMemoria::consultarConsumoAgregado", 
        std::to_string(listaShas.size()) + " SHAs, consumo total = " + 
        std::to_string(consumoTotal) + "L");
    
    return consumoTotal;
}

int LeituraDAOMemoria::removerLeituras(const std::string& idSha) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = leiturasporSha_.find(idSha);
    if (it == leiturasporSha_.end()) {
        return 0;
    }
    
    int count = 0;
    for (int idLeitura : it->second) {
        leituras_.erase(idLeitura);
        count++;
    }
    
    leiturasporSha_.erase(it);
    
    Logger::getInstance().log(LogLevel::INFO, 
        "LeituraDAOMemoria::removerLeituras", 
        std::to_string(count) + " leituras removidas do SHA " + idSha);
    
    return count;
}

int LeituraDAOMemoria::contarLeituras(const std::string& idSha) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = leiturasporSha_.find(idSha);
    if (it != leiturasporSha_.end()) {
        return it->second.size();
    }
    
    return 0;
}

void LeituraDAOMemoria::limpar() {
    std::lock_guard<std::mutex> lock(mutex_);
    leituras_.clear();
    leiturasporSha_.clear();
    proximoId_ = 1;
    
    Logger::getInstance().log(LogLevel::INFO, 
        "LeituraDAOMemoria::limpar", 
        "Todas as leituras foram removidas");
}
