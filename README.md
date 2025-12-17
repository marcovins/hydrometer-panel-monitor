# Sistema de Monitoramento de Hidrômetros (SSMH)

## 🎯 Visão Geral

O SSMH é um sistema modular completo para monitoramento de consumo de água através de hidrômetros digitais. O sistema integra três subsistemas principais através de uma **Arquitetura em Camadas** com padrões de projeto bem definidos.

## 📊 Progresso do Projeto
**✅ 100% Concluído (3/3 Subsistemas)**

🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩 100%

### Status dos Módulos
- ✅ **Subsistema de Usuários** - Completo com persistência SQLite e undo/redo
- ✅ **Subsistema de Monitoramento** - Completo com agregação de consumo e OCR
- ✅ **Subsistema de Alertas** - Completo com múltiplas estratégias e notificações
- ✅ **Fachada Central** - Integração completa dos três subsistemas

## 🏗️ Arquitetura e Padrões de Projeto

### Padrões Implementados

| Padrão | Localização | Propósito | Comportamento |
|--------|-------------|-----------|---------------|
| **Facade** | [src/core/fachada_ssmh.hpp](src/core/fachada_ssmh.hpp) | Ponto único de entrada | Orquestra os 3 subsistemas, simplificando interface complexa |
| **Strategy** | [src/usuarios/storage/](src/usuarios/storage/) | Persistência intercambiável | Troca transparente entre SQLite e Memória |
| **Strategy** | [src/alertas/strategies/](src/alertas/strategies/) | Análise de consumo | 3 algoritmos: Limite Diário, Média Móvel, Vazamento |
| **Strategy** | [src/alertas/notifications/](src/alertas/notifications/) | Canais de notificação | Console, Email (OAuth2) e Windows Popup |
| **Command** | [src/usuarios/commands/](src/usuarios/commands/) | CRUD com histórico | Undo/Redo completo para operações de usuário |
| **Observer** | [src/alertas/observers/](src/alertas/observers/) | Notificação assíncrona | 3 observers: Painel, Logger e Notificação |
| **Composite** | [src/monitoramento/composite/](src/monitoramento/composite/) | Agregação hierárquica | Calcula consumo individual ou agregado |
| **Adapter** | [src/monitoramento/adapter/](src/monitoramento/adapter/) | Integração OCR | Adapta bibliotecas externas ao sistema |
| **Factory** | [src/usuarios/services/](src/usuarios/services/) + [src/alertas/services/](src/alertas/services/) | Criação simplificada | Instancia serviços com configurações padrão |
| **Singleton** | [src/utils/logger.hpp](src/utils/logger.hpp) | Log centralizado | Instância única global acessível por todos |

### Princípios SOLID Aplicados

✅ **Single Responsibility**: Cada classe tem responsabilidade única e coesa  
✅ **Open/Closed**: Extensível via Strategy/Command sem modificar código existente  
✅ **Liskov Substitution**: Todas as estratégias são intercambiáveis  
✅ **Interface Segregation**: Interfaces pequenas e específicas (OCR, Storage, etc)  
✅ **Dependency Inversion**: Dependências via abstrações (interfaces)

## 📁 Estrutura Modular

O projeto segue **Clean Architecture** com camadas bem definidas:

```
src/
├── core/               # 🎯 Camada de Integração
│   └── fachada_ssmh.hpp/cpp     - Facade que orquestra os subsistemas
│
├── usuarios/           # 👤 Subsistema de Usuários
│   ├── domain/              - Entidades (Usuario, Fatura, TipoPerfil)
│   ├── storage/             - Persistência (Strategy: SQLite/Memória)
│   │   ├── armazenamento_strategy.hpp
│   │   ├── armazenamento_sqlite.hpp/cpp
│   │   └── armazenamento_volatil.hpp/cpp
│   ├── commands/            - Padrão Command (undo/redo)
│   │   ├── user_command.hpp
│   │   ├── user_commands.hpp/cpp
│   │   └── command_invoker.hpp/cpp
│   └── services/            - Lógica de negócio + Factory
│       ├── usuario_service.hpp/cpp
│       └── usuario_service_factory.hpp
│
├── monitoramento/      # 📊 Subsistema de Monitoramento
│   ├── domain/              - Entidade Leitura
│   ├── composite/           - Agregação de consumo
│   │   ├── consumo_monitoravel.hpp         (Component)
│   │   ├── consumo_hidrometro.hpp/cpp      (Leaf)
│   │   └── consumo_usuario.hpp/cpp         (Composite)
│   ├── adapter/             - Integração OCR
│   │   ├── processador_ocr.hpp             (Target)
│   │   └── adaptador_ocr.hpp/cpp           (Adapter)
│   ├── storage/             - Persistência de leituras (DAO)
│   └── services/            - Coordenação + Factory
│
├── alertas/            # 🚨 Subsistema de Alertas
│   ├── domain/              - Entidades (RegraAlerta, AlertaAtivo)
│   ├── strategies/          - Análise de consumo (Strategy)
│   │   ├── estrategia_analise_consumo.hpp
│   │   ├── limite_diario_strategy.hpp/cpp
│   │   ├── media_movel_strategy.hpp/cpp
│   │   └── deteccao_vazamento_strategy.hpp/cpp
│   ├── notifications/       - Canais de notificação (Strategy)
│   │   ├── notificacao_strategy.hpp
│   │   ├── notificacao_console_log.hpp/cpp
│   │   ├── notificacao_email.hpp/cpp
│   │   └── notificacao_windows_popup.hpp/cpp
│   ├── observers/           - Padrão Observer
│   │   ├── alert_observer.hpp
│   │   ├── painel_observer.hpp/cpp
│   │   ├── logger_observer.hpp/cpp
│   │   └── notificacao_observer.hpp/cpp
│   └── services/            - Coordenação + Factory
│       ├── alerta_service.hpp/cpp
│       └── alerta_service_factory.hpp/cpp
│
└── utils/              # 🔧 Utilitários Compartilhados
    ├── logger.hpp/cpp       - Sistema de log (Singleton)
    └── image.hpp/cpp        - Processamento de imagens
```

## 🔄 Comportamento dos Subsistemas

### 1. Subsistema de Usuários 👤

**Responsabilidade:** Gerenciar usuários, contas de água e hidrômetros vinculados.

**Funcionalidades:**
- ✅ CRUD completo de usuários
- ✅ Vinculação de hidrômetros a usuários
- ✅ Histórico de faturas
- ✅ Undo/Redo de operações (Command)
- ✅ Persistência em SQLite ou Memória (Strategy)

**Fluxo de Operação:**
```
Cliente → Fachada → CommandInvoker → UserCommand → UsuarioService → Storage
                                                         ↓
                                                      Logger (Singleton)
```

**Comportamento do Undo/Redo:**
- Cada operação (criar, atualizar, deletar) é encapsulada em um Command
- CommandInvoker mantém histórico de comandos executados
- `undo()` restaura estado anterior através de operação inversa
- `redo()` reaplica comando desfeito

**Persistência Strategy:**
- `ArmazenamentoSqlite`: Dados persistentes em banco de dados
- `ArmazenamentoVolatil`: Dados em memória (testes/desenvolvimento)
- Troca transparente através de Factory

---

### 2. Subsistema de Monitoramento 📊

**Responsabilidade:** Coletar e agregar dados de consumo de água.

**Funcionalidades:**
- ✅ Registro de leituras manuais
- ✅ Integração com OCR (Adapter)
- ✅ Agregação de consumo por hidrômetro ou usuário (Composite)
- ✅ Cálculo de consumo por período
- ✅ Persistência de leituras em DAO

**Fluxo de Operação:**
```
Cliente → Fachada → MonitoramentoService → LeituraDAO (storage)
                            ↓
                    ConsumoMonitoravel (Composite)
                            ↓
                    ┌──────┴──────┐
            ConsumoHidrometro   ConsumoUsuario
              (Leaf)            (Composite)
```

**Comportamento do Composite:**
- `ConsumoHidrometro` (Leaf): Calcula consumo de um hidrômetro específico
- `ConsumoUsuario` (Composite): Agrega múltiplos hidrômetros de um usuário
- Ambos implementam interface `ConsumoMonitoravel`
- Permite cálculo recursivo de consumo total

**Adaptador OCR:**
- Abstrai biblioteca externa de OCR (Tesseract, OpenCV, etc)
- Interface `ProcessadorOCR` define contrato
- `AdaptadorOCR` converte chamadas para biblioteca específica

---

### 3. Subsistema de Alertas 🚨

**Responsabilidade:** Monitorar consumo e notificar anomalias automaticamente.

**Funcionalidades:**
- ✅ Configuração de regras de alerta por usuário
- ✅ 3 estratégias de análise: Limite Diário, Média Móvel, Vazamento
- ✅ 3 canais de notificação: Console, Email (OAuth2), Windows Popup
- ✅ Notificação assíncrona via Observer
- ✅ Histórico de alertas disparados

**Fluxo de Operação:**
```
Cliente → Fachada → AlertaService → Verifica Regras
                                          ↓
                            EstrategiaAnaliseConsumo (Strategy)
                                          ↓
                            ┌─────────────┼─────────────┐
                    LimiteDiario   MediaMovel   Vazamento
                                          ↓
                            Dispara Alerta (Observer)
                                          ↓
                            ┌─────────────┼─────────────┐
                    PainelObserver  LoggerObserver  NotificacaoObserver
                                                            ↓
                                          NotificacaoStrategy (Strategy)
                                                            ↓
                                          ┌─────────────────┼────────────┐
                                    Console         Email         Popup
```

**Comportamento das Estratégias de Análise:**

1. **Limite Diário** (`limite_diario_strategy`):
   - Compara consumo atual com limite fixo configurado
   - Dispara alerta se `consumo > limite`
   - Uso: Controle de orçamento mensal

2. **Média Móvel** (`media_movel_strategy`):
   - Calcula média dos últimos N dias
   - Dispara se `consumo > média * fator` (ex: 1.5x)
   - Uso: Detectar mudanças de padrão

3. **Detecção de Vazamento** (`deteccao_vazamento_strategy`):
   - Monitora fluxo constante fora de horário esperado
   - Dispara se detectar consumo contínuo anormal
   - Uso: Identificar vazamentos ocultos

**Comportamento dos Observers:**
- `PainelObserver`: Atualiza UI em tempo real
- `LoggerObserver`: Registra evento no log
- `NotificacaoObserver`: Envia notificação via canal configurado

**Canais de Notificação (Strategy):**
- `NotificacaoConsoleLog`: Imprime no console
- `NotificacaoEmail`: Envia via Gmail com OAuth2
- `NotificacaoWindowsPopup`: Exibe notificação nativa do Windows

---

### 4. Fachada Central (Core) 🎯

**Responsabilidade:** Orquestrar comunicação entre os três subsistemas.

**Comportamento:**
```cpp
// Exemplo de fluxo coordenado pela Fachada:

// 1. Criar usuário
fachada.executarComandoUsuario(
    std::make_unique<CriarUsuarioCommand>(service, usuario)
);

// 2. Registrar leitura de consumo
fachada.registrarLeituraManual("SHA001", 150.5);

// 3. Configurar alerta
fachada.salvarRegra(usuarioId, "LIMITE_DIARIO", "100");

// 4. Sistema verifica automaticamente e notifica se necessário
fachada.verificarRegras(usuarioId, 150.5);  // Dispara alerta!
```

**Operações Orquestradas:**
1. Criar usuário → Vincular hidrômetro → Configurar alerta
2. Registrar leitura → Calcular consumo → Verificar regras → Notificar
3. Undo de operação → Sincronizar estado entre subsistemas

## 🚀 Como Usar o Sistema

### Compilação

```bash
# Compilar com configuração padrão
make

# Compilar em modo debug
make debug

# Limpar e recompilar
make clean && make
```

### Exemplos de Uso

#### 1. Demo Completo com Fachada
```bash
make demo-fachada
./demo_fachada
```

#### 2. Demo Interativo
```bash
make demo-interactive
./demo_interactive
```

#### 3. Testes Unitários
```bash
# Testar subsistema de usuários
make test-usuarios

# Testar persistência SQLite
make test-usuarios-db

# Testar subsistema de alertas
make test-alertas

# Testar subsistema de monitoramento
make test-monitoramento

# Testar notificações Windows
make test-popup
```

### Código de Exemplo

```cpp
#include "src/core/fachada_ssmh.hpp"
#include "src/usuarios/services/usuario_service_factory.hpp"
#include "src/monitoramento/services/monitoramento_service_factory.hpp"
#include "src/alertas/services/alerta_service_factory.hpp"

int main() {
    // 1. Criar serviços
    auto usuarioSvc = UsuarioServiceFactory::criarComSqlite();
    auto monitoramentoSvc = MonitoramentoServiceFactory::criar();
    auto alertaSvc = AlertaServiceFactory::criarPadrao();
    
    // 2. Instanciar Fachada
    FachadaSSMH fachada(usuarioSvc, monitoramentoSvc, alertaSvc);
    
    // 3. Criar usuário
    Usuario usuario(1, "João Silva", "joao@email.com", "12345678900", 
                    TipoPerfil::LEITOR);
    auto cmd = std::make_unique<CriarUsuarioCommand>(usuarioSvc, usuario);
    fachada.executarComandoUsuario(std::move(cmd));
    
    // 4. Vincular hidrômetro
    fachada.vincularHidrometroUsuario(1, "SHA001");
    
    // 5. Configurar alerta
    fachada.salvarRegra(1, "LIMITE_DIARIO", "100");
    
    // 6. Registrar leitura
    fachada.registrarLeituraManual("SHA001", 150.5);
    
    // 7. Verificar alertas (automático)
    fachada.verificarRegras(1, 150.5);  // Dispara alerta!
    
    return 0;
}
```

## 📋 Dependências e Requisitos

### Bibliotecas Necessárias

| Biblioteca | Versão | Uso |
|------------|--------|-----|
| **SQLite3** | 3.x | Persistência de dados |
| **Cairo** | 1.16+ | Renderização gráfica (opcional) |
| **libcurl** | 7.x | Envio de emails OAuth2 |
| **pkg-config** | - | Configuração de bibliotecas |

### Instalação no Windows (MSYS2)

```bash
# Atualizar pacotes
pacman -Syu

# Instalar dependências
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-sqlite3
pacman -S mingw-w64-x86_64-cairo
pacman -S mingw-w64-x86_64-curl
pacman -S pkg-config
```

### Configuração de Email (Opcional)

Para habilitar notificações por email:

1. Copie o template:
   ```bash
   cp config/email_config_example.hpp config/email_config.hpp
   ```

2. Configure suas credenciais OAuth2 (Gmail):
   - Siga o guia: [docs/GMAIL_OAUTH2_QUICKSTART.md](docs/GMAIL_OAUTH2_QUICKSTART.md)

3. Descomente no Makefile:
   ```makefile
   EMAIL_CONFIG_FLAG = -DEMAIL_CONFIG_EXISTS -I./config
   ```

---

## 🔧 Configuração de Simuladores

**Novo!** O projeto suporta **dois simuladores diferentes**:

1. **Hydrometer Project Simulator** - Completo com multi-threading e Cairo
2. **Simulador SHA** - Com Facade, CLI e OpenCV

### Trocar de Simulador

Edite o arquivo `simulator_config.mk`:

```makefile
# Opção 1: Simulador original
SIMULATOR_TYPE = hydrometer-project-simulator

# Opção 2: Simulador SHA
SIMULATOR_TYPE = simulador-hidrometro
```

**📖 Documentação completa:** [SIMULATORS_CONFIG.md](SIMULATORS_CONFIG.md)

---

## 📊 Estrutura de Dados

### Banco de Dados SQLite

**Tabela: usuarios**
```sql
CREATE TABLE usuarios (
    id INTEGER PRIMARY KEY,
    nome TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    cpf TEXT UNIQUE NOT NULL,
    tipo_perfil INTEGER NOT NULL,
    hidrometro_id TEXT
);
```

**Tabela: faturas**
```sql
CREATE TABLE faturas (
    id INTEGER PRIMARY KEY,
    usuario_id INTEGER NOT NULL,
    mes INTEGER NOT NULL,
    ano INTEGER NOT NULL,
    consumo REAL NOT NULL,
    valor REAL NOT NULL,
    FOREIGN KEY(usuario_id) REFERENCES usuarios(id)
);
```

**Tabela: regras_alertas**
```sql
CREATE TABLE regras_alertas (
    id INTEGER PRIMARY KEY,
    usuario_id INTEGER NOT NULL,
    tipo_estrategia TEXT NOT NULL,
    parametros TEXT NOT NULL,
    ativa BOOLEAN DEFAULT 1,
    FOREIGN KEY(usuario_id) REFERENCES usuarios(id)
);
```

**Tabela: alertas_ativos**
```sql
CREATE TABLE alertas_ativos (
    id INTEGER PRIMARY KEY,
    regra_id INTEGER NOT NULL,
    timestamp TEXT NOT NULL,
    mensagem TEXT NOT NULL,
    FOREIGN KEY(regra_id) REFERENCES regras_alertas(id)
);
```

---

## 🧪 Testes Disponíveis

| Teste | Comando | Descrição |
|-------|---------|-----------|
| **Usuários (Memória)** | `make test-usuarios` | Testa CRUD e undo/redo |
| **Usuários (SQLite)** | `make test-usuarios-db` | Testa persistência |
| **Alertas** | `make test-alertas` | Testa todas estratégias |
| **Monitoramento** | `make test-monitoramento` | Testa composite e OCR |
| **Popup Windows** | `make test-popup` | Testa notificação nativa |
| **Multi-threading** | `make test-multithread` | Testa concorrência |
| **Factory** | `make exemplo-factory` | Testa criação de serviços |

---

## 📖 Documentação Adicional

### Guias de Implementação
- [Subsistema de Usuários](docs/SUBSISTEMA_USUARIOS.md)
- [Subsistema de Monitoramento](docs/SUBSISTEMA_MONITORAMENTO.md)
- [Subsistema de Alertas Completo](docs/SUBSISTEMA_ALERTAS_COMPLETO.md)
- [Integração com Banco de Dados](docs/INTEGRACAO_BANCO_DADOS.md)

### Diagramas
- [Estrutura Modular](docs/diagrams/estrutura_modular.puml)
- [Facade SSMH](docs/diagrams/diagram_facadeSSMH.puml)
- [Sistema de Alertas](docs/diagrams/alert_diagram.puml)
- [Sistema de Usuários](docs/diagrams/users_diagram.puml)
- [OCR Adapter](docs/diagrams/ocr_diagram.puml)

### Configuração
- [Setup OAuth2 Gmail](docs/GMAIL_OAUTH2_QUICKSTART.md)
- [Configuração de Simuladores](SIMULATORS_CONFIG.md)
- [Multi-Simulador](IMPLEMENTACAO_MULTI_SIMULADOR.md)

## 🎓 Conceitos de Engenharia de Software Aplicados

### Princípios SOLID ✅
- **Single Responsibility**: Cada classe com responsabilidade única
- **Open/Closed**: Extensível sem modificar código existente
- **Liskov Substitution**: Estratégias totalmente intercambiáveis
- **Interface Segregation**: Interfaces coesas e específicas
- **Dependency Inversion**: Dependências via abstrações

### Clean Architecture ✅
- Camadas bem definidas (Domain → Storage → Services)
- Independência de frameworks
- Testabilidade em todos os níveis
- Separação clara de responsabilidades

### Design Patterns ✅
- **10 padrões** implementados e integrados
- Solução elegante para problemas recorrentes
- Código reutilizável e manutenível

---

## 📝 Convenções de Código

### Arquivos
- Headers: `.hpp`
- Implementações: `.cpp`
- Nomes: `snake_case` (ex: `usuario_service.hpp`)

### Classes e Interfaces
- Classes: `PascalCase` (ex: `UsuarioService`)
- Interfaces: Sufixo descritivo (ex: `Strategy`, `Command`, `Observer`)
- Abstratas: Prefixo descritivo quando necessário

### Diretórios
- Lowercase: `domain/`, `storage/`, `services/`
- Agrupados por responsabilidade funcional

---

## 🏆 Destaques do Projeto

✨ **Arquitetura Modular**: 3 subsistemas independentes e coesos  
✨ **10 Padrões de Projeto**: Implementados e integrados elegantemente  
✨ **SOLID Completo**: Todos os 5 princípios aplicados rigorosamente  
✨ **Clean Architecture**: Camadas bem definidas e testáveis  
✨ **Persistência Flexível**: SQLite ou Memória via Strategy  
✨ **Undo/Redo**: Sistema completo via Command Pattern  
✨ **Notificações Múltiplas**: Console, Email, Windows Popup  
✨ **Alertas Inteligentes**: 3 estratégias de análise de consumo  
✨ **Agregação de Dados**: Composite para cálculos hierárquicos  
✨ **Logging Centralizado**: Singleton para rastreamento global  

---

## 👨‍💻 Informações do Projeto

**Autor:** Marcos Belo  
**Última atualização:** 17/12/2025  
**Versão:** 3.0 - Sistema Completo e Integrado  
**Status:** ✅ 100% Concluído  

---

## 📄 Licença

Este projeto é parte de um trabalho acadêmico sobre Padrões de Projeto e Engenharia de Software.
