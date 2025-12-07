#ifndef NOTIFICACAO_STRATEGY_HPP
#define NOTIFICACAO_STRATEGY_HPP

#include <string>

/**
 * @brief Interface Strategy para envio de notificações
 * 
 * Define o contrato para diferentes canais de notificação.
 * Permite que o sistema troque dinamicamente entre diferentes
 * formas de enviar alertas (console, popup, email, SMS, etc).
 * 
 * Padrão de Projeto: Strategy
 */
class NotificacaoStrategy {
public:
    virtual ~NotificacaoStrategy() = default;

    /**
     * @brief Envia uma notificação
     * 
     * @param mensagem Conteúdo da mensagem a ser enviada
     * @param destinatario Identificador do destinatário (email, ID, etc)
     * @return true se o envio foi bem-sucedido, false caso contrário
     */
    virtual bool enviar(const std::string& mensagem, const std::string& destinatario) = 0;

    /**
     * @brief Retorna o nome do canal de notificação
     */
    virtual std::string getNomeCanal() const = 0;

    /**
     * @brief Verifica se o canal está disponível/configurado
     */
    virtual bool isDisponivel() const = 0;
};

#endif // NOTIFICACAO_STRATEGY_HPP
