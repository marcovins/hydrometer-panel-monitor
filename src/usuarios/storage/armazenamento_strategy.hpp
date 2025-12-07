#ifndef ARMAZENAMENTO_STRATEGY_HPP
#define ARMAZENAMENTO_STRATEGY_HPP

#include <vector>
#include <string>
#include <map>
#include "../domain/usuario.hpp"

// Interface Strategy para persistência de dados
class ArmazenamentoStrategy {
public:
    virtual ~ArmazenamentoStrategy() = default;
    
    // Operações CRUD básicas para usuários
    virtual void salvar(const Usuario& usuario) = 0;
    virtual Usuario buscar(int id) = 0;
    virtual void atualizar(int id, const std::map<std::string, std::string>& dados) = 0;
    virtual void deletar(int id) = 0;
    
    // Operações para vincular hidrômetros
    virtual void associarHidrometro(int idUser, const std::string& idSha) = 0;
    virtual void desassociarHidrometro(int idUser, const std::string& idSha) = 0;
    virtual std::vector<std::string> listarHidrometros(int idUser) = 0;
    
    // Operações para faturas
    virtual std::vector<Fatura> buscarFaturas(int idUser) = 0;
    virtual void salvarFatura(const Fatura& fatura) = 0;
    
    // Verificar se usuário existe
    virtual bool usuarioExiste(int id) = 0;
    
    // Listar todos os usuários
    virtual std::vector<Usuario> listarTodos() = 0;
    
    // Obter próximo ID disponível
    virtual int proximoId() = 0;
};

#endif
