#ifndef LOGGER_OBSERVER_HPP
#define LOGGER_OBSERVER_HPP

#include "alert_observer.hpp"
#include "../../utils/logger.hpp"
#include <memory>

/**
 * @brief Observer que registra alertas no sistema de log
 * 
 * Este observer garante que todos os alertas disparados sejam
 * persistidos no arquivo de log do sistema para auditoria.
 */
class LoggerObserver : public AlertObserver {
public:
    LoggerObserver();
    explicit LoggerObserver(std::shared_ptr<Logger> logger);

    void atualizar(const AlertaAtivo& alerta) override;
    std::string getNome() const override;

private:
    std::string formatarMensagemLog(const AlertaAtivo& alerta) const;
};

#endif // LOGGER_OBSERVER_HPP
