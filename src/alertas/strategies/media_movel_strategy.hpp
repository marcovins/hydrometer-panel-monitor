#ifndef MEDIA_MOVEL_STRATEGY_HPP
#define MEDIA_MOVEL_STRATEGY_HPP

#include "estrategia_analise_consumo.hpp"
#include <vector>
#include <map>

/**
 * @brief Estratégia que detecta anomalias comparando com média histórica
 * 
 * Esta estratégia calcula a média móvel de consumo dos últimos períodos
 * e dispara alerta se o consumo atual exceder significativamente a média.
 * 
 * Exemplo: Se a média dos últimos 7 dias é 50L e hoje foi 100L (100% acima),
 * o alerta é disparado.
 */
class MediaMovelStrategy : public EstrategiaAnaliseConsumo {
public:
    bool analisar(double consumoAtual, const std::string& valorParametro) const override;
    std::string getNome() const override;
    std::string gerarMensagem(double consumoAtual, const std::string& valorParametro) const override;

    // Métodos auxiliares para gerenciar histórico (simplificado para demo)
    void adicionarConsumo(int usuarioId, double consumo);
    double calcularMedia(int usuarioId, int periodos) const;

private:
    double extrairPercentualDesvio(const std::string& valorParametro) const;
    
    // Armazenamento simplificado do histórico (em produção, viria do banco)
    mutable std::map<int, std::vector<double>> historicoConsumo;
};

#endif // MEDIA_MOVEL_STRATEGY_HPP
