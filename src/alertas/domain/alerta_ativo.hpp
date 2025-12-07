#ifndef ALERTA_ATIVO_HPP
#define ALERTA_ATIVO_HPP

#include <string>
#include <ctime>

/**
 * @brief Entidade que representa um alerta que foi disparado pelo sistema
 * 
 * Esta classe armazena informações sobre alertas que foram acionados quando
 * uma regra de consumo foi violada.
 */
class AlertaAtivo {
public:
    // Enumeração para status do alerta
    enum Status {
        ATIVO,
        RESOLVIDO,
        IGNORADO
    };

    // Enumeração para severidade do alerta
    enum Severidade {
        BAIXA,
        MEDIA,
        ALTA,
        CRITICA
    };

private:
    int id;
    int usuarioId;
    std::string mensagem;
    time_t dataDisparo;
    Status status;
    Severidade severidade;
    double valorConsumo;
    std::string tipoRegra;

public:
    // Construtores
    AlertaAtivo();
    AlertaAtivo(int id, int usuarioId, const std::string& mensagem, 
                double valorConsumo, const std::string& tipoRegra, 
                Severidade severidade = MEDIA);

    // Getters
    int getId() const;
    int getUsuarioId() const;
    std::string getMensagem() const;
    time_t getDataDisparo() const;
    Status getStatus() const;
    Severidade getSeveridade() const;
    double getValorConsumo() const;
    std::string getTipoRegra() const;

    // Setters
    void setId(int id);
    void setUsuarioId(int usuarioId);
    void setMensagem(const std::string& mensagem);
    void setDataDisparo(time_t dataDisparo);
    void setStatus(Status status);
    void setSeveridade(Severidade severidade);
    void setValorConsumo(double valorConsumo);
    void setTipoRegra(const std::string& tipoRegra);

    // Métodos auxiliares
    std::string toString() const;
    std::string getStatusString() const;
    std::string getSeveridadeString() const;
    std::string getDataDisparoFormatada() const;
};

#endif // ALERTA_ATIVO_HPP
