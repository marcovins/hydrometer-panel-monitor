#ifndef ARMAZENAMENTO_SQLITE_HPP
#define ARMAZENAMENTO_SQLITE_HPP

#include "armazenamento_strategy.hpp"
#include <sqlite3.h>
#include <string>
#include <memory>

// Implementação de ArmazenamentoStrategy usando SQLite
class ArmazenamentoSqlite : public ArmazenamentoStrategy {
private:
    sqlite3* db;
    std::string dbPath;
    bool initialized;
    
    // Métodos auxiliares privados
    void criarTabelas();
    void executarSQL(const std::string& sql);
    int obterUltimoId(const std::string& tabela);
    std::string escaparString(const std::string& str);

public:
    explicit ArmazenamentoSqlite(const std::string& caminhoDb = "ssmh.db");
    ~ArmazenamentoSqlite() override;
    
    // Impede cópia e movimentação
    ArmazenamentoSqlite(const ArmazenamentoSqlite&) = delete;
    ArmazenamentoSqlite& operator=(const ArmazenamentoSqlite&) = delete;
    
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
    
    // Método para verificar se está conectado
    bool estaConectado() const;
};

#endif
