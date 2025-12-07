#ifndef CONSUMO_MONITORAVEL_HPP
#define CONSUMO_MONITORAVEL_HPP

#include <string>
#include <ctime>

/**
 * @brief Interface Component do padrão Composite
 * 
 * Define a interface comum para calcular consumo tanto de um
 * hidrômetro individual (Leaf) quanto de um conjunto de
 * hidrômetros de um usuário (Composite).
 * 
 * Padrão de Projeto: Composite
 */
class ConsumoMonitoravel {
public:
    virtual ~ConsumoMonitoravel() = default;
    
    /**
     * @brief Calcula o consumo no período especificado
     * @param dataInicio Timestamp de início do período
     * @param dataFim Timestamp de fim do período
     * @return Consumo total em litros
     */
    virtual double calcularConsumo(std::time_t dataInicio, std::time_t dataFim) = 0;
    
    /**
     * @brief Obtém o identificador do componente
     * @return String identificadora (ID do SHA ou ID do usuário)
     */
    virtual std::string obterIdentificador() const = 0;
    
    /**
     * @brief Obtém uma descrição do componente
     * @return String descritiva
     */
    virtual std::string obterDescricao() const = 0;
};

#endif // CONSUMO_MONITORAVEL_HPP
