# Subsistema de Alertas

## 🎯 Visão Geral

Este é o **Subsistema de Alertas** do Sistema de Monitoramento de Hidrômetros (SSMH). Implementa monitoramento inteligente de consumo de água com notificações automáticas quando regras são violadas.

## 🏗️ Padrões de Projeto

- **Observer**: Notificação automática de múltiplos componentes
- **Strategy**: Algoritmos intercambiáveis de análise e notificação
- **Factory**: Criação simplificada com configurações pré-definidas
- **Singleton**: Integração com sistema de log centralizado

## 📁 Estrutura

```
alertas/
├── domain/          # Entidades (RegraAlerta, AlertaAtivo)
├── strategies/      # Estratégias de análise de consumo
├── notifications/   # Estratégias de notificação
├── observers/       # Observers (Painel, Logger, Notificação)
└── services/        # Lógica de negócio e Factory
```

## ⚡ Quick Start

```cpp
#include "services/alerta_service_factory.hpp"

// Criar service
auto service = AlertaServiceFactory::criarPadrao();

// Criar regra
service->salvarRegra(usuarioId, "LIMITE_DIARIO", "70");

// Verificar consumo
bool alertou = service->verificarRegras(usuarioId, 85.0);

// Consultar alertas
auto alertas = service->buscarAlertasAtivos();
```

## 🔧 Funcionalidades

### Estratégias de Análise
- **LIMITE_DIARIO**: Alerta quando consumo > limite fixo
- **MEDIA_MOVEL**: Alerta quando consumo excede média histórica
- **DETECCAO_VAZAMENTO**: Detecta fluxo constante anormal

### Canais de Notificação
- **Console**: Log formatado no terminal
- **Windows Popup**: Notificação do sistema operacional
- **Email**: Envio via SMTP (simulado)

### Observers
- **PainelObserver**: Atualiza interface gráfica
- **LoggerObserver**: Registra em arquivo de log
- **NotificacaoObserver**: Envia via canal configurado

## 📖 Documentação Completa

Veja [SUBSISTEMA_ALERTAS.md](../../docs/SUBSISTEMA_ALERTAS.md) para documentação detalhada.

## 🧪 Teste

```bash
make test_alertas
./test_alertas
```

## 🎓 Exemplos de Uso

### Exemplo 1: Configuração Básica

```cpp
auto service = AlertaServiceFactory::criarPadrao();
service->salvarRegra(1, "LIMITE_DIARIO", "70");
service->verificarRegras(1, 85.0); // Dispara alerta
```

### Exemplo 2: Troca Dinâmica de Canal

```cpp
auto service = AlertaServiceFactory::criarPadrao();

// Inicia com console
service->verificarRegras(userId, consumo);

// Troca para email
service->definirEstrategiaNotificacao(
    std::make_shared<NotificacaoEmail>("smtp.exemplo.com", 587, "alertas@cagepa.com.br")
);

service->verificarRegras(userId, consumo); // Agora envia email
```

### Exemplo 3: Observers Customizados

```cpp
auto service = std::make_shared<AlertaService>();

// Cria observers personalizados
auto meuObserver = std::make_shared<MeuObserverCustomizado>();
service->anexarObserver(meuObserver);

// Observer será notificado automaticamente
service->verificarRegras(userId, consumo);
```

## 🔌 Integração

### Com Monitoramento

```cpp
// Após registrar leitura
monitoramentoService->registrarLeitura(hidrometroId, valor);

// Verificar alertas
double consumoDiario = monitoramentoService->calcularConsumoDia(usuarioId);
alertaService->verificarRegras(usuarioId, consumoDiario);
```

### Com Usuários

```cpp
// Ao criar usuário, configurar alerta padrão
usuarioService->criarUsuario(usuario);
alertaService->salvarRegra(usuario.getId(), "LIMITE_DIARIO", "70");
```

## 🚀 Extensibilidade

### Nova Estratégia de Análise

```cpp
class MinhaEstrategia : public EstrategiaAnaliseConsumo {
    bool analisar(double consumo, const std::string& param) const override {
        // Sua lógica aqui
    }
};

service->registrarEstrategiaAnalise("MINHA_ESTRATEGIA", 
                                    std::make_shared<MinhaEstrategia>());
```

### Novo Canal de Notificação

```cpp
class MeuCanal : public NotificacaoStrategy {
    bool enviar(const std::string& msg, const std::string& dest) override {
        // Sua lógica de envio
    }
};

service->definirEstrategiaNotificacao(std::make_shared<MeuCanal>());
```

## 📊 Estatísticas

```cpp
std::cout << service->getEstatisticas() << std::endl;
```

Saída:
```
=== Estatísticas do Sistema de Alertas ===
Regras cadastradas: 15
Regras ativas: 12
Alertas ativos: 3
Total de alertas: 8
Observers registrados: 3
Estratégias de análise: 3
```

## 🎯 Benefícios dos Padrões

### Observer
- ✅ Desacoplamento entre alertas e notificações
- ✅ Fácil adicionar novos observers
- ✅ Notificação automática e síncrona

### Strategy
- ✅ Algoritmos intercambiáveis
- ✅ Código limpo e testável
- ✅ Extensível sem modificar código existente

### Factory
- ✅ Criação simplificada
- ✅ Configurações pré-definidas
- ✅ Reduz complexidade no código cliente

---

**Para mais informações, consulte a documentação completa em `docs/SUBSISTEMA_ALERTAS.md`**
