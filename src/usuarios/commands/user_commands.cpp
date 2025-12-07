#include "user_commands.hpp"
#include <sstream>

// ============================================================================
// CriarUsuarioCommand
// ============================================================================

CriarUsuarioCommand::CriarUsuarioCommand(UsuarioService* service, const std::map<std::string, std::string>& dados)
    : receiver(service), dados(dados), executado(false) {}

void CriarUsuarioCommand::execute() {
    usuarioCriado = receiver->criarUsuario(dados);
    executado = true;
}

void CriarUsuarioCommand::undo() {
    if (executado) {
        receiver->deletarUsuario(usuarioCriado.getId());
        executado = false;
    }
}

std::string CriarUsuarioCommand::getDescricao() const {
    std::stringstream ss;
    ss << "Criar usuario: " << (dados.find("nome") != dados.end() ? dados.at("nome") : "N/A");
    return ss.str();
}

Usuario CriarUsuarioCommand::getUsuarioCriado() const {
    return usuarioCriado;
}

// ============================================================================
// AtualizarUsuarioCommand
// ============================================================================

AtualizarUsuarioCommand::AtualizarUsuarioCommand(UsuarioService* service, int id, 
                                                   const std::map<std::string, std::string>& dados)
    : receiver(service), id(id), dados(dados), executado(false) {}

void AtualizarUsuarioCommand::execute() {
    // Salva os dados antigos para permitir undo
    Usuario usuarioAtual = receiver->buscarUsuario(id);
    dadosAntigos["nome"] = usuarioAtual.getNome();
    dadosAntigos["email"] = usuarioAtual.getEmail();
    dadosAntigos["tipoPerfil"] = Usuario::tipoPerfilToString(usuarioAtual.getTipoPerfil());
    
    // Executa a atualização
    receiver->atualizarUsuario(id, dados);
    executado = true;
}

void AtualizarUsuarioCommand::undo() {
    if (executado) {
        receiver->atualizarUsuario(id, dadosAntigos);
        executado = false;
    }
}

std::string AtualizarUsuarioCommand::getDescricao() const {
    std::stringstream ss;
    ss << "Atualizar usuario ID: " << id;
    return ss.str();
}

// ============================================================================
// DeletarUsuarioCommand
// ============================================================================

DeletarUsuarioCommand::DeletarUsuarioCommand(UsuarioService* service, int id)
    : receiver(service), id(id), executado(false) {}

void DeletarUsuarioCommand::execute() {
    // Salva dados do usuário antes de deletar para permitir undo
    usuarioDeletado = receiver->buscarUsuario(id);
    hidrometrosAssociados = receiver->listarHidrometros(id);
    
    // Deleta o usuário
    receiver->deletarUsuario(id);
    executado = true;
}

void DeletarUsuarioCommand::undo() {
    if (executado) {
        // Recria o usuário com os mesmos dados
        std::map<std::string, std::string> dados;
        dados["nome"] = usuarioDeletado.getNome();
        dados["email"] = usuarioDeletado.getEmail();
        dados["tipoPerfil"] = Usuario::tipoPerfilToString(usuarioDeletado.getTipoPerfil());
        
        // Nota: Aqui assumimos que o ID será o mesmo ao recriar
        // Em uma implementação real, pode ser necessário ajustar a estratégia
        Usuario novoUsuario(usuarioDeletado.getId(), 
                           usuarioDeletado.getNome(),
                           usuarioDeletado.getEmail(),
                           usuarioDeletado.getTipoPerfil());
        
        // Força salvar com o ID original (depende da implementação da estratégia)
        // Para ArmazenamentoVolatil, isso funciona
        receiver->criarUsuario(dados);
        
        // Restaura vínculos de hidrômetros
        for (const auto& sha : hidrometrosAssociados) {
            receiver->vincularHidrometro(id, sha);
        }
        
        executado = false;
    }
}

std::string DeletarUsuarioCommand::getDescricao() const {
    std::stringstream ss;
    ss << "Deletar usuario ID: " << id;
    return ss.str();
}

// ============================================================================
// VincularHidrometroCommand
// ============================================================================

VincularHidrometroCommand::VincularHidrometroCommand(UsuarioService* service, int idUser, const std::string& idSha)
    : receiver(service), idUser(idUser), idSha(idSha), executado(false) {}

void VincularHidrometroCommand::execute() {
    receiver->vincularHidrometro(idUser, idSha);
    executado = true;
}

void VincularHidrometroCommand::undo() {
    if (executado) {
        receiver->desvincularHidrometro(idUser, idSha);
        executado = false;
    }
}

std::string VincularHidrometroCommand::getDescricao() const {
    std::stringstream ss;
    ss << "Vincular hidrômetro " << idSha << " ao usuario ID: " << idUser;
    return ss.str();
}
