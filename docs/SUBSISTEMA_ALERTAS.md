# Subsistema de Alertas - Documentação Completa

## 📋 Resumo

Este subsistema implementa o sistema de alertas e notificações para o Sistema de Monitoramento de Hidrômetros (SSMH). Foi desenvolvido seguindo os padrões de projeto **Observer** e **Strategy** conforme especificado no documento de requisitos.

---

## 🎯 Funcionalidades Principais

### 1. Gerenciamento de Regras
- ✅ Criar regras de alerta por usuário
- ✅ Diferentes tipos de análise (Limite Diário, Média Móvel, Detecção de Vazamento)
- ✅ Ativar/desativar regras dinamicamente
- ✅ Consultar regras ativas

### 2. Verificação Automática
- ✅ Verificação de consumo em tempo real
- ✅ Detecção de violações de regras
- ✅ Análise inteligente baseada em estratégias
- ✅ Classificação automática de severidade

### 3. Sistema de Notificações
- ✅ Múltiplos canais (Console, Popup, Email)
- ✅ Troca dinâmica de canal em runtime
- ✅ Notificação automática de observers
- ✅ Formatação personalizada por canal

### 4. Histórico e Auditoria
- ✅ Registro de todos os alertas disparados
- ✅ Log automático via Logger Singleton
- ✅ Consulta de alertas por usuário/período
- ✅ Resolução e gerenciamento de alertas

---

## 🏗️ Arquitetura

### Estrutura de Diretórios

```
src/alertas/
├── domain/                      # Entidades de domínio
│   ├── regra_alerta.hpp/cpp         (Regra de alerta configurável)
│   └── alerta_ativo.hpp/cpp         (Alerta disparado)
│
├── strategies/                  # Padrão Strategy - Análise
│   ├── estrategia_analise_consumo.hpp     (Interface Strategy)
│   ├── limite_diario_strategy.hpp/cpp     (Estratégia de limite)
│   ├── media_movel_strategy.hpp/cpp       (Estratégia de média)
│   └── deteccao_vazamento_strategy.hpp/cpp (Estratégia de vazamento)
│
├── notifications/               # Padrão Strategy - Notificação
│   ├── notificacao_strategy.hpp           (Interface Strategy)
│   ├── notificacao_console_log.hpp/cpp    (Console)
│   ├── notificacao_windows_popup.hpp/cpp  (Windows Popup)
│   └── notificacao_email.hpp/cpp          (Email)
│
├── observers/                   # Padrão Observer
│   ├── alert_observer.hpp                 (Interface Observer)
│   ├── painel_observer.hpp/cpp            (Observer do Painel)
│   ├── logger_observer.hpp/cpp            (Observer de Log)
│   └── notificacao_observer.hpp/cpp       (Observer de Notificação)
│
└── services/                    # Serviços e Lógica de Negócio
    ├── alerta_service.hpp/cpp             (Serviço principal)
    └── alerta_service_factory.hpp/cpp     (Factory)
```

---

## 🎨 Padrões de Projeto Implementados

### 1. Padrão Observer

**Problema:** O sistema precisa notificar múltiplos componentes (painel, log, notificações) quando um alerta é disparado, sem criar acoplamento forte.

**Solução:** O `AlertaService` atua como **Subject** e mantém uma lista de **Observers** que são notificados automaticamente quando ocorre um evento.

```cpp
// Interface Observer
class AlertObserver {
public:
    virtual void atualizar(const AlertaAtivo& alerta) = 0;
    virtual std::string getNome() const = 0;
};

// Subject (AlertaService)
class AlertaService {
private:
    std::vector<std::shared_ptr<AlertObserver>> observers;
    
public:
    void anexarObserver(std::shared_ptr<AlertObserver> observer);
    void desanexarObserver(std::shared_ptr<AlertObserver> observer);
    void notificarObservers(const AlertaAtivo& alerta);
};
```

**Observers Concretos:**

1. **PainelObserver**: Atualiza a interface do painel com novos alertas
2. **LoggerObserver**: Registra alertas no arquivo de log do sistema
3. **NotificacaoObserver**: Envia notificações via canal configurado

**Benefícios:**
- ✅ Desacoplamento entre gerador e consumidores de alertas
- ✅ Fácil adição de novos observers sem modificar o service
- ✅ Notificação automática e síncrona
- ✅ Cada observer pode processar o alerta de forma independente

**Exemplo de Uso:**

```cpp
auto service = std::make_shared<AlertaService>();

// Anexa observers
service->anexarObserver(std::make_shared<PainelObserver>());
service->anexarObserver(std::make_shared<LoggerObserver>());
service->anexarObserver(std::make_shared<NotificacaoObserver>());

// Quando um alerta é disparado, todos são notificados automaticamente
service->verificarRegras(usuarioId, consumo);
```

---

### 2. Padrão Strategy (Análise de Consumo)

**Problema:** O sistema precisa suportar diferentes algoritmos de análise de consumo (limite fixo, média móvel, detecção de vazamento) e permitir que novos algoritmos sejam adicionados sem modificar o código existente.

**Solução:** Define uma interface `EstrategiaAnaliseConsumo` e implementa cada algoritmo como uma estratégia concreta.

```cpp
// Interface Strategy
class EstrategiaAnaliseConsumo {
public:
    virtual bool analisar(double consumoAtual, 
                         const std::string& valorParametro) const = 0;
    virtual std::string getNome() const = 0;
    virtual std::string gerarMensagem(double consumoAtual, 
                                     const std::string& valorParametro) const = 0;
};
```

**Estratégias Concretas:**

1. **LimiteDiarioStrategy**
   - Compara consumo com limite fixo
   - Exemplo: Alerta se consumo > 70L/dia

2. **MediaMovelStrategy**
   - Compara com média histórica
   - Exemplo: Alerta se consumo > 50% da média

3. **DeteccaoVazamentoStrategy**
   - Detecta fluxo constante anormal
   - Exemplo: Alerta se há 2L/h constante por 24h

**Benefícios:**
- ✅ Algoritmos intercambiáveis em runtime
- ✅ Fácil adição de novos algoritmos
- ✅ Código limpo e testável
- ✅ Cada estratégia é independente

**Exemplo de Uso:**

```cpp
auto service = std::make_shared<AlertaService>();

// Cria diferentes tipos de regras
service->salvarRegra(1, "LIMITE_DIARIO", "70");      // Limite fixo
service->salvarRegra(2, "MEDIA_MOVEL", "50");        // 50% acima da média
service->salvarRegra(3, "DETECCAO_VAZAMENTO", "24h"); // Vazamento
```

---

### 3. Padrão Strategy (Notificações)

**Problema:** O sistema precisa enviar notificações por diferentes canais (console, popup, email, SMS) e permitir troca dinâmica do canal.

**Solução:** Define uma interface `NotificacaoStrategy` e implementa cada canal como uma estratégia concreta.

```cpp
// Interface Strategy
class NotificacaoStrategy {
public:
    virtual bool enviar(const std::string& mensagem, 
                       const std::string& destinatario) = 0;
    virtual std::string getNomeCanal() const = 0;
    virtual bool isDisponivel() const = 0;
};
```

**Estratégias Concretas:**

1. **NotificacaoConsoleLog**: Imprime no console (útil para debug)
2. **NotificacaoWindowsPopup**: Popup do sistema operacional
3. **NotificacaoEmail**: Envio por SMTP (simulado)

**Benefícios:**
- ✅ Troca de canal em runtime sem restart
- ✅ Fácil adição de novos canais (SMS, Telegram, etc)
- ✅ Abstração da complexidade de cada canal
- ✅ Testável com mocks

**Exemplo de Uso:**

```cpp
auto service = std::make_shared<AlertaService>();

// Inicia com console
service->definirEstrategiaNotificacao(
    std::make_shared<NotificacaoConsoleLog>());

// Troca para email em runtime
service->definirEstrategiaNotificacao(
    std::make_shared<NotificacaoEmail>("smtp.exemplo.com", 587, "alertas@cagepa.com.br"));

// Troca para popup
service->definirEstrategiaNotificacao(
    std::make_shared<NotificacaoWindowsPopup>());
```

---

### 4. Padrão Factory

**Problema:** A criação do `AlertaService` envolve configurar múltiplos observers e strategies, tornando o código cliente complexo.

**Solução:** A `AlertaServiceFactory` encapsula diferentes configurações pré-definidas.

```cpp
class AlertaServiceFactory {
public:
    static std::shared_ptr<AlertaService> criarPadrao();
    static std::shared_ptr<AlertaService> criarParaTeste();
    static std::shared_ptr<AlertaService> criarParaProducao(...);
    static std::shared_ptr<AlertaService> criarMinimalista();
};
```

**Configurações Disponíveis:**

| Método | Notificação | Observers | Uso |
|--------|-------------|-----------|-----|
| `criarPadrao()` | Console | Painel, Logger, Notif | Desenvolvimento |
| `criarParaTeste()` | Console | Painel, Logger | Testes unitários |
| `criarParaProducao()` | Email | Todos | Produção |
| `criarMinimalista()` | Nenhuma | Nenhum | Casos específicos |

**Exemplo de Uso:**

```cpp
// Desenvolvimento
auto service = AlertaServiceFactory::criarPadrao();

// Produção
auto service = AlertaServiceFactory::criarParaProducao(
    "smtp.cagepa.com.br", 587, "alertas@cagepa.com.br");

// Testes
auto service = AlertaServiceFactory::criarParaTeste();
```

---

## 📊 Fluxo de Funcionamento

### Fluxo Completo de um Alerta

```
1. Usuário cria regra
   └─> AlertaService::salvarRegra()
       └─> Armazena RegraAlerta

2. Sistema verifica consumo (periódico ou sob demanda)
   └─> AlertaService::verificarRegras()
       ├─> Busca regras ativas do usuário
       ├─> Para cada regra:
       │   ├─> Obtém EstrategiaAnaliseConsumo correspondente
       │   ├─> Executa estrategia->analisar()
       │   └─> Se violação detectada:
       │       ├─> Cria AlertaAtivo
       │       ├─> Determina severidade
       │       └─> Chama notificarObservers()
       │
       └─> notificarObservers()
           ├─> PainelObserver::atualizar()      → Atualiza UI
           ├─> LoggerObserver::atualizar()      → Grava em log
           └─> NotificacaoObserver::atualizar() → Envia notificação
               └─> NotificacaoStrategy::enviar()
```

---

## 💻 Como Usar

### Exemplo Completo

```cpp
#include "src/alertas/services/alerta_service_factory.hpp"

int main() {
    // 1. Cria o service
    auto alertaService = AlertaServiceFactory::criarPadrao();
    
    // 2. Configura regras para usuários
    alertaService->salvarRegra(100, "LIMITE_DIARIO", "70");
    alertaService->salvarRegra(101, "MEDIA_MOVEL", "50");
    alertaService->salvarRegra(102, "DETECCAO_VAZAMENTO", "24h");
    
    // 3. Verifica consumo (chamado periodicamente ou em tempo real)
    bool alertouUsuario100 = alertaService->verificarRegras(100, 85.0);
    
    // 4. Consulta alertas ativos
    auto alertas = alertaService->buscarAlertasAtivos();
    for (const auto& alerta : alertas) {
        std::cout << alerta.toString() << std::endl;
    }
    
    // 5. Resolve um alerta
    if (!alertas.empty()) {
        alertaService->resolverAlerta(alertas[0].getId());
    }
    
    // 6. Troca estratégia de notificação em runtime
    alertaService->definirEstrategiaNotificacao(
        std::make_shared<NotificacaoEmail>("smtp.exemplo.com", 587, "alertas@cagepa.com.br")
    );
    
    return 0;
}
```

---

## 🧪 Compilação e Teste

### Compilar o Teste

```bash
g++ -std=c++17 -I. test_alertas.cpp \
    src/alertas/domain/*.cpp \
    src/alertas/strategies/*.cpp \
    src/alertas/notifications/*.cpp \
    src/alertas/observers/*.cpp \
    src/alertas/services/*.cpp \
    src/utils/logger.cpp \
    -o test_alertas

./test_alertas
```

### Integração com Makefile

Adicione ao Makefile existente:

```makefile
# Subsistema de Alertas
ALERTA_SOURCES = src/alertas/domain/*.cpp \
                 src/alertas/strategies/*.cpp \
                 src/alertas/notifications/*.cpp \
                 src/alertas/observers/*.cpp \
                 src/alertas/services/*.cpp

test_alertas: test_alertas.cpp $(ALERTA_SOURCES)
	$(CXX) $(CXXFLAGS) $^ src/utils/logger.cpp -o $@
```

---

## 🔧 Extensibilidade

### Adicionar Nova Estratégia de Análise

```cpp
// 1. Criar nova estratégia
class PicoConsumoStrategy : public EstrategiaAnaliseConsumo {
public:
    bool analisar(double consumoAtual, const std::string& valorParametro) const override {
        // Implementar lógica
        return consumoAtual > std::stod(valorParametro) * 2;
    }
    
    std::string getNome() const override {
        return "PICO_CONSUMO";
    }
    
    std::string gerarMensagem(double consumoAtual, const std::string& valorParametro) const override {
        return "Pico de consumo detectado: " + std::to_string(consumoAtual) + "L";
    }
};

// 2. Registrar no service
service->registrarEstrategiaAnalise("PICO_CONSUMO", 
                                    std::make_shared<PicoConsumoStrategy>());

// 3. Usar
service->salvarRegra(userId, "PICO_CONSUMO", "100");
```

### Adicionar Novo Canal de Notificação

```cpp
// 1. Implementar interface
class NotificacaoSMS : public NotificacaoStrategy {
public:
    bool enviar(const std::string& mensagem, const std::string& destinatario) override {
        // Integrar com API de SMS
        return true;
    }
    
    std::string getNomeCanal() const override {
        return "SMS";
    }
    
    bool isDisponivel() const override {
        return true;
    }
};

// 2. Configurar
service->definirEstrategiaNotificacao(std::make_shared<NotificacaoSMS>());
```

### Adicionar Novo Observer

```cpp
// 1. Implementar interface
class DatabaseObserver : public AlertObserver {
public:
    void atualizar(const AlertaAtivo& alerta) override {
        // Persistir alerta no banco de dados
    }
    
    std::string getNome() const override {
        return "DatabaseObserver";
    }
};

// 2. Anexar
service->anexarObserver(std::make_shared<DatabaseObserver>());
```

---

## 📈 Integração com Outros Subsistemas

### Com Subsistema de Monitoramento

```cpp
// Quando uma leitura é registrada
void MonitoramentoService::registrarLeitura(int hidrometroId, double valor) {
    // ... registra leitura ...
    
    // Obtém usuário do hidrômetro
    int usuarioId = obterUsuarioDoHidrometro(hidrometroId);
    
    // Calcula consumo diário
    double consumoDiario = calcularConsumoDia(usuarioId);
    
    // Verifica alertas
    alertaService->verificarRegras(usuarioId, consumoDiario);
}
```

### Com Subsistema de Usuários

```cpp
// Ao criar usuário, criar regra padrão
void UsuarioService::criarUsuario(const Usuario& usuario) {
    // ... cria usuário ...
    
    // Configura alerta padrão de 70L/dia
    alertaService->salvarRegra(usuario.getId(), "LIMITE_DIARIO", "70");
}
```

---

## 📝 Documentação das Classes Principais

### AlertaService

**Responsabilidade:** Coordenador central do subsistema de alertas

**Métodos Principais:**
- `salvarRegra()`: Cria nova regra de alerta
- `verificarRegras()`: Verifica violações de consumo
- `anexarObserver()`: Registra observer
- `notificarObservers()`: Notifica todos os observers
- `buscarAlertasAtivos()`: Retorna alertas ativos

### RegraAlerta

**Responsabilidade:** Representa uma regra configurada

**Atributos:**
- `id`: Identificador único
- `usuarioId`: ID do usuário
- `tipoEstrategia`: Tipo de análise
- `valorParametro`: Parâmetro da regra
- `ativo`: Se a regra está ativa

### AlertaAtivo

**Responsabilidade:** Representa um alerta disparado

**Atributos:**
- `id`: Identificador único
- `usuarioId`: ID do usuário
- `mensagem`: Mensagem descritiva
- `dataDisparo`: Quando foi disparado
- `status`: ATIVO, RESOLVIDO, IGNORADO
- `severidade`: BAIXA, MEDIA, ALTA, CRITICA

---

## 🎓 Conceitos de Design

### Por que Observer + Strategy?

1. **Observer** permite notificação desacoplada
   - Múltiplos componentes reagem ao mesmo evento
   - Fácil adicionar/remover observers

2. **Strategy** permite algoritmos intercambiáveis
   - Análise: Diferentes formas de detectar problemas
   - Notificação: Diferentes canais de comunicação

3. **Juntos** criam sistema flexível e extensível
   - Novas regras sem alterar código existente
   - Novos canais sem afetar lógica de negócio
   - Testabilidade alta (mock de strategies)

---

## ✅ Checklist de Implementação

- [x] Entidades de domínio (RegraAlerta, AlertaAtivo)
- [x] Interface e estratégias de análise (3 tipos)
- [x] Interface e estratégias de notificação (3 canais)
- [x] Interface Observer e 3 observers concretos
- [x] AlertaService completo (Subject do Observer)
- [x] Factory com múltiplas configurações
- [x] Teste completo com 10 cenários
- [x] Documentação detalhada
- [x] Integração com Logger (Singleton)
- [x] Gerenciamento de severidade automático

---

## 📚 Referências

- Gamma et al. "Design Patterns: Elements of Reusable Object-Oriented Software"
- Padrão Observer: Notificação de dependentes
- Padrão Strategy: Algoritmos intercambiáveis
- Especificação SSMH - Seção 4.3 (Subsistema de Alertas)

---

**Desenvolvido para o Sistema de Monitoramento de Hidrômetros (SSMH)**  
**Padrões: Observer + Strategy**  
**Linguagem: C++17**
