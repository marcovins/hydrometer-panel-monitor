#include "alerta_service.hpp"
#include "../strategies/limite_diario_strategy.hpp"
#include "../strategies/media_movel_strategy.hpp"
#include "../strategies/deteccao_vazamento_strategy.hpp"
#include "../notifications/notificacao_console_log.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <ctime>

AlertaService::AlertaService() 
    : proximoIdRegra(1), proximoIdAlerta(1) {
    // Inicializa com estratégia de notificação padrão
    notificacaoStrategy = std::make_shared<NotificacaoConsoleLog>();
    
    // Registra estratégias de análise padrão
    inicializarEstrategiasAnalise();
}

// ==================== Padrão Observer ====================

void AlertaService::anexarObserver(std::shared_ptr<AlertObserver> observer) {
    observers.push_back(observer);
    std::cout << "[ALERTA_SERVICE] Observer anexado: " << observer->getNome() << std::endl;
}

void AlertaService::desanexarObserver(std::shared_ptr<AlertObserver> observer) {
    auto it = std::remove(observers.begin(), observers.end(), observer);
    if (it != observers.end()) {
        observers.erase(it, observers.end());
        std::cout << "[ALERTA_SERVICE] Observer removido: " << observer->getNome() << std::endl;
    }
}

void AlertaService::notificarObservers(const AlertaAtivo& alerta) {
    std::cout << "[ALERTA_SERVICE] Notificando " << observers.size() << " observers..." << std::endl;
    
    for (auto& observer : observers) {
        try {
            observer->atualizar(alerta);
        } catch (const std::exception& e) {
            std::cerr << "[ALERTA_SERVICE] Erro ao notificar " 
                      << observer->getNome() << ": " << e.what() << std::endl;
        }
    }
}

// ==================== Gerenciamento de Regras ====================

int AlertaService::salvarRegra(int usuarioId, const std::string& tipoEstrategia, 
                                const std::string& valorParametro) {
    // Valida se a estratégia existe
    if (estrategiasAnalise.find(tipoEstrategia) == estrategiasAnalise.end()) {
        std::cerr << "[ALERTA_SERVICE] Erro: Estratégia desconhecida: " 
                  << tipoEstrategia << std::endl;
        return -1;
    }

    RegraAlerta regra(proximoIdRegra++, usuarioId, tipoEstrategia, valorParametro);
    regrasAlerta.push_back(regra);

    std::cout << "[ALERTA_SERVICE] Regra criada: " << regra.toString() << std::endl;
    return regra.getId();
}

bool AlertaService::desativarRegra(int regraId) {
    for (auto& regra : regrasAlerta) {
        if (regra.getId() == regraId) {
            regra.setAtivo(false);
            std::cout << "[ALERTA_SERVICE] Regra " << regraId << " desativada" << std::endl;
            return true;
        }
    }
    return false;
}

std::vector<RegraAlerta> AlertaService::buscarRegrasPorUsuario(int usuarioId) const {
    std::vector<RegraAlerta> resultado;
    for (const auto& regra : regrasAlerta) {
        if (regra.getUsuarioId() == usuarioId) {
            resultado.push_back(regra);
        }
    }
    return resultado;
}

std::vector<RegraAlerta> AlertaService::buscarRegrasAtivas() const {
    std::vector<RegraAlerta> resultado;
    for (const auto& regra : regrasAlerta) {
        if (regra.isAtivo()) {
            resultado.push_back(regra);
        }
    }
    return resultado;
}

// ==================== Verificação de Alertas ====================

bool AlertaService::verificarRegras(int usuarioId, double consumoAtual) {
    std::vector<RegraAlerta> regrasUsuario = buscarRegrasPorUsuario(usuarioId);
    bool violacaoDetectada = false;

    for (const auto& regra : regrasUsuario) {
        if (!regra.isAtivo()) {
            continue;
        }

        // Obtém a estratégia de análise
        auto estrategia = obterEstrategiaAnalise(regra.getTipoEstrategia());
        if (!estrategia) {
            std::cerr << "[ALERTA_SERVICE] Estratégia não encontrada: " 
                      << regra.getTipoEstrategia() << std::endl;
            continue;
        }

        // Analisa o consumo
        bool violou = estrategia->analisar(consumoAtual, regra.getValorParametro());

        if (violou) {
            // Gera mensagem descritiva
            std::string mensagem = estrategia->gerarMensagem(consumoAtual, 
                                                             regra.getValorParametro());

            // Dispara o alerta
            AlertaAtivo alerta = dispararAlerta(usuarioId, regra, consumoAtual, mensagem);
            
            // Notifica todos os observers
            notificarObservers(alerta);
            
            violacaoDetectada = true;

            std::cout << "[ALERTA_SERVICE] ⚠️  ALERTA DISPARADO para usuário " 
                      << usuarioId << std::endl;
        }
    }

    return violacaoDetectada;
}

void AlertaService::executarVerificacaoAutomatica() {
    std::cout << "[ALERTA_SERVICE] Executando verificação automática..." << std::endl;
    
    // Em produção, aqui buscaria o consumo real do banco de dados
    // Para demonstração, simula verificação
    
    std::vector<RegraAlerta> regrasAtivas = buscarRegrasAtivas();
    std::cout << "[ALERTA_SERVICE] Verificando " << regrasAtivas.size() 
              << " regras ativas" << std::endl;

    // Agrupa por usuário
    std::map<int, bool> usuariosVerificados;
    for (const auto& regra : regrasAtivas) {
        int usuarioId = regra.getUsuarioId();
        if (usuariosVerificados.find(usuarioId) == usuariosVerificados.end()) {
            // Simula consumo (em produção, viria do banco)
            double consumoSimulado = 75.0; // Exemplo
            verificarRegras(usuarioId, consumoSimulado);
            usuariosVerificados[usuarioId] = true;
        }
    }
}

// ==================== Gerenciamento de Alertas Ativos ====================

std::vector<AlertaAtivo> AlertaService::buscarAlertasAtivos() const {
    std::vector<AlertaAtivo> resultado;
    for (const auto& alerta : alertasAtivos) {
        if (alerta.getStatus() == AlertaAtivo::ATIVO) {
            resultado.push_back(alerta);
        }
    }
    return resultado;
}

std::vector<AlertaAtivo> AlertaService::buscarAlertasPorUsuario(int usuarioId) const {
    std::vector<AlertaAtivo> resultado;
    for (const auto& alerta : alertasAtivos) {
        if (alerta.getUsuarioId() == usuarioId) {
            resultado.push_back(alerta);
        }
    }
    return resultado;
}

bool AlertaService::resolverAlerta(int alertaId) {
    for (auto& alerta : alertasAtivos) {
        if (alerta.getId() == alertaId) {
            alerta.setStatus(AlertaAtivo::RESOLVIDO);
            std::cout << "[ALERTA_SERVICE] Alerta " << alertaId << " marcado como resolvido" 
                      << std::endl;
            return true;
        }
    }
    return false;
}

void AlertaService::limparAlertasAntigos(int diasRetencao) {
    time_t agora = std::time(nullptr);
    time_t limiteRetencao = agora - (diasRetencao * 24 * 60 * 60);

    auto novoFim = std::remove_if(alertasAtivos.begin(), alertasAtivos.end(),
        [limiteRetencao](const AlertaAtivo& alerta) {
            return alerta.getDataDisparo() < limiteRetencao;
        });

    size_t removidos = std::distance(novoFim, alertasAtivos.end());
    alertasAtivos.erase(novoFim, alertasAtivos.end());

    if (removidos > 0) {
        std::cout << "[ALERTA_SERVICE] " << removidos << " alertas antigos removidos" 
                  << std::endl;
    }
}

// ==================== Configuração de Estratégias ====================

void AlertaService::definirEstrategiaNotificacao(std::shared_ptr<NotificacaoStrategy> strategy) {
    notificacaoStrategy = strategy;
    std::cout << "[ALERTA_SERVICE] Estratégia de notificação alterada para: " 
              << strategy->getNomeCanal() << std::endl;
}

void AlertaService::registrarEstrategiaAnalise(const std::string& tipo, 
                                                std::shared_ptr<EstrategiaAnaliseConsumo> strategy) {
    estrategiasAnalise[tipo] = strategy;
    std::cout << "[ALERTA_SERVICE] Estratégia de análise registrada: " << tipo << std::endl;
}

// ==================== Métodos Auxiliares ====================

std::string AlertaService::getEstatisticas() const {
    std::stringstream ss;
    ss << "=== Estatísticas do Sistema de Alertas ===\n"
       << "Regras cadastradas: " << regrasAlerta.size() << "\n"
       << "Regras ativas: " << buscarRegrasAtivas().size() << "\n"
       << "Alertas ativos: " << buscarAlertasAtivos().size() << "\n"
       << "Total de alertas: " << alertasAtivos.size() << "\n"
       << "Observers registrados: " << observers.size() << "\n"
       << "Estratégias de análise: " << estrategiasAnalise.size();
    return ss.str();
}

// ==================== Métodos Privados ====================

AlertaAtivo AlertaService::dispararAlerta(int usuarioId, const RegraAlerta& regra, 
                                          double consumoAtual, const std::string& mensagem) {
    AlertaAtivo::Severidade severidade = determinarSeveridade(consumoAtual, regra);
    
    AlertaAtivo alerta(proximoIdAlerta++, usuarioId, mensagem, 
                       consumoAtual, regra.getTipoEstrategia(), severidade);
    
    alertasAtivos.push_back(alerta);
    return alerta;
}

std::shared_ptr<EstrategiaAnaliseConsumo> AlertaService::obterEstrategiaAnalise(
    const std::string& tipo) const {
    
    auto it = estrategiasAnalise.find(tipo);
    if (it != estrategiasAnalise.end()) {
        return it->second;
    }
    return nullptr;
}

AlertaAtivo::Severidade AlertaService::determinarSeveridade(double consumoAtual, 
                                                             const RegraAlerta& regra) const {
    double parametro = regra.getValorParametroNumerico();
    
    if (parametro == 0) {
        return AlertaAtivo::MEDIA;
    }

    double percentualExcesso = ((consumoAtual - parametro) / parametro) * 100.0;

    if (percentualExcesso > 100) {
        return AlertaAtivo::CRITICA;
    } else if (percentualExcesso > 50) {
        return AlertaAtivo::ALTA;
    } else if (percentualExcesso > 20) {
        return AlertaAtivo::MEDIA;
    } else {
        return AlertaAtivo::BAIXA;
    }
}

void AlertaService::inicializarEstrategiasAnalise() {
    registrarEstrategiaAnalise("LIMITE_DIARIO", 
                               std::make_shared<LimiteDiarioStrategy>());
    registrarEstrategiaAnalise("MEDIA_MOVEL", 
                               std::make_shared<MediaMovelStrategy>());
    registrarEstrategiaAnalise("DETECCAO_VAZAMENTO", 
                               std::make_shared<DeteccaoVazamentoStrategy>());
}
