# Integração do Subsistema de Usuários com Banco de Dados

## Visão Geral

O subsistema de usuários foi estendido para suportar **persistência em banco de dados SQLite**, mantendo a flexibilidade do **padrão Strategy**. Agora você pode escolher entre:

- **ArmazenamentoVolatil**: Dados em memória (ideal para testes rápidos)
- **ArmazenamentoSqlite**: Dados persistidos em banco SQLite (ideal para produção)

## Arquitetura

```
UsuarioService
      ↓ (usa)
ArmazenamentoStrategy (interface)
      ↓ (implementações)
      ├── ArmazenamentoVolatil (memória)
      └── ArmazenamentoSqlite (SQLite)
```

## Estrutura do Banco de Dados

### Tabela: `usuarios`
```sql
CREATE TABLE usuarios (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nome TEXT NOT NULL,
    email TEXT NOT NULL,
    tipo_perfil TEXT NOT NULL DEFAULT 'LEITOR'
);
```

### Tabela: `vinculos_hidrometro`
```sql
CREATE TABLE vinculos_hidrometro (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    id_usuario INTEGER NOT NULL,
    id_sha TEXT NOT NULL,
    FOREIGN KEY (id_usuario) REFERENCES usuarios(id) ON DELETE CASCADE,
    UNIQUE(id_usuario, id_sha)
);
```

### Tabela: `faturas`
```sql
CREATE TABLE faturas (
    id INTEGER PRIMARY KEY,
    id_usuario INTEGER NOT NULL,
    valor REAL NOT NULL,
    data_vencimento INTEGER NOT NULL,
    status TEXT NOT NULL,
    FOREIGN KEY (id_usuario) REFERENCES usuarios(id) ON DELETE CASCADE
);
```

## Como Usar

### Opção 1: Criação Manual

```cpp
#include "src/modules/usuario_service.hpp"
#include "src/modules/armazenamento_sqlite.hpp"
#include "src/modules/armazenamento_volatil.hpp"

// Com SQLite (persistente)
auto serviceSqlite = std::make_unique<UsuarioService>(
    std::make_unique<ArmazenamentoSqlite>("meu_banco.db")
);

// Com Memória (volátil)
auto serviceVolatil = std::make_unique<UsuarioService>(
    std::make_unique<ArmazenamentoVolatil>()
);
```

### Opção 2: Usando Factory (Recomendado)

```cpp
#include "src/modules/usuario_service_factory.hpp"

// Por tipo enum
auto service1 = UsuarioServiceFactory::criar(
    UsuarioServiceFactory::TipoArmazenamento::SQLITE,
    "ssmh.db"
);

// Por nome (útil para configuração)
auto service2 = UsuarioServiceFactory::criarPorNome("sqlite", "ssmh.db");
auto service3 = UsuarioServiceFactory::criarPorNome("volatil");
```

### Opção 3: Trocar Estratégia em Tempo de Execução

```cpp
UsuarioService service;

// Inicia com volátil (para testes)
service.definirImplementacao(std::make_unique<ArmazenamentoVolatil>());

// ... executa testes ...

// Troca para SQLite (para produção)
service.definirImplementacao(
    std::make_unique<ArmazenamentoSqlite>("producao.db")
);
```

## Testes Disponíveis

### 1. Teste Básico (Memória)
```bash
make test-usuarios
```

### 2. Teste Completo (Ambas Estratégias)
```bash
make test-usuarios-db
```

### 3. Teste Apenas SQLite
```bash
make test-sqlite
# ou
./test_usuarios_db sqlite
```

### 4. Teste Apenas Volátil
```bash
make test-volatil
# ou
./test_usuarios_db volatil
```

## Vantagens da Implementação

### 1. **Flexibilidade** (Padrão Strategy)
- Troca de estratégia em tempo de execução
- Sem modificar código cliente
- Fácil adicionar novas estratégias (MySQL, PostgreSQL, etc.)

### 2. **Testabilidade**
- ArmazenamentoVolatil para testes unitários rápidos
- Sem dependência de banco em testes
- Isolamento completo

### 3. **Persistência Robusta**
- SQLite embutido (sem servidor externo)
- ACID compliant
- Foreign keys e índices
- Backup simples (um arquivo)

### 4. **Desacoplamento**
- UsuarioService não conhece detalhes de persistência
- Estratégias isoladas
- Manutenção facilitada

## Exemplo Completo de Uso

```cpp
#include "src/modules/usuario_service_factory.hpp"
#include "src/modules/user_commands.hpp"
#include "src/modules/command_invoker.hpp"

int main() {
    // Cria serviço com SQLite
    auto service = UsuarioServiceFactory::criarPorNome("sqlite", "app.db");
    
    // Cria invoker para comandos
    CommandInvoker invoker;
    
    // Cria usuário
    std::map<std::string, std::string> dados;
    dados["nome"] = "Maria Silva";
    dados["email"] = "maria@exemplo.com";
    dados["tipoPerfil"] = "ADMIN";
    
    auto comando = std::make_unique<CriarUsuarioCommand>(service.get(), dados);
    invoker.executarComando(std::move(comando));
    
    // Lista todos os usuários (persistidos)
    auto usuarios = service->listarUsuarios();
    
    // Pode desfazer
    invoker.desfazer();
    
    return 0;
}
```

## Migrações e Compatibilidade

O banco é criado automaticamente na primeira execução. As tabelas são criadas com `CREATE TABLE IF NOT EXISTS`, garantindo que múltiplas execuções não causem erros.

### Verificar Dados no Banco

```bash
# Instalar sqlite3 se necessário
sudo apt install sqlite3

# Abrir banco
sqlite3 ssmh_test.db

# Comandos úteis
.tables                    # Lista tabelas
SELECT * FROM usuarios;    # Lista usuários
SELECT * FROM vinculos_hidrometro;  # Lista vínculos
.schema usuarios          # Mostra estrutura da tabela
.exit                     # Sair
```

## Dependências

- **SQLite3**: Biblioteca de banco de dados
  ```bash
  # Ubuntu/Debian
  sudo apt install libsqlite3-dev
  
  # Fedora
  sudo dnf install sqlite-devel
  
  # macOS
  brew install sqlite3
  ```

## Performance

### ArmazenamentoVolatil
- ✅ Velocidade máxima (RAM)
- ✅ Sem I/O de disco
- ❌ Dados não persistem

### ArmazenamentoSqlite
- ✅ Dados persistentes
- ✅ ACID compliant
- ✅ Performance excelente para aplicação
- ✅ Índices otimizados
- ⚠️  Pequeno overhead de I/O

## Próximos Passos

1. **Migração de dados**: Implementar export/import entre estratégias
2. **Pool de conexões**: Para alta concorrência
3. **Transações**: Suporte explícito a transações
4. **Cache**: Camada de cache sobre SQLite
5. **Backup automatizado**: Sistema de backup periódico

## Integração com a Fachada

```cpp
class Fachada {
private:
    std::unique_ptr<UsuarioService> usuarioService;
    CommandInvoker invoker;
    
public:
    Fachada(const std::string& tipoArmazenamento = "sqlite") {
        // Cria serviço com estratégia escolhida
        usuarioService = UsuarioServiceFactory::criarPorNome(tipoArmazenamento);
    }
    
    void executarComandoUsuario(std::unique_ptr<UserCommand> comando) {
        invoker.executarComando(std::move(comando));
    }
};
```

## Troubleshooting

### Erro: "libsqlite3.so not found"
```bash
sudo apt install libsqlite3-dev
make clean
make test-usuarios-db
```

### Erro: "database is locked"
- Feche outras conexões ao banco
- Verifique se não há processos travados

### Banco corrompido
```bash
rm ssmh_test.db
./test_usuarios_db sqlite  # Recria banco
```

---

**Status**: ✅ Implementação completa e testada  
**Data**: 07/12/2025  
**Versão**: 2.0 - Com persistência SQLite
