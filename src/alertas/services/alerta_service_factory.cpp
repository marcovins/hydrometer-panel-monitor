#include "alerta_service_factory.hpp"
#include <iostream>

std::shared_ptr<AlertaService> AlertaServiceFactory::criarPadrao() {
    auto service = std::make_shared<AlertaService>();
    auto notifStrategy = std::make_shared<NotificacaoConsoleLog>();
    
    service->definirEstrategiaNotificacao(notifStrategy);
    configurarObserversPadrao(service, notifStrategy);

    std::cout << "[FACTORY] AlertaService criado com configuração padrão" << std::endl;
    return service;
}

std::shared_ptr<AlertaService> AlertaServiceFactory::criarParaTeste() {
    auto service = std::make_shared<AlertaService>();
    auto notifStrategy = std::make_shared<NotificacaoConsoleLog>();
    
    service->definirEstrategiaNotificacao(notifStrategy);

    // Anexa apenas observers essenciais para teste
    auto painelObserver = std::make_shared<PainelObserver>(50); // Limite reduzido
    auto loggerObserver = std::make_shared<LoggerObserver>();
    
    service->anexarObserver(painelObserver);
    service->anexarObserver(loggerObserver);

    std::cout << "[FACTORY] AlertaService criado para testes" << std::endl;
    return service;
}

std::shared_ptr<AlertaService> AlertaServiceFactory::criarParaProducao(
    const std::string& smtpServidor,
    int smtpPorta,
    const std::string& emailRemetente) {
    
    auto service = std::make_shared<AlertaService>();
    
    // Configura estratégia de email
    auto emailStrategy = std::make_shared<NotificacaoEmail>(
        smtpServidor, smtpPorta, emailRemetente);
    
    service->definirEstrategiaNotificacao(emailStrategy);

    // Anexa todos os observers incluindo email
    auto painelObserver = std::make_shared<PainelObserver>();
    auto loggerObserver = std::make_shared<LoggerObserver>();
    auto notifObserver = std::make_shared<NotificacaoObserver>(emailStrategy);
    
    service->anexarObserver(painelObserver);
    service->anexarObserver(loggerObserver);
    service->anexarObserver(notifObserver);

    std::cout << "[FACTORY] AlertaService criado para produção com email" << std::endl;
    return service;
}

std::shared_ptr<AlertaService> AlertaServiceFactory::criarComObservers(
    std::shared_ptr<NotificacaoStrategy> notificacaoStrategy,
    const std::vector<std::shared_ptr<AlertObserver>>& observers) {
    
    auto service = std::make_shared<AlertaService>();
    service->definirEstrategiaNotificacao(notificacaoStrategy);

    for (auto& observer : observers) {
        service->anexarObserver(observer);
    }

    std::cout << "[FACTORY] AlertaService criado com " << observers.size() 
              << " observers customizados" << std::endl;
    return service;
}

std::shared_ptr<AlertaService> AlertaServiceFactory::criarMinimalista() {
    auto service = std::make_shared<AlertaService>();
    
    // Apenas configuração básica, sem observers
    std::cout << "[FACTORY] AlertaService criado (minimalista)" << std::endl;
    return service;
}

void AlertaServiceFactory::configurarObserversPadrao(
    std::shared_ptr<AlertaService> service,
    std::shared_ptr<NotificacaoStrategy> notifStrategy) {
    
    // Cria e anexa observer do painel
    auto painelObserver = std::make_shared<PainelObserver>();
    service->anexarObserver(painelObserver);

    // Cria e anexa observer de log
    auto loggerObserver = std::make_shared<LoggerObserver>();
    service->anexarObserver(loggerObserver);

    // Cria e anexa observer de notificação
    auto notifObserver = std::make_shared<NotificacaoObserver>(notifStrategy);
    service->anexarObserver(notifObserver);
}
