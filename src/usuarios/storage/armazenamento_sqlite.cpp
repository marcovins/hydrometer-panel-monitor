#include "armazenamento_sqlite.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>

ArmazenamentoSqlite::ArmazenamentoSqlite(const std::string& caminhoDb) 
    : db(nullptr), dbPath(caminhoDb), initialized(false) {
    
    // Abre/cria o banco de dados
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::string erro = "Erro ao abrir banco de dados: ";
        if (db) {
            erro += sqlite3_errmsg(db);
            sqlite3_close(db);
        }
        throw std::runtime_error(erro);
    }
    
    // Cria as tabelas se não existirem
    criarTabelas();
    initialized = true;
}

ArmazenamentoSqlite::~ArmazenamentoSqlite() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

void ArmazenamentoSqlite::criarTabelas() {
    // Tabela de usuários
    std::string sqlUsuarios = R"(
        CREATE TABLE IF NOT EXISTS usuarios (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nome TEXT NOT NULL,
            email TEXT NOT NULL,
            tipo_perfil TEXT NOT NULL DEFAULT 'LEITOR'
        );
    )";
    
    // Tabela de vínculos usuário-hidrômetro
    std::string sqlVinculos = R"(
        CREATE TABLE IF NOT EXISTS vinculos_hidrometro (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            id_usuario INTEGER NOT NULL,
            id_sha TEXT NOT NULL,
            FOREIGN KEY (id_usuario) REFERENCES usuarios(id) ON DELETE CASCADE,
            UNIQUE(id_usuario, id_sha)
        );
    )";
    
    // Tabela de faturas
    std::string sqlFaturas = R"(
        CREATE TABLE IF NOT EXISTS faturas (
            id INTEGER PRIMARY KEY,
            id_usuario INTEGER NOT NULL,
            valor REAL NOT NULL,
            data_vencimento INTEGER NOT NULL,
            status TEXT NOT NULL,
            FOREIGN KEY (id_usuario) REFERENCES usuarios(id) ON DELETE CASCADE
        );
    )";
    
    // Índices para melhor performance
    std::string sqlIndices = R"(
        CREATE INDEX IF NOT EXISTS idx_vinculos_usuario ON vinculos_hidrometro(id_usuario);
        CREATE INDEX IF NOT EXISTS idx_faturas_usuario ON faturas(id_usuario);
    )";
    
    executarSQL(sqlUsuarios);
    executarSQL(sqlVinculos);
    executarSQL(sqlFaturas);
    executarSQL(sqlIndices);
}

void ArmazenamentoSqlite::executarSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::string erro = "Erro SQL: ";
        if (errMsg) {
            erro += errMsg;
            sqlite3_free(errMsg);
        }
        throw std::runtime_error(erro);
    }
}

std::string ArmazenamentoSqlite::escaparString(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        if (c == '\'') {
            escaped += "''"; // Escape de aspas simples em SQL
        } else {
            escaped += c;
        }
    }
    return escaped;
}

int ArmazenamentoSqlite::obterUltimoId(const std::string& tabela) {
    std::stringstream ss;
    ss << "SELECT MAX(id) FROM " << tabela;
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, ss.str().c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Erro ao preparar statement para obter último ID");
    }
    
    int maxId = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
            maxId = sqlite3_column_int(stmt, 0);
        }
    }
    
    sqlite3_finalize(stmt);
    return maxId;
}

void ArmazenamentoSqlite::salvar(const Usuario& usuario) {
    std::stringstream ss;
    ss << "INSERT INTO usuarios (id, nome, email, tipo_perfil) VALUES ("
       << usuario.getId() << ", "
       << "'" << escaparString(usuario.getNome()) << "', "
       << "'" << escaparString(usuario.getEmail()) << "', "
       << "'" << Usuario::tipoPerfilToString(usuario.getTipoPerfil()) << "')";
    
    executarSQL(ss.str());
}

Usuario ArmazenamentoSqlite::buscar(int id) {
    std::stringstream ss;
    ss << "SELECT id, nome, email, tipo_perfil FROM usuarios WHERE id = " << id;
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, ss.str().c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Erro ao preparar statement de busca");
    }
    
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Usuario nao encontrado: ID " + std::to_string(id));
    }
    
    int userId = sqlite3_column_int(stmt, 0);
    std::string nome = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    std::string tipoStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    
    TipoPerfil tipo = Usuario::stringToTipoPerfil(tipoStr);
    
    sqlite3_finalize(stmt);
    
    return Usuario(userId, nome, email, tipo);
}

void ArmazenamentoSqlite::atualizar(int id, const std::map<std::string, std::string>& dados) {
    if (!usuarioExiste(id)) {
        throw std::runtime_error("Usuario nao encontrado para atualizacao: ID " + std::to_string(id));
    }
    
    std::stringstream ss;
    ss << "UPDATE usuarios SET ";
    
    bool primeiro = true;
    for (const auto& par : dados) {
        if (!primeiro) ss << ", ";
        
        if (par.first == "nome" || par.first == "email") {
            ss << par.first << " = '" << escaparString(par.second) << "'";
        } else if (par.first == "tipoPerfil") {
            ss << "tipo_perfil = '" << escaparString(par.second) << "'";
        }
        
        primeiro = false;
    }
    
    ss << " WHERE id = " << id;
    
    executarSQL(ss.str());
}

void ArmazenamentoSqlite::deletar(int id) {
    if (!usuarioExiste(id)) {
        throw std::runtime_error("Usuario nao encontrado para delecao: ID " + std::to_string(id));
    }
    
    std::stringstream ss;
    ss << "DELETE FROM usuarios WHERE id = " << id;
    
    executarSQL(ss.str());
}

void ArmazenamentoSqlite::associarHidrometro(int idUser, const std::string& idSha) {
    if (!usuarioExiste(idUser)) {
        throw std::runtime_error("Usuario nao encontrado: ID " + std::to_string(idUser));
    }
    
    std::stringstream ss;
    ss << "INSERT OR IGNORE INTO vinculos_hidrometro (id_usuario, id_sha) VALUES ("
       << idUser << ", '" << escaparString(idSha) << "')";
    
    executarSQL(ss.str());
}

void ArmazenamentoSqlite::desassociarHidrometro(int idUser, const std::string& idSha) {
    std::stringstream ss;
    ss << "DELETE FROM vinculos_hidrometro WHERE id_usuario = " << idUser
       << " AND id_sha = '" << escaparString(idSha) << "'";
    
    executarSQL(ss.str());
}

std::vector<std::string> ArmazenamentoSqlite::listarHidrometros(int idUser) {
    std::stringstream ss;
    ss << "SELECT id_sha FROM vinculos_hidrometro WHERE id_usuario = " << idUser;
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, ss.str().c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Erro ao preparar statement de listagem de hidrometros");
    }
    
    std::vector<std::string> hidrometros;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string idSha = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        hidrometros.push_back(idSha);
    }
    
    sqlite3_finalize(stmt);
    return hidrometros;
}

std::vector<Fatura> ArmazenamentoSqlite::buscarFaturas(int idUser) {
    std::stringstream ss;
    ss << "SELECT id, id_usuario, valor, data_vencimento, status FROM faturas WHERE id_usuario = " << idUser;
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, ss.str().c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Erro ao preparar statement de busca de faturas");
    }
    
    std::vector<Fatura> faturas;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int idUsuario = sqlite3_column_int(stmt, 1);
        double valor = sqlite3_column_double(stmt, 2);
        std::time_t dataVencimento = static_cast<std::time_t>(sqlite3_column_int64(stmt, 3));
        std::string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        
        faturas.emplace_back(id, idUsuario, valor, dataVencimento, status);
    }
    
    sqlite3_finalize(stmt);
    return faturas;
}

void ArmazenamentoSqlite::salvarFatura(const Fatura& fatura) {
    std::stringstream ss;
    ss << "INSERT INTO faturas (id, id_usuario, valor, data_vencimento, status) VALUES ("
       << fatura.getId() << ", "
       << fatura.getIdUsuario() << ", "
       << fatura.getValor() << ", "
       << static_cast<int64_t>(fatura.getDataVencimento()) << ", "
       << "'" << escaparString(fatura.getStatus()) << "')";
    
    executarSQL(ss.str());
}

bool ArmazenamentoSqlite::usuarioExiste(int id) {
    std::stringstream ss;
    ss << "SELECT COUNT(*) FROM usuarios WHERE id = " << id;
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, ss.str().c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        return false;
    }
    
    bool existe = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        existe = (sqlite3_column_int(stmt, 0) > 0);
    }
    
    sqlite3_finalize(stmt);
    return existe;
}

std::vector<Usuario> ArmazenamentoSqlite::listarTodos() {
    std::string sql = "SELECT id, nome, email, tipo_perfil FROM usuarios ORDER BY id";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Erro ao preparar statement de listagem");
    }
    
    std::vector<Usuario> usuarios;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string nome = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string tipoStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        TipoPerfil tipo = Usuario::stringToTipoPerfil(tipoStr);
        usuarios.emplace_back(id, nome, email, tipo);
    }
    
    sqlite3_finalize(stmt);
    return usuarios;
}

int ArmazenamentoSqlite::proximoId() {
    return obterUltimoId("usuarios") + 1;
}

bool ArmazenamentoSqlite::estaConectado() const {
    return db != nullptr && initialized;
}
