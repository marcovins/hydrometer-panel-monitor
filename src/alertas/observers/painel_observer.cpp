#include "painel_observer.hpp"
#include <iostream>

PainelObserver::PainelObserver(size_t maxAlertas) 
    : maxAlertasArmazenados(maxAlertas) {}

void PainelObserver::atualizar(const AlertaAtivo& alerta) {
    // Adiciona o alerta à lista
    alertasRecentes.push_back(alerta);

    // Remove alertas antigos se exceder o limite
    if (alertasRecentes.size() > maxAlertasArmazenados) {
        alertasRecentes.erase(alertasRecentes.begin());
    }

    // Log para debug
    std::cout << "[PAINEL] Novo alerta recebido: " << alerta.getMensagem() << std::endl;
}

std::string PainelObserver::getNome() const {
    return "PainelObserver";
}

std::vector<AlertaAtivo> PainelObserver::getAlertasRecentes() const {
    return alertasRecentes;
}

size_t PainelObserver::getQuantidadeAlertas() const {
    return alertasRecentes.size();
}

void PainelObserver::limparAlertas() {
    alertasRecentes.clear();
}

AlertaAtivo PainelObserver::getUltimoAlerta() const {
    if (alertasRecentes.empty()) {
        return AlertaAtivo();
    }
    return alertasRecentes.back();
}
