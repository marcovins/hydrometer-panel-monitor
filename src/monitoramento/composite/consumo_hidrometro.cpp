#include "consumo_hidrometro.hpp"
#include "../storage/leitura_dao.hpp"
#include "../../utils/logger.hpp"

ConsumoHidrometro::ConsumoHidrometro(const std::string& idSha, std::shared_ptr<LeituraDAO> repositorio)
    : idSha_(idSha), repositorio_(repositorio) {
}

double ConsumoHidrometro::calcularConsumo(std::time_t dataInicio, std::time_t dataFim) {
    if (!repositorio_) {
        Logger::getInstance().log(LogLevel::ERROR, 
            "ConsumoHidrometro::calcularConsumo", 
            "Repositório não inicializado para SHA: " + idSha_);
        return 0.0;
    }
    
    double consumo = repositorio_->consultarConsumo(idSha_, dataInicio, dataFim);
    
    Logger::getInstance().log(LogLevel::INFO, 
        "ConsumoHidrometro::calcularConsumo", 
        "SHA " + idSha_ + " - Consumo: " + std::to_string(consumo) + "L");
    
    return consumo;
}

std::string ConsumoHidrometro::obterIdentificador() const {
    return idSha_;
}

std::string ConsumoHidrometro::obterDescricao() const {
    return "Hidrômetro SHA-" + idSha_;
}
