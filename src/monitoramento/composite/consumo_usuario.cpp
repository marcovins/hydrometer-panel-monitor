#include "consumo_usuario.hpp"
#include "../../utils/logger.hpp"
#include <algorithm>

ConsumoUsuario::ConsumoUsuario(int idUsuario) 
    : idUsuario_(idUsuario) {
}

void ConsumoUsuario::adicionarHidrometro(std::shared_ptr<ConsumoMonitoravel> hidrometro) {
    if (hidrometro) {
        hidrometros_.push_back(hidrometro);
        Logger::getInstance().log(LogLevel::INFO, 
            "ConsumoUsuario::adicionarHidrometro", 
            "Hidrômetro " + hidrometro->obterIdentificador() + 
            " adicionado ao usuário " + std::to_string(idUsuario_));
    }
}

void ConsumoUsuario::removerHidrometro(std::shared_ptr<ConsumoMonitoravel> hidrometro) {
    auto it = std::find(hidrometros_.begin(), hidrometros_.end(), hidrometro);
    if (it != hidrometros_.end()) {
        std::string idHidrometro = (*it)->obterIdentificador();
        hidrometros_.erase(it);
        Logger::getInstance().log(LogLevel::INFO, 
            "ConsumoUsuario::removerHidrometro", 
            "Hidrômetro " + idHidrometro + 
            " removido do usuário " + std::to_string(idUsuario_));
    }
}

double ConsumoUsuario::calcularConsumo(std::time_t dataInicio, std::time_t dataFim) {
    double consumoTotal = 0.0;
    
    Logger::getInstance().log(LogLevel::INFO, 
        "ConsumoUsuario::calcularConsumo", 
        "Calculando consumo do usuário " + std::to_string(idUsuario_) + 
        " com " + std::to_string(hidrometros_.size()) + " hidrômetros");
    
    // Soma o consumo de todos os hidrômetros (implementação do Composite)
    for (const auto& hidrometro : hidrometros_) {
        double consumo = hidrometro->calcularConsumo(dataInicio, dataFim);
        consumoTotal += consumo;
    }
    
    Logger::getInstance().log(LogLevel::INFO, 
        "ConsumoUsuario::calcularConsumo", 
        "Usuário " + std::to_string(idUsuario_) + 
        " - Consumo total: " + std::to_string(consumoTotal) + "L");
    
    return consumoTotal;
}

std::string ConsumoUsuario::obterIdentificador() const {
    return std::to_string(idUsuario_);
}

std::string ConsumoUsuario::obterDescricao() const {
    return "Usuário #" + std::to_string(idUsuario_) + 
           " (" + std::to_string(hidrometros_.size()) + " hidrômetros)";
}
