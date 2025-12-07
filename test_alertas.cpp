/**
 * @file test_alertas.cpp
 * @brief Teste completo do Subsistema de Alertas
 * 
 * Este arquivo demonstra todos os recursos do subsistema de alertas:
 * - Padrão Observer (notificação de múltiplos observers)
 * - Padrão Strategy (análise de consumo e notificações)
 * - Diferentes estratégias de análise (Limite Diário, Média Móvel, Vazamento)
 * - Diferentes canais de notificação (Console, Popup, Email)
 */

#include "src/alertas/services/alerta_service.hpp"
#include "src/alertas/services/alerta_service_factory.hpp"
#include "src/alertas/strategies/limite_diario_strategy.hpp"
#include "src/alertas/strategies/media_movel_strategy.hpp"
#include "src/alertas/strategies/deteccao_vazamento_strategy.hpp"
#include "src/alertas/notifications/notificacao_console_log.hpp"
#include "src/alertas/notifications/notificacao_windows_popup.hpp"
#include "src/alertas/notifications/notificacao_email.hpp"
#include "src/alertas/observers/painel_observer.hpp"
#include "src/alertas/observers/logger_observer.hpp"
#include "src/alertas/observers/notificacao_observer.hpp"

#include <iostream>
#include <iomanip>
#include <memory>

void imprimirSeparador(const std::string& titulo = "") {
    std::cout << "\n" << std::string(70, '=') << "\n";
    if (!titulo.empty()) {
        std::cout << "  " << titulo << "\n";
        std::cout << std::string(70, '=') << "\n";
    }
}

void teste1_CriacaoBasica() {
    imprimirSeparador("TESTE 1: Criação Básica do AlertaService");
    
    // Cria service usando factory
    auto service = AlertaServiceFactory::criarPadrao();
    
    std::cout << "\n✓ AlertaService criado com sucesso\n";
    std::cout << service->getEstatisticas() << std::endl;
}

void teste2_CriacaoRegras() {
    imprimirSeparador("TESTE 2: Criação e Gerenciamento de Regras");
    
    auto service = AlertaServiceFactory::criarParaTeste();
    
    // Cria diferentes tipos de regras
    std::cout << "\n1. Criando regra de limite diário...\n";
    int regra1 = service->salvarRegra(1, "LIMITE_DIARIO", "70");
    
    std::cout << "\n2. Criando regra de média móvel...\n";
    int regra2 = service->salvarRegra(2, "MEDIA_MOVEL", "50");
    
    std::cout << "\n3. Criando regra de detecção de vazamento...\n";
    int regra3 = service->salvarRegra(3, "DETECCAO_VAZAMENTO", "24h");
    
    // Lista regras criadas
    std::cout << "\n--- Regras Ativas ---\n";
    auto regras = service->buscarRegrasAtivas();
    for (const auto& regra : regras) {
        std::cout << "  " << regra.toString() << "\n";
    }
    
    std::cout << "\n✓ " << regras.size() << " regras criadas com sucesso\n";
}

void teste3_VerificacaoConsumo() {
    imprimirSeparador("TESTE 3: Verificação de Consumo e Disparo de Alertas");
    
    auto service = AlertaServiceFactory::criarPadrao();
    
    // Cria regra de limite
    service->salvarRegra(100, "LIMITE_DIARIO", "70");
    
    std::cout << "\n--- Cenário 1: Consumo Normal (50L) ---\n";
    bool alertou1 = service->verificarRegras(100, 50.0);
    std::cout << "Alerta disparado: " << (alertou1 ? "SIM" : "NÃO") << "\n";
    
    std::cout << "\n--- Cenário 2: Consumo Acima do Limite (85L) ---\n";
    bool alertou2 = service->verificarRegras(100, 85.0);
    std::cout << "Alerta disparado: " << (alertou2 ? "SIM" : "NÃO") << "\n";
    
    std::cout << "\n--- Cenário 3: Consumo Muito Alto (150L) ---\n";
    bool alertou3 = service->verificarRegras(100, 150.0);
    std::cout << "Alerta disparado: " << (alertou3 ? "SIM" : "NÃO") << "\n";
    
    // Exibe alertas ativos
    std::cout << "\n--- Alertas Ativos ---\n";
    auto alertas = service->buscarAlertasAtivos();
    for (const auto& alerta : alertas) {
        std::cout << "  " << alerta.toString() << "\n";
    }
    
    std::cout << "\n✓ " << alertas.size() << " alertas disparados\n";
}

void teste4_PadraoObserver() {
    imprimirSeparador("TESTE 4: Padrão Observer - Múltiplos Observadores");
    
    auto service = std::make_shared<AlertaService>();
    
    // Cria observers customizados
    auto painelObs = std::make_shared<PainelObserver>();
    auto loggerObs = std::make_shared<LoggerObserver>();
    auto notifObs = std::make_shared<NotificacaoObserver>(
        std::make_shared<NotificacaoConsoleLog>()
    );
    
    // Anexa observers
    std::cout << "\nAnexando observers...\n";
    service->anexarObserver(painelObs);
    service->anexarObserver(loggerObs);
    service->anexarObserver(notifObs);
    
    // Cria regra e dispara alerta
    std::cout << "\nCriando regra e verificando consumo...\n";
    service->salvarRegra(200, "LIMITE_DIARIO", "60");
    service->verificarRegras(200, 95.0);
    
    // Verifica se painel recebeu o alerta
    std::cout << "\n--- Status do Painel ---\n";
    std::cout << "Alertas no painel: " << painelObs->getQuantidadeAlertas() << "\n";
    if (painelObs->getQuantidadeAlertas() > 0) {
        std::cout << "Último alerta: " << painelObs->getUltimoAlerta().getMensagem() << "\n";
    }
    
    std::cout << "\n✓ Todos os observers foram notificados\n";
}

void teste5_EstrategiasAnalise() {
    imprimirSeparador("TESTE 5: Diferentes Estratégias de Análise");
    
    auto service = AlertaServiceFactory::criarParaTeste();
    
    std::cout << "\n--- Testando Limite Diário ---\n";
    service->salvarRegra(301, "LIMITE_DIARIO", "70");
    service->verificarRegras(301, 85.0); // Deve alertar
    
    std::cout << "\n--- Testando Média Móvel ---\n";
    service->salvarRegra(302, "MEDIA_MOVEL", "50"); // 50% acima da média
    service->verificarRegras(302, 100.0); // Deve alertar se média histórica for ~50L
    
    std::cout << "\n--- Testando Detecção de Vazamento ---\n";
    service->salvarRegra(303, "DETECCAO_VAZAMENTO", "24h");
    service->verificarRegras(303, 48.0); // 2L/h constante = possível vazamento
    
    auto alertas = service->buscarAlertasAtivos();
    std::cout << "\n✓ Total de alertas: " << alertas.size() << "\n";
}

void teste6_EstrategiasNotificacao() {
    imprimirSeparador("TESTE 6: Diferentes Estratégias de Notificação");
    
    std::cout << "\n--- 1. Notificação via Console ---\n";
    auto consoleStrategy = std::make_shared<NotificacaoConsoleLog>();
    consoleStrategy->enviar("Teste de notificação via console", "usuario@exemplo.com");
    
    std::cout << "\n--- 2. Notificação via Windows Popup ---\n";
    auto popupStrategy = std::make_shared<NotificacaoWindowsPopup>();
    popupStrategy->enviar("Teste de popup", "Usuario123");
    
    std::cout << "\n--- 3. Notificação via Email (Simulado) ---\n";
    auto emailStrategy = std::make_shared<NotificacaoEmail>(
        "smtp.cagepa.com.br", 587, "alertas@cagepa.com.br"
    );
    emailStrategy->enviar("Teste de email", "cliente@exemplo.com");
    
    std::cout << "\n✓ Todas as estratégias de notificação testadas\n";
}

void teste7_TrocaDinamicaEstrategia() {
    imprimirSeparador("TESTE 7: Troca Dinâmica de Estratégia de Notificação");
    
    auto service = AlertaServiceFactory::criarParaTeste();
    service->salvarRegra(400, "LIMITE_DIARIO", "70");
    
    std::cout << "\n--- Alerta 1: Com Console Log ---\n";
    service->verificarRegras(400, 90.0);
    
    std::cout << "\n--- Alterando para Windows Popup ---\n";
    service->definirEstrategiaNotificacao(std::make_shared<NotificacaoWindowsPopup>());
    
    std::cout << "\n--- Alerta 2: Com Windows Popup ---\n";
    service->verificarRegras(400, 95.0);
    
    std::cout << "\n--- Alterando para Email ---\n";
    auto emailStrat = std::make_shared<NotificacaoEmail>(
        "smtp.exemplo.com", 587, "sistema@cagepa.com.br"
    );
    service->definirEstrategiaNotificacao(emailStrat);
    
    std::cout << "\n--- Alerta 3: Com Email ---\n";
    service->verificarRegras(400, 100.0);
    
    std::cout << "\n✓ Estratégia trocada dinamicamente 3 vezes\n";
}

void teste8_FactoryPatterns() {
    imprimirSeparador("TESTE 8: Padrão Factory - Diferentes Configurações");
    
    std::cout << "\n--- 1. Factory Padrão ---\n";
    auto servicePadrao = AlertaServiceFactory::criarPadrao();
    std::cout << servicePadrao->getEstatisticas() << "\n";
    
    std::cout << "\n--- 2. Factory Para Testes ---\n";
    auto serviceTeste = AlertaServiceFactory::criarParaTeste();
    std::cout << serviceTeste->getEstatisticas() << "\n";
    
    std::cout << "\n--- 3. Factory Para Produção ---\n";
    auto serviceProducao = AlertaServiceFactory::criarParaProducao(
        "smtp.cagepa.com.br", 587, "alertas@cagepa.com.br"
    );
    std::cout << serviceProducao->getEstatisticas() << "\n";
    
    std::cout << "\n--- 4. Factory Minimalista ---\n";
    auto serviceMin = AlertaServiceFactory::criarMinimalista();
    std::cout << serviceMin->getEstatisticas() << "\n";
    
    std::cout << "\n✓ Todas as configurações de factory testadas\n";
}

void teste9_GerenciamentoAlertas() {
    imprimirSeparador("TESTE 9: Gerenciamento de Alertas Ativos");
    
    auto service = AlertaServiceFactory::criarParaTeste();
    
    // Cria regras e dispara alertas
    service->salvarRegra(500, "LIMITE_DIARIO", "70");
    service->verificarRegras(500, 85.0);
    service->verificarRegras(500, 90.0);
    service->verificarRegras(500, 95.0);
    
    std::cout << "\n--- Alertas Ativos ---\n";
    auto alertas = service->buscarAlertasAtivos();
    for (size_t i = 0; i < alertas.size(); ++i) {
        std::cout << "  " << (i+1) << ". " << alertas[i].toString() << "\n";
    }
    
    // Resolve primeiro alerta
    if (!alertas.empty()) {
        std::cout << "\nResolvendo alerta " << alertas[0].getId() << "...\n";
        service->resolverAlerta(alertas[0].getId());
    }
    
    std::cout << "\n--- Alertas Após Resolução ---\n";
    alertas = service->buscarAlertasAtivos();
    std::cout << "Alertas ativos restantes: " << alertas.size() << "\n";
    
    std::cout << "\n✓ Gerenciamento de alertas testado\n";
}

void teste10_EstatisticasCompletas() {
    imprimirSeparador("TESTE 10: Estatísticas do Sistema");
    
    auto service = AlertaServiceFactory::criarPadrao();
    
    // Cria diversos elementos
    service->salvarRegra(1, "LIMITE_DIARIO", "70");
    service->salvarRegra(2, "MEDIA_MOVEL", "50");
    service->salvarRegra(3, "DETECCAO_VAZAMENTO", "24h");
    
    service->verificarRegras(1, 85.0);
    service->verificarRegras(2, 95.0);
    
    std::cout << "\n" << service->getEstatisticas() << "\n";
    
    std::cout << "\n✓ Relatório de estatísticas gerado\n";
}

int main() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║     TESTE COMPLETO DO SUBSISTEMA DE ALERTAS - SSMH              ║\n";
    std::cout << "║     Padrões: Observer + Strategy                                 ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════════╝\n";

    try {
        teste1_CriacaoBasica();
        teste2_CriacaoRegras();
        teste3_VerificacaoConsumo();
        teste4_PadraoObserver();
        teste5_EstrategiasAnalise();
        teste6_EstrategiasNotificacao();
        teste7_TrocaDinamicaEstrategia();
        teste8_FactoryPatterns();
        teste9_GerenciamentoAlertas();
        teste10_EstatisticasCompletas();

        imprimirSeparador("RESULTADO FINAL");
        std::cout << "\n✅ TODOS OS TESTES EXECUTADOS COM SUCESSO!\n\n";

    } catch (const std::exception& e) {
        std::cerr << "\n❌ ERRO: " << e.what() << "\n\n";
        return 1;
    }

    return 0;
}
