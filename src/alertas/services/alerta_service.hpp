#ifndef ALERTA_SERVICE_HPP
#define ALERTA_SERVICE_HPP

#include "../domain/regra_alerta.hpp"
#include "../domain/alerta_ativo.hpp"
#include "../strategies/estrategia_analise_consumo.hpp"
#include "../notifications/notificacao_strategy.hpp"
#include "../observers/alert_observer.hpp"
#include <vector>
#include <map>
#include <memory>
#include <string>

/**
 * @brief Serviço principal do subsistema de alertas
 * 
 * Este serviço atua como Subject no padrão Observer e coordena
 * as estratégias de análise e notificação.
 * 
 * Padrões implementados:
 * - Observer: Gerencia lista de observers e os notifica
 * - Strategy: Usa estratégias configuráveis para análise e notificação
 * 
 * Responsabilidades:
 * - Gerenciar regras de alerta
 * - Verificar violações de consumo
 * - Notificar observers quando alertas são disparados
 * - Manter histórico de alertas ativos
 */
class AlertaService {
private:
    // Padrão Observer
    std::vector<std::shared_ptr<AlertObserver>> observers;

    // Padrão Strategy - Análise
    std::map<std::string, std::shared_ptr<EstrategiaAnaliseConsumo>> estrategiasAnalise;

    // Padrão Strategy - Notificação
    std::shared_ptr<NotificacaoStrategy> notificacaoStrategy;

    // Dados
    std::vector<RegraAlerta> regrasAlerta;
    std::vector<AlertaAtivo> alertasAtivos;
    
    // Contadores
    int proximoIdRegra;
    int proximoIdAlerta;

public:
    AlertaService();

    // ==================== Padrão Observer ====================
    
    /**
     * @brief Anexa um observer à lista de notificação
     */
    void anexarObserver(std::shared_ptr<AlertObserver> observer);

    /**
     * @brief Remove um observer da lista
     */
    void desanexarObserver(std::shared_ptr<AlertObserver> observer);

    /**
     * @brief Notifica todos os observers sobre um alerta
     */
    void notificarObservers(const AlertaAtivo& alerta);

    // ==================== Gerenciamento de Regras ====================
    
    /**
     * @brief Salva uma nova regra de alerta para um usuário
     * 
     * @param usuarioId ID do usuário
     * @param tipoEstrategia Tipo de análise ("LIMITE_DIARIO", "MEDIA_MOVEL", etc)
     * @param valorParametro Parâmetro da regra (ex: "70" para limite)
     * @return ID da regra criada
     */
    int salvarRegra(int usuarioId, const std::string& tipoEstrategia, 
                    const std::string& valorParametro);

    /**
     * @brief Desativa uma regra de alerta
     */
    bool desativarRegra(int regraId);

    /**
     * @brief Busca regras de um usuário
     */
    std::vector<RegraAlerta> buscarRegrasPorUsuario(int usuarioId) const;

    /**
     * @brief Retorna todas as regras ativas
     */
    std::vector<RegraAlerta> buscarRegrasAtivas() const;

    // ==================== Verificação de Alertas ====================
    
    /**
     * @brief Verifica se um usuário violou suas regras de consumo
     * 
     * @param usuarioId ID do usuário
     * @param consumoAtual Consumo atual em litros
     * @return true se alguma regra foi violada
     */
    bool verificarRegras(int usuarioId, double consumoAtual);

    /**
     * @brief Executa verificação automática para todos os usuários com regras
     * 
     * Este método deve ser chamado periodicamente (ex: a cada hora)
     * para monitorar o consumo de todos os usuários
     */
    void executarVerificacaoAutomatica();

    // ==================== Gerenciamento de Alertas Ativos ====================
    
    /**
     * @brief Retorna lista de alertas ativos
     */
    std::vector<AlertaAtivo> buscarAlertasAtivos() const;

    /**
     * @brief Busca alertas de um usuário específico
     */
    std::vector<AlertaAtivo> buscarAlertasPorUsuario(int usuarioId) const;

    /**
     * @brief Marca um alerta como resolvido
     */
    bool resolverAlerta(int alertaId);

    /**
     * @brief Limpa alertas antigos (baseado em data)
     */
    void limparAlertasAntigos(int diasRetencao = 30);

    // ==================== Configuração de Estratégias ====================
    
    /**
     * @brief Define a estratégia de notificação
     */
    void definirEstrategiaNotificacao(std::shared_ptr<NotificacaoStrategy> strategy);

    /**
     * @brief Registra uma estratégia de análise
     */
    void registrarEstrategiaAnalise(const std::string& tipo, 
                                     std::shared_ptr<EstrategiaAnaliseConsumo> strategy);

    // ==================== Métodos Auxiliares ====================
    
    /**
     * @brief Retorna estatísticas do sistema de alertas
     */
    std::string getEstatisticas() const;

private:
    /**
     * @brief Dispara um novo alerta
     */
    AlertaAtivo dispararAlerta(int usuarioId, const RegraAlerta& regra, 
                               double consumoAtual, const std::string& mensagem);

    /**
     * @brief Obtém a estratégia de análise para um tipo
     */
    std::shared_ptr<EstrategiaAnaliseConsumo> obterEstrategiaAnalise(
        const std::string& tipo) const;

    /**
     * @brief Determina a severidade do alerta baseado no consumo
     */
    AlertaAtivo::Severidade determinarSeveridade(double consumoAtual, 
                                                  const RegraAlerta& regra) const;

    /**
     * @brief Inicializa as estratégias padrão
     */
    void inicializarEstrategiasAnalise();
};

#endif // ALERTA_SERVICE_HPP
