#ifndef CONSUMO_HIDROMETRO_HPP
#define CONSUMO_HIDROMETRO_HPP

#include "consumo_monitoravel.hpp"
#include <memory>

// Forward declaration
class LeituraDAO;

/**
 * @brief Leaf do padrão Composite - Representa o consumo de um hidrômetro individual
 * 
 * Esta classe implementa a interface ConsumoMonitoravel para calcular
 * o consumo de um único hidrômetro (SHA). Consulta o repositório de
 * leituras para obter os dados.
 * 
 * Padrão de Projeto: Composite (Leaf)
 */
class ConsumoHidrometro : public ConsumoMonitoravel {
public:
    /**
     * @brief Construtor
     * @param idSha Identificador do hidrômetro
     * @param repositorio Ponteiro para o DAO de leituras
     */
    ConsumoHidrometro(const std::string& idSha, std::shared_ptr<LeituraDAO> repositorio);
    
    /**
     * @brief Destrutor
     */
    virtual ~ConsumoHidrometro() = default;
    
    /**
     * @brief Calcula o consumo do hidrômetro no período
     * @param dataInicio Timestamp de início
     * @param dataFim Timestamp de fim
     * @return Consumo em litros
     */
    double calcularConsumo(std::time_t dataInicio, std::time_t dataFim) override;
    
    /**
     * @brief Obtém o ID do hidrômetro
     * @return ID do SHA
     */
    std::string obterIdentificador() const override;
    
    /**
     * @brief Obtém descrição do hidrômetro
     * @return String descritiva
     */
    std::string obterDescricao() const override;
    
private:
    std::string idSha_;
    std::shared_ptr<LeituraDAO> repositorio_;
};

#endif // CONSUMO_HIDROMETRO_HPP
