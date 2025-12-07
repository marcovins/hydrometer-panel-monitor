# Estrutura Modular do Projeto SSMH

## Visão Geral

Este diretório contém todo o código-fonte do Sistema de Monitoramento de Hidrômetros (SSMH), organizado em uma **estrutura modular** que facilita manutenção, testes e escalabilidade.

## Estrutura de Diretórios

```
src/
├── simulator/          # Módulo do Simulador de Hidrômetro
│   ├── simulator.hpp/cpp    - Orquestrador da simulação
│   ├── hidrometer.hpp/cpp   - Lógica do hidrômetro
│   └── pipe.hpp/cpp         - Sistema de tubulação
│
├── usuarios/           # Módulo de Gerenciamento de Usuários
│   ├── domain/              - Entidades de Domínio
│   │   ├── usuario.hpp/cpp      - Entidade Usuario
│   │   └── (Fatura incluída)    - Entidade Fatura
│   │
│   ├── storage/             - Camada de Persistência (Strategy)
│   │   ├── armazenamento_strategy.hpp     - Interface Strategy
│   │   ├── armazenamento_volatil.hpp/cpp  - Implementação em memória
│   │   └── armazenamento_sqlite.hpp/cpp   - Implementação SQLite
│   │
│   ├── commands/            - Padrão Command
│   │   ├── user_command.hpp               - Interface Command
│   │   ├── user_commands.hpp/cpp          - Comandos concretos
│   │   └── command_invoker.hpp/cpp        - Invoker (undo/redo)
│   │
│   └── services/            - Lógica de Negócio
│       ├── usuario_service.hpp/cpp        - Serviço principal
│       └── usuario_service_factory.hpp    - Factory para criação
│
├── utils/              # Utilitários Compartilhados
│   ├── logger.hpp/cpp       - Sistema de logging (Singleton)
│   └── image.hpp/cpp        - Processamento de imagens
│
└── core/               # (Futuro) Componentes centrais compartilhados
```

## Módulos Detalhados

### 1. Módulo Simulator (`simulator/`)

**Responsabilidade:** Simula o funcionamento de um hidrômetro em tempo real.

**Componentes:**
- `Simulator`: Orquestra a simulação completa
- `Hidrometer`: Representa o hidrômetro físico
- `Pipe`: Sistema de tubulação com entrada/saída de água

**Padrões:** Observer (eventos de mudança de estado)

### 2. Módulo Usuarios (`usuarios/`)

**Responsabilidade:** Gerencia todo o ciclo de vida de usuários e suas contas de água.

#### 2.1. Domain (`usuarios/domain/`)
Contém as **entidades de domínio** puras, sem lógica de persistência ou apresentação.

- `Usuario`: Representa um usuário do sistema
- `Fatura`: Representa uma conta de água
- `TipoPerfil`: Enum para tipos de usuário (ADMIN/LEITOR)

**Princípio:** Domain-Driven Design (DDD)

#### 2.2. Storage (`usuarios/storage/`)
Implementa a **camada de persistência** usando o padrão Strategy.

- `ArmazenamentoStrategy`: Interface comum
- `ArmazenamentoVolatil`: Dados em memória (testes)
- `ArmazenamentoSqlite`: Dados persistentes (produção)

**Padrão:** Strategy + Repository Pattern

**Benefícios:**
- Troca transparente de estratégia
- Testes unitários sem banco
- Fácil adicionar novos backends (MySQL, PostgreSQL, etc.)

#### 2.3. Commands (`usuarios/commands/`)
Implementa o **padrão Command** para operações com undo/redo.

- `UserCommand`: Interface base
- `CriarUsuarioCommand`: Criar usuário
- `AtualizarUsuarioCommand`: Atualizar dados
- `DeletarUsuarioCommand`: Remover usuário
- `VincularHidrometroCommand`: Associar hidrômetro
- `CommandInvoker`: Gerencia execução e histórico

**Padrão:** Command

**Benefícios:**
- Undo/Redo automático
- Histórico de operações
- Fácil adicionar novas operações
- Logging transparente

#### 2.4. Services (`usuarios/services/`)
Contém a **lógica de negócio** e coordenação entre camadas.

- `UsuarioService`: Serviço principal (Receiver do Command)
- `UsuarioServiceFactory`: Simplifica criação com Strategy

**Padrão:** Service Layer + Factory

**Benefícios:**
- Encapsulamento da lógica de negócio
- Ponto único de entrada
- Validações centralizadas

### 3. Módulo Utils (`utils/`)

**Responsabilidade:** Utilitários compartilhados por todo o sistema.

- `Logger`: Sistema de logging global (Singleton)
- `Image`: Processamento de imagens para OCR

**Padrão:** Singleton (Logger)

## Dependências Entre Módulos

```
main.cpp
    ↓
simulator/ ← utils/logger
    ↓
usuarios/services/
    ↓
usuarios/commands/
    ↓
usuarios/storage/ → usuarios/domain/
```

**Regras de Dependência:**
1. Domain não depende de ninguém (camada mais interna)
2. Storage só depende de Domain
3. Services coordena Storage e Commands
4. Commands usa Services (receptor)
5. Utils pode ser usado por qualquer módulo

## Como Adicionar um Novo Módulo

### Exemplo: Módulo de Alertas

1. **Criar estrutura:**
   ```bash
   mkdir -p src/alertas/{domain,services,strategies}
   ```

2. **Domain:** Entidades puras
   ```cpp
   // src/alertas/domain/alerta.hpp
   class Alerta {
       // Entidade de domínio
   };
   ```

3. **Strategies:** Implementações intercambiáveis
   ```cpp
   // src/alertas/strategies/notificacao_strategy.hpp
   class NotificacaoStrategy {
       virtual void notificar() = 0;
   };
   ```

4. **Services:** Lógica de negócio
   ```cpp
   // src/alertas/services/alerta_service.hpp
   class AlertaService {
       // Coordena domain e strategies
   };
   ```

5. **Atualizar Makefile:**
   ```makefile
   ALERTAS_SOURCES = $(SRC_DIR)/alertas/domain/alerta.cpp \
                     $(SRC_DIR)/alertas/services/alerta_service.cpp
   ```

## Princípios Seguidos

### SOLID

- **S**ingle Responsibility: Cada classe tem uma responsabilidade única
- **O**pen/Closed: Extensível via Strategy, Command, etc.
- **L**iskov Substitution: Estratégias são intercambiáveis
- **I**nterface Segregation: Interfaces pequenas e coesas
- **D**ependency Inversion: Depende de abstrações (Strategy, Command)

### Clean Architecture

- **Camadas bem definidas:** Domain → Storage → Services → Commands
- **Independência de frameworks:** Core não depende de SQLite
- **Testável:** Cada camada pode ser testada isoladamente
- **Independente de UI:** Lógica separada da apresentação

### DRY (Don't Repeat Yourself)

- Utils compartilhados (Logger, Image)
- Factory para criação padronizada
- Strategy para evitar código duplicado

## Compilação Modular

O Makefile foi organizado para compilar módulos independentemente:

```makefile
# Apenas Simulator
SIMULATOR_SOURCES = $(SIMULATOR_DIR)/simulator.cpp ...

# Apenas Usuarios
USUARIO_SOURCES = $(USUARIOS_DIR)/domain/usuario.cpp ...

# Combinar conforme necessário
ALL_SOURCES = $(SIMULATOR_SOURCES) $(USUARIO_SOURCES) ...
```

## Testes

Cada módulo pode ser testado independentemente:

```bash
make test-usuarios      # Testa módulo de usuários
make test-sqlite        # Testa persistência SQLite
make exemplo-factory    # Testa Factory
```

## Padrões de Nomenclatura

### Arquivos
- Headers: `.hpp`
- Implementações: `.cpp`
- Nomes em snake_case: `usuario_service.hpp`

### Classes
- PascalCase: `UsuarioService`, `ArmazenamentoStrategy`
- Interfaces terminam em sufixo descritivo: `Strategy`, `Command`

### Diretórios
- Lowercase: `domain/`, `storage/`, `commands/`
- Nome do módulo principal: `usuarios/`, `simulator/`

## Evolução Futura

### Próximos Módulos Planejados
1. **alertas/** - Sistema de alertas e notificações
2. **monitoramento/** - Coleta e análise de dados
3. **fachada/** - Ponto de entrada unificado (Facade Pattern)
4. **api/** - Interface REST (se necessário)

### Melhorias Planejadas
- [ ] Testes unitários com Google Test
- [ ] Documentação automática com Doxygen
- [ ] CI/CD com GitHub Actions
- [ ] Cobertura de código
- [ ] Análise estática (cppcheck, clang-tidy)

## Contribuindo

Ao adicionar código:
1. **Respeite a estrutura modular**
2. **Mantenha as dependências unidirecionais**
3. **Documente com comentários claros**
4. **Adicione testes quando aplicável**
5. **Atualize este README se necessário**

## Referências

- **Design Patterns:** Gang of Four
- **Clean Architecture:** Robert C. Martin
- **Domain-Driven Design:** Eric Evans
- **SOLID Principles:** Robert C. Martin

---

**Última atualização:** 07/12/2025  
**Versão:** 2.0 - Estrutura Modular
