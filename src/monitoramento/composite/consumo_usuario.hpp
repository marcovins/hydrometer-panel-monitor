#ifndef CONSUMO_USUARIO_HPP
#define CONSUMO_USUARIO_HPP

#include "consumo_monitoravel.hpp"
#include <vector>
#include <memory>

/**
 * @brief Composite do padrão Composite - Representa o consumo agregado de um usuário
 * 
 * Esta classe implementa a interface ConsumoMonitoravel e pode conter
 * múltiplos componentes (hidrômetros). O consumo total é a soma dos
 * consumos de todos os hidrômetros associados.
 * 
 * Padrão de Projeto: Composite (Composite)
 */
class ConsumoUsuario : public ConsumoMonitoravel {
public:
    /**
     * @brief Construtor
     * @param idUsuario Identificador do usuário
     */
    explicit ConsumoUsuario(int idUsuario);
    
    /**
     * @brief Destrutor
     */
    virtual ~ConsumoUsuario() = default;
    
    /**
     * @brief Adiciona um hidrômetro à lista
     * @param hidrometro Ponteiro para ConsumoMonitoravel (geralmente ConsumoHidrometro)
     */
    void adicionarHidrometro(std::shared_ptr<ConsumoMonitoravel> hidrometro);
    
    /**
     * @brief Remove um hidrômetro da lista
     * @param hidrometro Ponteiro para o hidrômetro a ser removido
     */
    void removerHidrometro(std::shared_ptr<ConsumoMonitoravel> hidrometro);
    
    /**
     * @brief Calcula o consumo total de todos os hidrômetros
     * @param dataInicio Timestamp de início
     * @param dataFim Timestamp de fim
     * @return Soma do consumo de todos os hidrômetros em litros
     */
    double calcularConsumo(std::time_t dataInicio, std::time_t dataFim) override;
    
    /**
     * @brief Obtém o ID do usuário
     * @return ID do usuário como string
     */
    std::string obterIdentificador() const override;
    
    /**
     * @brief Obtém descrição do usuário
     * @return String descritiva
     */
    std::string obterDescricao() const override;
    
    /**
     * @brief Obtém o número de hidrômetros associados
     * @return Quantidade de hidrômetros
     */
    size_t getNumeroHidrometros() const { return hidrometros_.size(); }
    
    /**
     * @brief Obtém a lista de hidrômetros
     * @return Vetor de ponteiros para ConsumoMonitoravel
     */
    const std::vector<std::shared_ptr<ConsumoMonitoravel>>& getHidrometros() const {
        return hidrometros_;
    }
    
private:
    int idUsuario_;
    std::vector<std::shared_ptr<ConsumoMonitoravel>> hidrometros_;
};

#endif // CONSUMO_USUARIO_HPP
