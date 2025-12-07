#ifndef MONITORAMENTO_SERVICE_FACTORY_HPP
#define MONITORAMENTO_SERVICE_FACTORY_HPP

#include "monitoramento_service.hpp"
#include "../adapter/adaptador_ocr.hpp"
#include "../storage/leitura_dao_memoria.hpp"
#include <memory>

/**
 * @brief Factory para criar instâncias de MonitoramentoService
 * 
 * Padrão de Projeto: Factory Method
 * 
 * Facilita a criação do serviço com suas dependências configuradas.
 */
class MonitoramentoServiceFactory {
public:
    /**
     * @brief Tipos de armazenamento disponíveis
     */
    enum class TipoArmazenamento {
        MEMORIA,    // Armazenamento em memória (para testes)
        SQLITE      // Armazenamento em banco SQLite (futuro)
    };
    
    /**
     * @brief Cria um MonitoramentoService configurado
     * @param tipo Tipo de armazenamento a usar
     * @return Ponteiro compartilhado para o serviço criado
     */
    static std::shared_ptr<MonitoramentoService> criar(
        TipoArmazenamento tipo = TipoArmazenamento::MEMORIA) {
        
        // Cria o processador OCR
        auto ocr = std::make_shared<AdaptadorOCR>();
        
        // Cria o repositório de acordo com o tipo
        std::shared_ptr<LeituraDAO> repositorio;
        
        switch (tipo) {
            case TipoArmazenamento::MEMORIA:
                repositorio = std::make_shared<LeituraDAOMemoria>();
                break;
            
            case TipoArmazenamento::SQLITE:
                // TODO: Implementar LeituraDAOSqlite no futuro
                // repositorio = std::make_shared<LeituraDAOSqlite>();
                // Por enquanto, usa memória
                repositorio = std::make_shared<LeituraDAOMemoria>();
                break;
        }
        
        // Cria e retorna o serviço
        return std::make_shared<MonitoramentoService>(ocr, repositorio);
    }
    
    /**
     * @brief Cria um MonitoramentoService com componentes customizados
     * @param ocr Processador OCR customizado
     * @param repositorio DAO customizado
     * @return Ponteiro compartilhado para o serviço criado
     */
    static std::shared_ptr<MonitoramentoService> criarCustomizado(
        std::shared_ptr<ProcessadorOCR> ocr,
        std::shared_ptr<LeituraDAO> repositorio) {
        
        return std::make_shared<MonitoramentoService>(ocr, repositorio);
    }
};

#endif // MONITORAMENTO_SERVICE_FACTORY_HPP
