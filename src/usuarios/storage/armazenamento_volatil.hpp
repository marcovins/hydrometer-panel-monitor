#ifndef ARMAZENAMENTO_VOLATIL_HPP
#define ARMAZENAMENTO_VOLATIL_HPP

#include "armazenamento_strategy.hpp"
#include <map>
#include <vector>
#include <stdexcept>

// Implementação de ArmazenamentoStrategy em memória (para testes)
class ArmazenamentoVolatil : public ArmazenamentoStrategy {
private:
    std::map<int, Usuario> usuarios;
    std::map<int, std::vector<std::string>> vinculos; // idUsuario -> lista de SHAs
    std::map<int, std::vector<Fatura>> faturas; // idUsuario -> lista de faturas
    int nextId;

public:
    ArmazenamentoVolatil();
    ~ArmazenamentoVolatil() override = default;
    
    // Implementação das operações CRUD
    void salvar(const Usuario& usuario) override;
    Usuario buscar(int id) override;
    void atualizar(int id, const std::map<std::string, std::string>& dados) override;
    void deletar(int id) override;
    
    // Implementação das operações de hidrômetros
    void associarHidrometro(int idUser, const std::string& idSha) override;
    void desassociarHidrometro(int idUser, const std::string& idSha) override;
    std::vector<std::string> listarHidrometros(int idUser) override;
    
    // Implementação das operações de faturas
    std::vector<Fatura> buscarFaturas(int idUser) override;
    void salvarFatura(const Fatura& fatura) override;
    
    // Métodos auxiliares
    bool usuarioExiste(int id) override;
    std::vector<Usuario> listarTodos() override;
    int proximoId() override;
};

#endif
