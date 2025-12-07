#include "deteccao_vazamento_strategy.hpp"
#include <sstream>
#include <iomanip>

bool DeteccaoVazamentoStrategy::analisar(double consumoAtual, const std::string& valorParametro) const {
    int periodoHoras = extrairPeriodoHoras(valorParametro);
    double fluxoMinimo = calcularFluxoMinimo();
    
    // Detecta vazamento se há consumo constante mínimo por período prolongado
    // Para simplificar, considera que consumoAtual é o consumo por hora
    double consumoPorHora = consumoAtual / 24.0;
    
    // Se consumo por hora é baixo mas constante (indicativo de vazamento)
    return (consumoPorHora >= fluxoMinimo && consumoPorHora <= fluxoMinimo * 3);
}

std::string DeteccaoVazamentoStrategy::getNome() const {
    return "DETECCAO_VAZAMENTO";
}

std::string DeteccaoVazamentoStrategy::gerarMensagem(double consumoAtual, const std::string& valorParametro) const {
    int periodoHoras = extrairPeriodoHoras(valorParametro);
    double consumoPorHora = consumoAtual / 24.0;
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Possível vazamento detectado: consumo constante de " 
       << consumoPorHora << "L/h por mais de " << periodoHoras << " horas";
    return ss.str();
}

int DeteccaoVazamentoStrategy::extrairPeriodoHoras(const std::string& valorParametro) const {
    try {
        // Remove sufixo "h" se presente
        std::string valor = valorParametro;
        size_t pos = valor.find('h');
        if (pos != std::string::npos) {
            valor = valor.substr(0, pos);
        }
        return std::stoi(valor);
    } catch (...) {
        return 24; // Valor padrão: 24 horas
    }
}

double DeteccaoVazamentoStrategy::calcularFluxoMinimo() const {
    // Fluxo mínimo considerado como vazamento: 2L/hora
    return 2.0;
}
