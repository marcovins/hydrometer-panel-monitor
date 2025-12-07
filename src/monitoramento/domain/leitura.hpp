#ifndef LEITURA_HPP
#define LEITURA_HPP

#include <string>
#include <chrono>
#include <ctime>

/**
 * @brief Entidade de domínio que representa uma leitura de hidrômetro
 * 
 * Esta classe encapsula os dados de uma leitura individual de um
 * hidrômetro (SHA), incluindo identificador, valor lido e timestamp.
 */
class Leitura {
public:
    /**
     * @brief Construtor padrão
     */
    Leitura();
    
    /**
     * @brief Construtor com parâmetros
     * @param id Identificador único da leitura
     * @param idSha Identificador do hidrômetro
     * @param valor Valor lido em litros
     * @param dataHora Timestamp da leitura
     */
    Leitura(int id, const std::string& idSha, int valor, std::time_t dataHora);
    
    // Getters
    int getId() const { return id_; }
    std::string getIdSha() const { return idSha_; }
    int getValor() const { return valor_; }
    std::time_t getDataHora() const { return dataHora_; }
    
    // Setters
    void setId(int id) { id_ = id; }
    void setIdSha(const std::string& idSha) { idSha_ = idSha; }
    void setValor(int valor) { valor_ = valor; }
    void setDataHora(std::time_t dataHora) { dataHora_ = dataHora; }
    
    /**
     * @brief Converte a data/hora para string formatada
     * @return String no formato "DD/MM/YYYY HH:MM:SS"
     */
    std::string getDataHoraFormatada() const;
    
private:
    int id_;
    std::string idSha_;
    int valor_;           // Valor em litros
    std::time_t dataHora_;
};

#endif // LEITURA_HPP
