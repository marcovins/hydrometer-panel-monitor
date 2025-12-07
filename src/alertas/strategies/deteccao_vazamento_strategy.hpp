#ifndef DETECCAO_VAZAMENTO_STRATEGY_HPP
#define DETECCAO_VAZAMENTO_STRATEGY_HPP

#include "estrategia_analise_consumo.hpp"

/**
 * @brief Estratégia que detecta possíveis vazamentos
 * 
 * Esta estratégia monitora consumo contínuo mesmo em horários atípicos
 * (ex: madrugada) ou fluxo constante por período prolongado, indicando
 * possível vazamento.
 * 
 * Exemplo: Se detecta consumo de 5L/h constante por 24h seguidas,
 * provavelmente há um vazamento.
 */
class DeteccaoVazamentoStrategy : public EstrategiaAnaliseConsumo {
public:
    bool analisar(double consumoAtual, const std::string& valorParametro) const override;
    std::string getNome() const override;
    std::string gerarMensagem(double consumoAtual, const std::string& valorParametro) const override;

private:
    int extrairPeriodoHoras(const std::string& valorParametro) const;
    double calcularFluxoMinimo() const;
};

#endif // DETECCAO_VAZAMENTO_STRATEGY_HPP
