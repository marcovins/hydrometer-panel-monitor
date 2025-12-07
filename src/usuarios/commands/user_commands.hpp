#ifndef USER_COMMANDS_HPP
#define USER_COMMANDS_HPP

#include "user_command.hpp"
#include "../domain/usuario.hpp"
#include <map>
#include <string>

// Comando para criar um usuário
class CriarUsuarioCommand : public UserCommand {
private:
    UsuarioService* receiver;
    std::map<std::string, std::string> dados;
    Usuario usuarioCriado;
    bool executado;

public:
    CriarUsuarioCommand(UsuarioService* service, const std::map<std::string, std::string>& dados);
    
    void execute() override;
    void undo() override;
    std::string getDescricao() const override;
    
    Usuario getUsuarioCriado() const;
};

// Comando para atualizar um usuário
class AtualizarUsuarioCommand : public UserCommand {
private:
    UsuarioService* receiver;
    int id;
    std::map<std::string, std::string> dados;
    std::map<std::string, std::string> dadosAntigos;
    bool executado;

public:
    AtualizarUsuarioCommand(UsuarioService* service, int id, const std::map<std::string, std::string>& dados);
    
    void execute() override;
    void undo() override;
    std::string getDescricao() const override;
};

// Comando para deletar um usuário
class DeletarUsuarioCommand : public UserCommand {
private:
    UsuarioService* receiver;
    int id;
    Usuario usuarioDeletado;
    std::vector<std::string> hidrometrosAssociados;
    bool executado;

public:
    DeletarUsuarioCommand(UsuarioService* service, int id);
    
    void execute() override;
    void undo() override;
    std::string getDescricao() const override;
};

// Comando para vincular hidrômetro
class VincularHidrometroCommand : public UserCommand {
private:
    UsuarioService* receiver;
    int idUser;
    std::string idSha;
    bool executado;

public:
    VincularHidrometroCommand(UsuarioService* service, int idUser, const std::string& idSha);
    
    void execute() override;
    void undo() override;
    std::string getDescricao() const override;
};

#endif
