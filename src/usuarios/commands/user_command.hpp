#ifndef USER_COMMAND_HPP
#define USER_COMMAND_HPP

#include "../services/usuario_service.hpp"
#include <memory>

// Interface do padrão Command
class UserCommand {
public:
    virtual ~UserCommand() = default;
    
    // Executa o comando
    virtual void execute() = 0;
    
    // Desfaz o comando
    virtual void undo() = 0;
    
    // Retorna descrição do comando para logging
    virtual std::string getDescricao() const = 0;
};

#endif
