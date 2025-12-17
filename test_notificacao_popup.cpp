#include "src/alertas/notifications/notificacao_windows_popup.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== TESTE DE NOTIFICAÇÃO POPUP ===" << std::endl;
    std::cout << std::endl;

    // Cria instância da notificação
    NotificacaoWindowsPopup notificacao;

    // Verifica disponibilidade
    std::cout << "1. Verificando disponibilidade..." << std::endl;
    if (notificacao.isDisponivel()) {
        std::cout << "   ✓ Notificação disponível" << std::endl;
    } else {
        std::cout << "   ✗ Notificação não disponível" << std::endl;
    }
    std::cout << "   Canal: " << notificacao.getNomeCanal() << std::endl;
    std::cout << std::endl;

    // Teste 1: Notificação simples
    std::cout << "2. Enviando notificação de teste simples..." << std::endl;
    bool resultado1 = notificacao.enviar(
        "Consumo acima do normal detectado!",
        "usuario_teste"
    );
    std::cout << "   Resultado: " << (resultado1 ? "✓ Sucesso" : "✗ Falha") << std::endl;
    std::cout << std::endl;

    // Aguarda um pouco
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Teste 2: Notificação com mais detalhes
    std::cout << "3. Enviando notificação com detalhes..." << std::endl;
    bool resultado2 = notificacao.enviar(
        "Consumo: 450 litros (Limite: 350 litros). Vazamento detectado no hidrômetro H001.",
        "joao.silva@email.com"
    );
    std::cout << "   Resultado: " << (resultado2 ? "✓ Sucesso" : "✗ Falha") << std::endl;
    std::cout << std::endl;

    // Aguarda um pouco
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Teste 3: Notificação com caracteres especiais
    std::cout << "4. Enviando notificação com aspas e caracteres especiais..." << std::endl;
    bool resultado3 = notificacao.enviar(
        "Atenção: Consumo \"crítico\" detectado! Valor: 500L (150% acima da média)",
        "maria.santos"
    );
    std::cout << "   Resultado: " << (resultado3 ? "✓ Sucesso" : "✗ Falha") << std::endl;
    std::cout << std::endl;

    // Resumo
    std::cout << "=== RESUMO DOS TESTES ===" << std::endl;
    int sucessos = (resultado1 ? 1 : 0) + (resultado2 ? 1 : 0) + (resultado3 ? 1 : 0);
    std::cout << "Testes realizados: 3" << std::endl;
    std::cout << "Sucessos: " << sucessos << std::endl;
    std::cout << "Falhas: " << (3 - sucessos) << std::endl;
    std::cout << std::endl;

    if (sucessos == 3) {
        std::cout << "✓ Todos os testes passaram!" << std::endl;
    } else {
        std::cout << "⚠ Alguns testes falharam. Verifique a configuração." << std::endl;
    }

    return 0;
}
