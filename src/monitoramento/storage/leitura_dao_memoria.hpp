#ifndef LEITURA_DAO_MEMORIA_HPP
#define LEITURA_DAO_MEMORIA_HPP

#include "leitura_dao.hpp"
#include <map>
#include <mutex>

/**
 * @brief Implementação em memória do LeituraDAO
 * 
 * Armazena leituras em memória RAM usando estruturas de dados STL.
 * Útil para testes e prototipagem rápida.
 * Thread-safe através de mutex.
 */
class LeituraDAOMemoria : public LeituraDAO {
public:
    LeituraDAOMemoria();
    virtual ~LeituraDAOMemoria() = default;
    
    bool salvarLeitura(const Leitura& leitura) override;
    Leitura buscarLeitura(int id) override;
    std::vector<Leitura> consultarLeituras(
        const std::string& idSha, 
        std::time_t dataInicio, 
        std::time_t dataFim) override;
    double consultarConsumo(
        const std::string& idSha, 
        std::time_t dataInicio, 
        std::time_t dataFim) override;
    double consultarConsumoAgregado(
        const std::vector<std::string>& listaShas,
        std::time_t dataInicio,
        std::time_t dataFim) override;
    int removerLeituras(const std::string& idSha) override;
    int contarLeituras(const std::string& idSha) override;
    
    /**
     * @brief Limpa todos os dados em memória
     */
    void limpar();
    
private:
    // Mapa: ID da leitura -> Leitura
    std::map<int, Leitura> leituras_;
    
    // Mapa: ID do SHA -> Lista de IDs de leituras
    std::map<std::string, std::vector<int>> leiturasporSha_;
    
    // Contador de IDs auto-incremento
    int proximoId_;
    
    // Mutex para thread-safety
    mutable std::mutex mutex_;
};

#endif // LEITURA_DAO_MEMORIA_HPP
