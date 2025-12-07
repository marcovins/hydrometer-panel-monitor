#include "armazenamento_volatil.hpp"
#include <algorithm>

ArmazenamentoVolatil::ArmazenamentoVolatil() : nextId(1) {}

void ArmazenamentoVolatil::salvar(const Usuario& usuario) {
    usuarios[usuario.getId()] = usuario;
    if (usuario.getId() >= nextId) {
        nextId = usuario.getId() + 1;
    }
}

Usuario ArmazenamentoVolatil::buscar(int id) {
    auto it = usuarios.find(id);
    if (it == usuarios.end()) {
        throw std::runtime_error("Usuario nao encontrado: ID " + std::to_string(id));
    }
    return it->second;
}

void ArmazenamentoVolatil::atualizar(int id, const std::map<std::string, std::string>& dados) {
    auto it = usuarios.find(id);
    if (it == usuarios.end()) {
        throw std::runtime_error("Usuario nao encontrado para atualizacao: ID " + std::to_string(id));
    }
    
    Usuario& usuario = it->second;
    
    // Atualiza campos conforme os dados fornecidos
    if (dados.find("nome") != dados.end()) {
        usuario.setNome(dados.at("nome"));
    }
    if (dados.find("email") != dados.end()) {
        usuario.setEmail(dados.at("email"));
    }
    if (dados.find("tipoPerfil") != dados.end()) {
        usuario.setTipoPerfil(Usuario::stringToTipoPerfil(dados.at("tipoPerfil")));
    }
}

void ArmazenamentoVolatil::deletar(int id) {
    auto it = usuarios.find(id);
    if (it == usuarios.end()) {
        throw std::runtime_error("Usuario nao encontrado para delecao: ID " + std::to_string(id));
    }
    
    // Remove usuário e seus dados associados
    usuarios.erase(id);
    vinculos.erase(id);
    faturas.erase(id);
}

void ArmazenamentoVolatil::associarHidrometro(int idUser, const std::string& idSha) {
    if (!usuarioExiste(idUser)) {
        throw std::runtime_error("Usuario nao encontrado: ID " + std::to_string(idUser));
    }
    
    // Verifica se já não está associado
    auto& lista = vinculos[idUser];
    if (std::find(lista.begin(), lista.end(), idSha) == lista.end()) {
        lista.push_back(idSha);
    }
}

void ArmazenamentoVolatil::desassociarHidrometro(int idUser, const std::string& idSha) {
    if (!usuarioExiste(idUser)) {
        throw std::runtime_error("Usuario nao encontrado: ID " + std::to_string(idUser));
    }
    
    auto& lista = vinculos[idUser];
    lista.erase(std::remove(lista.begin(), lista.end(), idSha), lista.end());
}

std::vector<std::string> ArmazenamentoVolatil::listarHidrometros(int idUser) {
    auto it = vinculos.find(idUser);
    if (it == vinculos.end()) {
        return std::vector<std::string>(); // Retorna lista vazia
    }
    return it->second;
}

std::vector<Fatura> ArmazenamentoVolatil::buscarFaturas(int idUser) {
    auto it = faturas.find(idUser);
    if (it == faturas.end()) {
        return std::vector<Fatura>(); // Retorna lista vazia
    }
    return it->second;
}

void ArmazenamentoVolatil::salvarFatura(const Fatura& fatura) {
    faturas[fatura.getIdUsuario()].push_back(fatura);
}

bool ArmazenamentoVolatil::usuarioExiste(int id) {
    return usuarios.find(id) != usuarios.end();
}

std::vector<Usuario> ArmazenamentoVolatil::listarTodos() {
    std::vector<Usuario> lista;
    for (const auto& pair : usuarios) {
        lista.push_back(pair.second);
    }
    return lista;
}

int ArmazenamentoVolatil::proximoId() {
    return nextId++;
}
