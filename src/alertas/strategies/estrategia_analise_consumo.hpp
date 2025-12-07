#ifndef ESTRATEGIA_ANALISE_CONSUMO_HPP
#define ESTRATEGIA_ANALISE_CONSUMO_HPP

#include <string>

/**
 * @brief Interface Strategy para análise de consumo
 * 
 * Define o contrato para diferentes algoritmos de análise de consumo
 * de água. Cada estratégia implementa uma lógica diferente para detectar
 * se há uma violação das regras de consumo.
 * 
 * Padrão de Projeto: Strategy
 */
class EstrategiaAnaliseConsumo {
public:
    virtual ~EstrategiaAnaliseConsumo() = default;

    /**
     * @brief Analisa o consumo atual e determina se há violação da regra
     * 
     * @param consumoAtual Consumo atual em litros
     * @param valorParametro Parâmetro configurável da regra (ex: limite, período)
     * @return true se houver violação da regra, false caso contrário
     */
    virtual bool analisar(double consumoAtual, const std::string& valorParametro) const = 0;

    /**
     * @brief Retorna o nome da estratégia
     */
    virtual std::string getNome() const = 0;

    /**
     * @brief Gera mensagem descritiva da análise
     */
    virtual std::string gerarMensagem(double consumoAtual, const std::string& valorParametro) const = 0;
};

#endif // ESTRATEGIA_ANALISE_CONSUMO_HPP
