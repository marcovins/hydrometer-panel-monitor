#ifndef PAINEL_OBSERVER_HPP
#define PAINEL_OBSERVER_HPP

#include "alert_observer.hpp"
#include <vector>
#include <memory>

/**
 * @brief Observer que atualiza o painel/interface com novos alertas
 * 
 * Este observer mantém uma lista de alertas ativos que podem ser
 * consultados pela interface gráfica do sistema.
 */
class PainelObserver : public AlertObserver {
private:
    std::vector<AlertaAtivo> alertasRecentes;
    size_t maxAlertasArmazenados;

public:
    PainelObserver(size_t maxAlertas = 100);

    void atualizar(const AlertaAtivo& alerta) override;
    std::string getNome() const override;

    // Métodos específicos do painel
    std::vector<AlertaAtivo> getAlertasRecentes() const;
    size_t getQuantidadeAlertas() const;
    void limparAlertas();
    AlertaAtivo getUltimoAlerta() const;
};

#endif // PAINEL_OBSERVER_HPP
