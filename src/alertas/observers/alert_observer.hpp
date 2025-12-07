#ifndef ALERT_OBSERVER_HPP
#define ALERT_OBSERVER_HPP

#include "../domain/alerta_ativo.hpp"

/**
 * @brief Interface Observer para o padrão Observer
 * 
 * Define o contrato para objetos que desejam ser notificados
 * quando um alerta é disparado no sistema.
 * 
 * Padrão de Projeto: Observer
 */
class AlertObserver {
public:
    virtual ~AlertObserver() = default;

    /**
     * @brief Método chamado quando um alerta é disparado
     * 
     * @param alerta O alerta que foi ativado
     */
    virtual void atualizar(const AlertaAtivo& alerta) = 0;

    /**
     * @brief Retorna o nome do observer (para debug/log)
     */
    virtual std::string getNome() const = 0;
};

#endif // ALERT_OBSERVER_HPP
