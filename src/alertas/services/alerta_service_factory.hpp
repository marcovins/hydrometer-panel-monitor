#ifndef ALERTA_SERVICE_FACTORY_HPP
#define ALERTA_SERVICE_FACTORY_HPP

#include "alerta_service.hpp"
#include "../observers/painel_observer.hpp"
#include "../observers/logger_observer.hpp"
#include "../observers/notificacao_observer.hpp"
#include "../notifications/notificacao_console_log.hpp"
#include "../notifications/notificacao_email.hpp"
#include "../notifications/notificacao_windows_popup.hpp"
#include <memory>
#include <string>

/**
 * @brief Factory para criação e configuração do AlertaService
 * 
 * Padrão de Projeto: Factory Method
 * 
 * Facilita a criação de instâncias do AlertaService com diferentes
 * configurações pré-definidas, encapsulando a complexidade de
 * inicialização e configuração dos observers e strategies.
 */
class AlertaServiceFactory {
public:
    /**
     * @brief Cria um AlertaService com configuração padrão
     * 
     * Configuração:
     * - Notificação: Console Log
     * - Observers: Painel, Logger e Notificação
     * - Estratégias: Todas as estratégias de análise padrão
     */
    static std::shared_ptr<AlertaService> criarPadrao();

    /**
     * @brief Cria um AlertaService para ambiente de teste
     * 
     * Configuração:
     * - Notificação: Console Log (simplificado)
     * - Observers: Apenas Painel e Logger
     * - Ideal para testes unitários e integração
     */
    static std::shared_ptr<AlertaService> criarParaTeste();

    /**
     * @brief Cria um AlertaService para produção
     * 
     * Configuração:
     * - Notificação: Email (requer configuração SMTP)
     * - Observers: Painel, Logger e Notificação Email
     * - Logging completo e persistência
     */
    static std::shared_ptr<AlertaService> criarParaProducao(
        const std::string& smtpServidor,
        int smtpPorta,
        const std::string& emailRemetente);

    /**
     * @brief Cria um AlertaService com observers customizados
     * 
     * Permite controle total sobre os observers anexados
     */
    static std::shared_ptr<AlertaService> criarComObservers(
        std::shared_ptr<NotificacaoStrategy> notificacaoStrategy,
        const std::vector<std::shared_ptr<AlertObserver>>& observers);

    /**
     * @brief Cria um AlertaService minimalista
     * 
     * Sem observers, apenas para verificação de regras
     * Útil para casos de uso específicos
     */
    static std::shared_ptr<AlertaService> criarMinimalista();

private:
    /**
     * @brief Configura os observers padrão
     */
    static void configurarObserversPadrao(std::shared_ptr<AlertaService> service,
                                          std::shared_ptr<NotificacaoStrategy> notifStrategy);
};

#endif // ALERTA_SERVICE_FACTORY_HPP
