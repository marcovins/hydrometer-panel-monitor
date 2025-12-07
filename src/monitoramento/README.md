# Subsistema de Coleta e Monitoramento

## 📂 Estrutura

```
monitoramento/
├── domain/           # Entidades de domínio
├── composite/        # Padrão Composite (agregação de consumo)
├── adapter/          # Padrão Adapter (OCR)
├── storage/          # Camada de persistência (DAO)
└── services/         # Serviços e lógica de negócio
```

## 🎯 Componentes Principais

### Domain (Entidades)
- **Leitura:** Representa uma leitura de hidrômetro

### Composite (Padrão de Projeto)
- **ConsumoMonitoravel:** Interface Component
- **ConsumoHidrometro:** Leaf (hidrômetro individual)
- **ConsumoUsuario:** Composite (agregação de hidrômetros)

### Adapter (Padrão de Projeto)
- **ProcessadorOCR:** Interface Target
- **AdaptadorOCR:** Adapter que converte biblioteca OCR externa

### Storage (Persistência)
- **LeituraDAO:** Interface de persistência
- **LeituraDAOMemoria:** Implementação em memória

### Services
- **MonitoramentoService:** Coordena todas as operações
- **MonitoramentoServiceFactory:** Factory para criar o serviço

## 🔧 Uso Básico

```cpp
#include "services/monitoramento_service_factory.hpp"

// Cria o serviço
auto servico = MonitoramentoServiceFactory::criar();

// Registra leituras
servico->registrarLeituraManual("SHA001", 100);
servico->registrarLeituraManual("SHA001", 150);

// Consulta consumo individual
auto consumoHidrometro = servico->construirConsumoHidrometro("SHA001");
double consumo = consumoHidrometro->calcularConsumo(dataInicio, dataFim);

// Consulta consumo agregado
auto consumoUsuario = servico->construirConsumoUsuario(42, {"SHA001", "SHA002"});
double total = consumoUsuario->calcularConsumo(dataInicio, dataFim);
```

## 📚 Documentação Completa

Veja: `docs/SUBSISTEMA_MONITORAMENTO.md`

## 🧪 Testes

```bash
make test-monitoramento
```
