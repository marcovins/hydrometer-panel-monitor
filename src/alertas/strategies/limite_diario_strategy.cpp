#include "limite_diario_strategy.hpp"
#include <sstream>
#include <iomanip>

bool LimiteDiarioStrategy::analisar(double consumoAtual, const std::string& valorParametro) const {
    double limite = extrairLimite(valorParametro);
    return consumoAtual > limite;
}

std::string LimiteDiarioStrategy::getNome() const {
    return "LIMITE_DIARIO";
}

std::string LimiteDiarioStrategy::gerarMensagem(double consumoAtual, const std::string& valorParametro) const {
    double limite = extrairLimite(valorParametro);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Consumo diário de " << consumoAtual << "L excedeu o limite de " << limite << "L";
    return ss.str();
}

double LimiteDiarioStrategy::extrairLimite(const std::string& valorParametro) const {
    try {
        return std::stod(valorParametro);
    } catch (...) {
        return 70.0; // Valor padrão
    }
}
