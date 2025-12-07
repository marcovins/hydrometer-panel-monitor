#include "alerta_ativo.hpp"
#include <sstream>
#include <iomanip>

// Construtor padrão
AlertaAtivo::AlertaAtivo() 
    : id(0), usuarioId(0), mensagem(""), dataDisparo(std::time(nullptr)), 
      status(ATIVO), severidade(MEDIA), valorConsumo(0.0), tipoRegra("") {}

// Construtor parametrizado
AlertaAtivo::AlertaAtivo(int id, int usuarioId, const std::string& mensagem, 
                         double valorConsumo, const std::string& tipoRegra, 
                         Severidade severidade)
    : id(id), usuarioId(usuarioId), mensagem(mensagem), 
      dataDisparo(std::time(nullptr)), status(ATIVO), 
      severidade(severidade), valorConsumo(valorConsumo), tipoRegra(tipoRegra) {}

// Getters
int AlertaAtivo::getId() const {
    return id;
}

int AlertaAtivo::getUsuarioId() const {
    return usuarioId;
}

std::string AlertaAtivo::getMensagem() const {
    return mensagem;
}

time_t AlertaAtivo::getDataDisparo() const {
    return dataDisparo;
}

AlertaAtivo::Status AlertaAtivo::getStatus() const {
    return status;
}

AlertaAtivo::Severidade AlertaAtivo::getSeveridade() const {
    return severidade;
}

double AlertaAtivo::getValorConsumo() const {
    return valorConsumo;
}

std::string AlertaAtivo::getTipoRegra() const {
    return tipoRegra;
}

// Setters
void AlertaAtivo::setId(int id) {
    this->id = id;
}

void AlertaAtivo::setUsuarioId(int usuarioId) {
    this->usuarioId = usuarioId;
}

void AlertaAtivo::setMensagem(const std::string& mensagem) {
    this->mensagem = mensagem;
}

void AlertaAtivo::setDataDisparo(time_t dataDisparo) {
    this->dataDisparo = dataDisparo;
}

void AlertaAtivo::setStatus(Status status) {
    this->status = status;
}

void AlertaAtivo::setSeveridade(Severidade severidade) {
    this->severidade = severidade;
}

void AlertaAtivo::setValorConsumo(double valorConsumo) {
    this->valorConsumo = valorConsumo;
}

void AlertaAtivo::setTipoRegra(const std::string& tipoRegra) {
    this->tipoRegra = tipoRegra;
}

// Métodos auxiliares
std::string AlertaAtivo::getStatusString() const {
    switch (status) {
        case ATIVO: return "ATIVO";
        case RESOLVIDO: return "RESOLVIDO";
        case IGNORADO: return "IGNORADO";
        default: return "DESCONHECIDO";
    }
}

std::string AlertaAtivo::getSeveridadeString() const {
    switch (severidade) {
        case BAIXA: return "BAIXA";
        case MEDIA: return "MEDIA";
        case ALTA: return "ALTA";
        case CRITICA: return "CRITICA";
        default: return "DESCONHECIDA";
    }
}

std::string AlertaAtivo::getDataDisparoFormatada() const {
    struct tm* timeinfo = localtime(&dataDisparo);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

std::string AlertaAtivo::toString() const {
    std::stringstream ss;
    ss << "AlertaAtivo[id=" << id 
       << ", usuarioId=" << usuarioId 
       << ", mensagem='" << mensagem << "'"
       << ", consumo=" << valorConsumo << "L"
       << ", tipo=" << tipoRegra
       << ", severidade=" << getSeveridadeString()
       << ", status=" << getStatusString()
       << ", data=" << getDataDisparoFormatada() << "]";
    return ss.str();
}
