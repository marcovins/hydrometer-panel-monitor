#include "usuario_service.hpp"
#include "../storage/armazenamento_volatil.hpp"
#include <stdexcept>

UsuarioService::UsuarioService() {
    // Por padrão, usa armazenamento em memória volátil
    armazenamentoStrategy = std::make_unique<ArmazenamentoVolatil>();
}

UsuarioService::UsuarioService(std::unique_ptr<ArmazenamentoStrategy> strategy) 
    : armazenamentoStrategy(std::move(strategy)) {}

void UsuarioService::definirImplementacao(std::unique_ptr<ArmazenamentoStrategy> strategy) {
    if (strategy) {
        armazenamentoStrategy = std::move(strategy);
    }
}

Usuario UsuarioService::criarUsuario(const std::map<std::string, std::string>& dados) {
    if (!armazenamentoStrategy) {
        throw std::runtime_error("Estrategia de armazenamento nao configurada");
    }
    
    // Valida dados obrigatórios
    if (dados.find("nome") == dados.end() || dados.at("nome").empty()) {
        throw std::invalid_argument("Nome do usuario e obrigatorio");
    }
    
    if (dados.find("email") == dados.end() || dados.at("email").empty()) {
        throw std::invalid_argument("Email do usuario e obrigatorio");
    }
    
    // Cria novo ID
    int novoId = armazenamentoStrategy->proximoId();
    
    // Define tipo de perfil (padrão: LEITOR)
    TipoPerfil tipo = TipoPerfil::LEITOR;
    if (dados.find("tipoPerfil") != dados.end()) {
        tipo = Usuario::stringToTipoPerfil(dados.at("tipoPerfil"));
    }
    
    // Cria o usuário
    Usuario novoUsuario(novoId, dados.at("nome"), dados.at("email"), tipo);
    
    // Persiste
    armazenamentoStrategy->salvar(novoUsuario);
    
    return novoUsuario;
}

Usuario UsuarioService::buscarUsuario(int id) {
    if (!armazenamentoStrategy) {
        throw std::runtime_error("Estrategia de armazenamento nao configurada");
    }
    
    return armazenamentoStrategy->buscar(id);
}

void UsuarioService::atualizarUsuario(int id, const std::map<std::string, std::string>& dados) {
    if (!armazenamentoStrategy) {
        throw std::runtime_error("Estrategia de armazenamento nao configurada");
    }
    
    // Verifica se usuário existe antes de atualizar
    if (!armazenamentoStrategy->usuarioExiste(id)) {
        throw std::runtime_error("Usuario nao encontrado: ID " + std::to_string(id));
    }
    
    armazenamentoStrategy->atualizar(id, dados);
}

void UsuarioService::deletarUsuario(int id) {
    if (!armazenamentoStrategy) {
        throw std::runtime_error("Estrategia de armazenamento nao configurada");
    }
    
    armazenamentoStrategy->deletar(id);
}

std::vector<Usuario> UsuarioService::listarUsuarios() {
    if (!armazenamentoStrategy) {
        throw std::runtime_error("Estrategia de armazenamento nao configurada");
    }
    
    return armazenamentoStrategy->listarTodos();
}

void UsuarioService::vincularHidrometro(int idUser, const std::string& idSha) {
    if (!armazenamentoStrategy) {
        throw std::runtime_error("Estrategia de armazenamento nao configurada");
    }
    
    if (idSha.empty()) {
        throw std::invalid_argument("ID do hidrômetro nao pode ser vazio");
    }
    
    armazenamentoStrategy->associarHidrometro(idUser, idSha);
}

void UsuarioService::desvincularHidrometro(int idUser, const std::string& idSha) {
    if (!armazenamentoStrategy) {
        throw std::runtime_error("Estrategia de armazenamento nao configurada");
    }
    
    armazenamentoStrategy->desassociarHidrometro(idUser, idSha);
}

std::vector<std::string> UsuarioService::listarHidrometros(int idUser) {
    if (!armazenamentoStrategy) {
        throw std::runtime_error("Estrategia de armazenamento nao configurada");
    }
    
    return armazenamentoStrategy->listarHidrometros(idUser);
}

std::vector<Fatura> UsuarioService::listarContasDeAgua(int idUser) {
    if (!armazenamentoStrategy) {
        throw std::runtime_error("Estrategia de armazenamento nao configurada");
    }
    
    return armazenamentoStrategy->buscarFaturas(idUser);
}

void UsuarioService::adicionarFatura(int idUser, double valor, std::time_t dataVencimento, const std::string& status) {
    if (!armazenamentoStrategy) {
        throw std::runtime_error("Estrategia de armazenamento nao configurada");
    }
    
    if (!armazenamentoStrategy->usuarioExiste(idUser)) {
        throw std::runtime_error("Usuario nao encontrado: ID " + std::to_string(idUser));
    }
    
    // Cria ID único para fatura (simples: timestamp + idUser)
    int idFatura = static_cast<int>(std::time(nullptr)) + idUser;
    
    Fatura fatura(idFatura, idUser, valor, dataVencimento, status);
    armazenamentoStrategy->salvarFatura(fatura);
}

bool UsuarioService::usuarioExiste(int id) {
    if (!armazenamentoStrategy) {
        throw std::runtime_error("Estrategia de armazenamento nao configurada");
    }
    
    return armazenamentoStrategy->usuarioExiste(id);
}
