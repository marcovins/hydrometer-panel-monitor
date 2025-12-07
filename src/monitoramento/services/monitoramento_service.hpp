#ifndef MONITORAMENTO_SERVICE_HPP
#define MONITORAMENTO_SERVICE_HPP

#include "../adapter/processador_ocr.hpp"
#include "../storage/leitura_dao.hpp"
#include "../composite/consumo_monitoravel.hpp"
#include "../composite/consumo_hidrometro.hpp"
#include "../composite/consumo_usuario.hpp"
#include "../domain/leitura.hpp"
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Serviço principal do subsistema de monitoramento
 * 
 * Coordena todas as operações do subsistema:
 * - Processamento de imagens via OCR (padrão Adapter)
 * - Persistência de leituras
 * - Construção de estruturas Composite para consulta de consumo
 * - Interface para a Fachada do sistema
 * 
 * Este serviço implementa a lógica de negócio do subsistema de
 * coleta e monitoramento conforme especificação.
 */
class MonitoramentoService {
public:
    /**
     * @brief Construtor
     * @param ocr Processador OCR para extrair dados de imagens
     * @param repositorio DAO para persistência de leituras
     */
    MonitoramentoService(
        std::shared_ptr<ProcessadorOCR> ocr,
        std::shared_ptr<LeituraDAO> repositorio);
    
    /**
     * @brief Destrutor
     */
    ~MonitoramentoService() = default;
    
    /**
     * @brief Processa uma imagem de hidrômetro e salva a leitura
     * 
     * Fluxo:
     * 1. Usa o OCR para extrair o valor da imagem
     * 2. Cria um objeto Leitura
     * 3. Persiste no repositório
     * 
     * @param idSha ID do hidrômetro
     * @param caminhoImagem Caminho para a imagem
     * @return ID da leitura salva, ou 0 se falhou
     */
    int processarLeitura(const std::string& idSha, const std::string& caminhoImagem);
    
    /**
     * @brief Registra uma leitura manual (sem OCR)
     * @param idSha ID do hidrômetro
     * @param valor Valor da leitura em litros
     * @return ID da leitura salva, ou 0 se falhou
     */
    int registrarLeituraManual(const std::string& idSha, int valor);
    
    /**
     * @brief Constrói um objeto Composite para consultar consumo de um hidrômetro
     * @param idSha ID do hidrômetro
     * @return Ponteiro para ConsumoMonitoravel (ConsumoHidrometro)
     */
    std::shared_ptr<ConsumoMonitoravel> construirConsumoHidrometro(const std::string& idSha);
    
    /**
     * @brief Constrói um objeto Composite para consultar consumo de um usuário
     * 
     * Busca todos os hidrômetros vinculados ao usuário e cria uma
     * estrutura Composite que agrega o consumo de todos.
     * 
     * @param idUsuario ID do usuário
     * @param listaShas Lista de IDs dos hidrômetros do usuário
     * @return Ponteiro para ConsumoMonitoravel (ConsumoUsuario)
     */
    std::shared_ptr<ConsumoMonitoravel> construirConsumoUsuario(
        int idUsuario, 
        const std::vector<std::string>& listaShas);
    
    /**
     * @brief Consulta o consumo usando um objeto ConsumoMonitoravel
     * 
     * Método unificado que funciona tanto para hidrômetro individual
     * quanto para usuário (graças ao padrão Composite).
     * 
     * @param monitoravel Objeto ConsumoMonitoravel
     * @param dataInicio Timestamp de início
     * @param dataFim Timestamp de fim
     * @return Consumo em litros
     */
    double consultarConsumo(
        std::shared_ptr<ConsumoMonitoravel> monitoravel,
        std::time_t dataInicio,
        std::time_t dataFim);
    
    /**
     * @brief Consulta direta do consumo de um hidrômetro
     * @param idSha ID do hidrômetro
     * @param dataInicio Timestamp de início
     * @param dataFim Timestamp de fim
     * @return Consumo em litros
     */
    double consultarConsumoHidrometro(
        const std::string& idSha,
        std::time_t dataInicio,
        std::time_t dataFim);
    
    /**
     * @brief Consulta direta do consumo agregado de múltiplos hidrômetros
     * @param listaShas Lista de IDs dos hidrômetros
     * @param dataInicio Timestamp de início
     * @param dataFim Timestamp de fim
     * @return Consumo total em litros
     */
    double consultarConsumoAgregado(
        const std::vector<std::string>& listaShas,
        std::time_t dataInicio,
        std::time_t dataFim);
    
    /**
     * @brief Obtém todas as leituras de um hidrômetro em um período
     * @param idSha ID do hidrômetro
     * @param dataInicio Timestamp de início
     * @param dataFim Timestamp de fim
     * @return Vetor de leituras
     */
    std::vector<Leitura> obterLeituras(
        const std::string& idSha,
        std::time_t dataInicio,
        std::time_t dataFim);
    
    /**
     * @brief Calcula o consumo recente de um hidrômetro
     * 
     * Útil para o subsistema de alertas verificar regras.
     * 
     * @param idSha ID do hidrômetro
     * @param periodoHoras Período em horas (ex: 24 para um dia)
     * @return Consumo no período em litros
     */
    double calcularConsumoRecente(const std::string& idSha, int periodoHoras);
    
    /**
     * @brief Remove todas as leituras de um hidrômetro
     * @param idSha ID do hidrômetro
     * @return Número de leituras removidas
     */
    int removerLeituras(const std::string& idSha);
    
    /**
     * @brief Conta quantas leituras existem para um hidrômetro
     * @param idSha ID do hidrômetro
     * @return Número de leituras
     */
    int contarLeituras(const std::string& idSha);
    
    /**
     * @brief Obtém referência ao repositório
     * @return Ponteiro compartilhado para o DAO
     */
    std::shared_ptr<LeituraDAO> getRepositorio() const { return repositorio_; }
    
    /**
     * @brief Obtém referência ao processador OCR
     * @return Ponteiro compartilhado para o OCR
     */
    std::shared_ptr<ProcessadorOCR> getOCR() const { return ocr_; }
    
private:
    std::shared_ptr<ProcessadorOCR> ocr_;
    std::shared_ptr<LeituraDAO> repositorio_;
};

#endif // MONITORAMENTO_SERVICE_HPP
