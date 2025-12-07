#include "regra_alerta.hpp"
#include <sstream>
#include <iomanip>

// Construtor padrão
RegraAlerta::RegraAlerta() 
    : id(0), usuarioId(0), tipoEstrategia(""), valorParametro(""), 
      ativo(true), dataCriacao(std::time(nullptr)) {}

// Construtor parametrizado
RegraAlerta::RegraAlerta(int id, int usuarioId, const std::string& tipoEstrategia, 
                         const std::string& valorParametro, bool ativo)
    : id(id), usuarioId(usuarioId), tipoEstrategia(tipoEstrategia), 
      valorParametro(valorParametro), ativo(ativo), dataCriacao(std::time(nullptr)) {}

// Getters
int RegraAlerta::getId() const {
    return id;
}

int RegraAlerta::getUsuarioId() const {
    return usuarioId;
}

std::string RegraAlerta::getTipoEstrategia() const {
    return tipoEstrategia;
}

std::string RegraAlerta::getValorParametro() const {
    return valorParametro;
}

bool RegraAlerta::isAtivo() const {
    return ativo;
}

time_t RegraAlerta::getDataCriacao() const {
    return dataCriacao;
}

// Setters
void RegraAlerta::setId(int id) {
    this->id = id;
}

void RegraAlerta::setUsuarioId(int usuarioId) {
    this->usuarioId = usuarioId;
}

void RegraAlerta::setTipoEstrategia(const std::string& tipoEstrategia) {
    this->tipoEstrategia = tipoEstrategia;
}

void RegraAlerta::setValorParametro(const std::string& valorParametro) {
    this->valorParametro = valorParametro;
}

void RegraAlerta::setAtivo(bool ativo) {
    this->ativo = ativo;
}

void RegraAlerta::setDataCriacao(time_t dataCriacao) {
    this->dataCriacao = dataCriacao;
}

// Métodos auxiliares
std::string RegraAlerta::toString() const {
    std::stringstream ss;
    ss << "RegraAlerta[id=" << id 
       << ", usuarioId=" << usuarioId 
       << ", tipo=" << tipoEstrategia 
       << ", parametro=" << valorParametro 
       << ", ativo=" << (ativo ? "true" : "false") << "]";
    return ss.str();
}

double RegraAlerta::getValorParametroNumerico() const {
    try {
        return std::stod(valorParametro);
    } catch (...) {
        return 0.0;
    }
}
