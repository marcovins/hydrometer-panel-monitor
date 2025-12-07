# Subsistema de Gerenciamento de Usuários

## Descrição

Este subsistema implementa o gerenciamento de usuários e suas contas de água para o Sistema de Monitoramento de Hidrômetros (SSMH). Foi desenvolvido seguindo os padrões de projeto **Command** e **Strategy** conforme especificado no documento de requisitos.

## Padrões de Projeto Implementados

### 1. Padrão Command
- **Interface**: `UserCommand`
- **Comandos Concretos**:
  - `CriarUsuarioCommand`: Cria um novo usuário
  - `AtualizarUsuarioCommand`: Atualiza dados de um usuário existente
  - `DeletarUsuarioCommand`: Remove um usuário do sistema
  - `VincularHidrometroCommand`: Associa um hidrômetro a um usuário
- **Invoker**: `CommandInvoker` - Gerencia execução e histórico de comandos (undo/redo)
- **Receiver**: `UsuarioService` - Executa as operações de negócio

### 2. Padrão Strategy
- **Interface**: `ArmazenamentoStrategy`
- **Estratégias Concretas**:
  - `ArmazenamentoVolatil`: Persistência em memória (ideal para testes)
  - `ArmazenamentoSqlite`: (A ser implementado) Persistência em banco de dados

## Estrutura de Arquivos

```
src/modules/
├── usuario.hpp/cpp              # Entidades de domínio (Usuario, Fatura, TipoPerfil)
├── armazenamento_strategy.hpp  # Interface Strategy para persistência
├── armazenamento_volatil.hpp/cpp # Implementação em memória
├── usuario_service.hpp/cpp      # Receiver - Lógica de negócio
├── user_command.hpp             # Interface Command
├── user_commands.hpp/cpp        # Comandos concretos
└── command_invoker.hpp/cpp      # Invoker - Gerenciador de comandos
```

## Funcionalidades

### Gerenciamento de Usuários
- **Criar**: Adiciona novos usuários ao sistema
- **Buscar**: Recupera dados de um usuário por ID
- **Atualizar**: Modifica informações de usuários existentes
- **Deletar**: Remove usuários do sistema
- **Listar**: Retorna todos os usuários cadastrados

### Perfis de Acesso
- **ADMIN**: Acesso completo ao sistema
- **LEITOR**: Acesso somente leitura

### Gerenciamento de Hidrômetros
- **Vincular**: Associa um hidrômetro (SHA) a um usuário
- **Desvincular**: Remove a associação
- **Listar**: Retorna todos os hidrômetros de um usuário

### Gerenciamento de Faturas
- **Adicionar**: Cria nova fatura para um usuário
- **Listar**: Retorna todas as faturas de um usuário

### Controle de Histórico (Undo/Redo)
- Desfazer última operação executada
- Refazer operação desfeita
- Histórico completo de comandos

## Como Usar

### Exemplo Básico

```cpp
#include "src/modules/usuario_service.hpp"
#include "src/modules/user_commands.hpp"
#include "src/modules/command_invoker.hpp"

// Cria o serviço com armazenamento em memória
UsuarioService service;

// Cria o invoker para gerenciar comandos
CommandInvoker invoker;

// Cria um novo usuário
std::map<std::string, std::string> dados;
dados["nome"] = "Maria Silva";
dados["email"] = "maria@exemplo.com";
dados["tipoPerfil"] = "ADMIN";

auto comando = std::make_unique<CriarUsuarioCommand>(&service, dados);
invoker.executarComando(std::move(comando));

// Busca o usuário criado
Usuario usuario = service.buscarUsuario(1);

// Vincula um hidrômetro
auto cmdVincular = std::make_unique<VincularHidrometroCommand>(&service, 1, "SHA-001");
invoker.executarComando(std::move(cmdVincular));

// Desfaz a última operação
invoker.desfazer();
```

### Compilação e Teste

Para compilar o exemplo de teste:

```bash
g++ -std=c++11 -o test_usuarios test_usuarios.cpp \
    src/modules/usuario.cpp \
    src/modules/armazenamento_volatil.cpp \
    src/modules/usuario_service.cpp \
    src/modules/user_commands.cpp \
    src/modules/command_invoker.cpp

./test_usuarios
```

## Vantagens da Arquitetura

### Padrão Command
- ✅ **Histórico de operações**: Todas as ações são registradas
- ✅ **Undo/Redo**: Fácil reversão de operações
- ✅ **Extensibilidade**: Novos comandos podem ser adicionados sem modificar código existente
- ✅ **Logging**: Cada comando pode ser facilmente registrado

### Padrão Strategy
- ✅ **Flexibilidade**: Troca de estratégia de persistência em tempo de execução
- ✅ **Testabilidade**: Armazenamento em memória facilita testes unitários
- ✅ **Manutenibilidade**: Estratégias isoladas são fáceis de manter
- ✅ **Escalabilidade**: Novas estratégias (SQLite, MySQL, etc.) podem ser adicionadas facilmente

## Próximas Implementações

- [ ] `ArmazenamentoSqlite`: Persistência em banco de dados
- [ ] Validações avançadas de email e dados
- [ ] Autenticação e controle de sessão
- [ ] Log de auditoria das operações
- [ ] Integração com subsistema de alertas
- [ ] Testes unitários completos

## Integração com SSMH

Este subsistema é parte integrante da Fachada do Sistema de Monitoramento de Hidrômetros (SSMH) e será utilizado pela classe `Fachada` através do método:

```cpp
Fachada::executarComandoUsuario(std::unique_ptr<UserCommand> comando)
```

A Fachada atua como ponto de entrada único, escondendo a complexidade deste subsistema do cliente (GUI/CLI).

## Autores

Desenvolvido como parte do projeto acadêmico de Padrões de Projeto.
