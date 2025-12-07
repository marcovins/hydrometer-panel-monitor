#ifndef LIMITE_DIARIO_STRATEGY_HPP
#define LIMITE_DIARIO_STRATEGY_HPP

#include "estrategia_analise_consumo.hpp"

/**
 * @brief Estratégia que verifica se o consumo ultrapassou um limite diário
 * 
 * Esta é a estratégia mais simples: compara o consumo atual com um valor fixo.
 * Se o consumo for maior que o limite, dispara um alerta.
 * 
 * Exemplo: Se o limite é 70L/dia e o consumo foi 85L, o alerta é disparado.
 */
class LimiteDiarioStrategy : public EstrategiaAnaliseConsumo {
public:
    bool analisar(double consumoAtual, const std::string& valorParametro) const override;
    std::string getNome() const override;
    std::string gerarMensagem(double consumoAtual, const std::string& valorParametro) const override;

private:
    double extrairLimite(const std::string& valorParametro) const;
};

#endif // LIMITE_DIARIO_STRATEGY_HPP
