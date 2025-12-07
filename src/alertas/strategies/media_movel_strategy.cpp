#include "media_movel_strategy.hpp"
#include <sstream>
#include <iomanip>
#include <numeric>

bool MediaMovelStrategy::analisar(double consumoAtual, const std::string& valorParametro) const {
    double percentualDesvio = extrairPercentualDesvio(valorParametro);
    
    // Para demonstração, usa um valor fixo de média histórica
    // Em produção, isso viria do banco de dados
    double mediaHistorica = 50.0; // Valor simulado
    
    double limiteMaximo = mediaHistorica * (1.0 + percentualDesvio / 100.0);
    return consumoAtual > limiteMaximo;
}

std::string MediaMovelStrategy::getNome() const {
    return "MEDIA_MOVEL";
}

std::string MediaMovelStrategy::gerarMensagem(double consumoAtual, const std::string& valorParametro) const {
    double percentualDesvio = extrairPercentualDesvio(valorParametro);
    double mediaHistorica = 50.0; // Valor simulado
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Consumo atual de " << consumoAtual << "L excede em "
       << percentualDesvio << "% a média histórica de " << mediaHistorica << "L";
    return ss.str();
}

void MediaMovelStrategy::adicionarConsumo(int usuarioId, double consumo) {
    historicoConsumo[usuarioId].push_back(consumo);
    
    // Mantém apenas os últimos 30 valores para não crescer indefinidamente
    if (historicoConsumo[usuarioId].size() > 30) {
        historicoConsumo[usuarioId].erase(historicoConsumo[usuarioId].begin());
    }
}

double MediaMovelStrategy::calcularMedia(int usuarioId, int periodos) const {
    auto it = historicoConsumo.find(usuarioId);
    if (it == historicoConsumo.end() || it->second.empty()) {
        return 0.0;
    }
    
    const std::vector<double>& valores = it->second;
    int n = std::min(periodos, static_cast<int>(valores.size()));
    
    if (n == 0) return 0.0;
    
    double soma = std::accumulate(valores.end() - n, valores.end(), 0.0);
    return soma / n;
}

double MediaMovelStrategy::extrairPercentualDesvio(const std::string& valorParametro) const {
    try {
        return std::stod(valorParametro);
    } catch (...) {
        return 50.0; // Valor padrão: 50% acima da média
    }
}
