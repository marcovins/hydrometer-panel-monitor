#include "usuario.hpp"

// Implementação da classe Usuario

Usuario::Usuario() : id(0), nome(""), email(""), tipoPerfil(TipoPerfil::LEITOR) {}

Usuario::Usuario(int id, const std::string& nome, const std::string& email, TipoPerfil tipo)
    : id(id), nome(nome), email(email), tipoPerfil(tipo) {}

int Usuario::getId() const {
    return id;
}

std::string Usuario::getNome() const {
    return nome;
}

std::string Usuario::getEmail() const {
    return email;
}

TipoPerfil Usuario::getTipoPerfil() const {
    return tipoPerfil;
}

void Usuario::setId(int id) {
    this->id = id;
}

void Usuario::setNome(const std::string& nome) {
    this->nome = nome;
}

void Usuario::setEmail(const std::string& email) {
    this->email = email;
}

void Usuario::setTipoPerfil(TipoPerfil tipo) {
    this->tipoPerfil = tipo;
}

TipoPerfil Usuario::stringToTipoPerfil(const std::string& tipo) {
    if (tipo == "ADMIN") return TipoPerfil::ADMIN;
    return TipoPerfil::LEITOR;
}

std::string Usuario::tipoPerfilToString(TipoPerfil tipo) {
    return (tipo == TipoPerfil::ADMIN) ? "ADMIN" : "LEITOR";
}

// Implementação da classe Fatura

Fatura::Fatura() : id(0), idUsuario(0), valor(0.0), dataVencimento(0), status("PENDENTE") {}

Fatura::Fatura(int id, int idUsuario, double valor, std::time_t dataVencimento, const std::string& status)
    : id(id), idUsuario(idUsuario), valor(valor), dataVencimento(dataVencimento), status(status) {}

int Fatura::getId() const {
    return id;
}

int Fatura::getIdUsuario() const {
    return idUsuario;
}

double Fatura::getValor() const {
    return valor;
}

std::time_t Fatura::getDataVencimento() const {
    return dataVencimento;
}

std::string Fatura::getStatus() const {
    return status;
}

void Fatura::setId(int id) {
    this->id = id;
}

void Fatura::setIdUsuario(int idUsuario) {
    this->idUsuario = idUsuario;
}

void Fatura::setValor(double valor) {
    this->valor = valor;
}

void Fatura::setDataVencimento(std::time_t data) {
    this->dataVencimento = data;
}

void Fatura::setStatus(const std::string& status) {
    this->status = status;
}
