#ifndef USUARIO_SERVICE_HPP
#define USUARIO_SERVICE_HPP

#include "armazenamento_strategy.hpp"
#include "usuario.hpp"
#include <memory>
#include <string>
#include <map>

// UsuarioService - Receiver do padrão Command
// Usa Strategy para persistência
class UsuarioService {
private:
    std::unique_ptr<ArmazenamentoStrategy> armazenamentoStrategy;

public:
    UsuarioService();
    explicit UsuarioService(std::unique_ptr<ArmazenamentoStrategy> strategy);
    
    // Define ou muda a estratégia de armazenamento
    void definirImplementacao(std::unique_ptr<ArmazenamentoStrategy> strategy);
    
    // Operações de gerenciamento de usuários
    Usuario criarUsuario(const std::map<std::string, std::string>& dados);
    Usuario buscarUsuario(int id);
    void atualizarUsuario(int id, const std::map<std::string, std::string>& dados);
    void deletarUsuario(int id);
    std::vector<Usuario> listarUsuarios();
    
    // Operações de hidrômetros
    void vincularHidrometro(int idUser, const std::string& idSha);
    void desvincularHidrometro(int idUser, const std::string& idSha);
    std::vector<std::string> listarHidrometros(int idUser);
    
    // Operações de faturas/contas de água
    std::vector<Fatura> listarContasDeAgua(int idUser);
    void adicionarFatura(int idUser, double valor, std::time_t dataVencimento, const std::string& status);
    
    // Verificação de existência
    bool usuarioExiste(int id);
};

#endif
