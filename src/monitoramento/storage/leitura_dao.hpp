#ifndef LEITURA_DAO_HPP
#define LEITURA_DAO_HPP

#include "../domain/leitura.hpp"
#include <vector>
#include <string>
#include <memory>
#include <ctime>

/**
 * @brief Data Access Object para persistência de leituras
 * 
 * Gerencia a persistência e consulta de leituras de hidrômetros.
 * Suporta tanto armazenamento em memória quanto em banco de dados SQLite.
 */
class LeituraDAO {
public:
    virtual ~LeituraDAO() = default;
    
    /**
     * @brief Salva uma nova leitura
     * @param leitura Objeto Leitura a ser salvo
     * @return true se salvou com sucesso, false caso contrário
     */
    virtual bool salvarLeitura(const Leitura& leitura) = 0;
    
    /**
     * @brief Consulta uma leitura específica por ID
     * @param id ID da leitura
     * @return Leitura encontrada ou Leitura vazia se não encontrado
     */
    virtual Leitura buscarLeitura(int id) = 0;
    
    /**
     * @brief Consulta todas as leituras de um hidrômetro em um período
     * @param idSha ID do hidrômetro
     * @param dataInicio Timestamp de início
     * @param dataFim Timestamp de fim
     * @return Vetor de leituras encontradas
     */
    virtual std::vector<Leitura> consultarLeituras(
        const std::string& idSha, 
        std::time_t dataInicio, 
        std::time_t dataFim) = 0;
    
    /**
     * @brief Calcula o consumo total de um hidrômetro em um período
     * 
     * Calcula a diferença entre a última e a primeira leitura do período.
     * 
     * @param idSha ID do hidrômetro
     * @param dataInicio Timestamp de início
     * @param dataFim Timestamp de fim
     * @return Consumo em litros
     */
    virtual double consultarConsumo(
        const std::string& idSha, 
        std::time_t dataInicio, 
        std::time_t dataFim) = 0;
    
    /**
     * @brief Calcula o consumo agregado de múltiplos hidrômetros
     * @param listaShas Lista de IDs de hidrômetros
     * @param dataInicio Timestamp de início
     * @param dataFim Timestamp de fim
     * @return Consumo total em litros
     */
    virtual double consultarConsumoAgregado(
        const std::vector<std::string>& listaShas,
        std::time_t dataInicio,
        std::time_t dataFim) = 0;
    
    /**
     * @brief Remove todas as leituras de um hidrômetro
     * @param idSha ID do hidrômetro
     * @return Número de leituras removidas
     */
    virtual int removerLeituras(const std::string& idSha) = 0;
    
    /**
     * @brief Conta quantas leituras existem para um hidrômetro
     * @param idSha ID do hidrômetro
     * @return Número de leituras
     */
    virtual int contarLeituras(const std::string& idSha) = 0;
};

#endif // LEITURA_DAO_HPP
