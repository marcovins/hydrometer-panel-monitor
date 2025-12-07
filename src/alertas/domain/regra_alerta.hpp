#ifndef REGRA_ALERTA_HPP
#define REGRA_ALERTA_HPP

#include <string>
#include <ctime>

/**
 * @brief Entidade que representa uma regra de alerta configurada para um usuário
 * 
 * Esta classe encapsula os dados de uma regra que será verificada periodicamente
 * pelo sistema de alertas para detectar violações de consumo.
 */
class RegraAlerta {
private:
    int id;
    int usuarioId;
    std::string tipoEstrategia;  // "LIMITE_DIARIO", "MEDIA_MOVEL", "DETECCAO_VAZAMENTO"
    std::string valorParametro;  // Valor de ajuste (ex: "70" para limite, "24h" para vazamento)
    bool ativo;
    time_t dataCriacao;

public:
    // Construtores
    RegraAlerta();
    RegraAlerta(int id, int usuarioId, const std::string& tipoEstrategia, 
                const std::string& valorParametro, bool ativo = true);

    // Getters
    int getId() const;
    int getUsuarioId() const;
    std::string getTipoEstrategia() const;
    std::string getValorParametro() const;
    bool isAtivo() const;
    time_t getDataCriacao() const;

    // Setters
    void setId(int id);
    void setUsuarioId(int usuarioId);
    void setTipoEstrategia(const std::string& tipoEstrategia);
    void setValorParametro(const std::string& valorParametro);
    void setAtivo(bool ativo);
    void setDataCriacao(time_t dataCriacao);

    // Métodos auxiliares
    std::string toString() const;
    double getValorParametroNumerico() const;
};

#endif // REGRA_ALERTA_HPP
