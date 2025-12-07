# Subsistema de Coleta e Monitoramento - Documentação Completa

## 📋 Resumo

Este subsistema implementa a coleta e monitoramento de consumo de água para o Sistema de Monitoramento de Hidrômetros (SSMH). Foi desenvolvido seguindo os padrões de projeto **Adapter** e **Composite** conforme especificado no documento de requisitos.

---

## 🎯 Funcionalidades Principais

### 1. Coleta de Dados
- ✅ Processamento de imagens via OCR (Padrão Adapter)
- ✅ Registro manual de leituras
- ✅ Persistência em memória ou banco de dados

### 2. Monitoramento de Consumo
- ✅ Consulta de consumo individual por hidrômetro
- ✅ Consulta de consumo agregado por usuário
- ✅ Agregação transparente via Padrão Composite
- ✅ Consultas por período de tempo

### 3. Histórico e Análise
- ✅ Armazenamento de histórico de leituras
- ✅ Cálculo de consumo recente
- ✅ Consultas detalhadas por intervalo

---

## 🏗️ Arquitetura

### Estrutura de Diretórios

```
src/monitoramento/
├── domain/                  # Entidades de domínio
│   ├── leitura.hpp
│   └── leitura.cpp
│
├── composite/               # Padrão Composite
│   ├── consumo_monitoravel.hpp      (Interface Component)
│   ├── consumo_hidrometro.hpp       (Leaf)
│   ├── consumo_hidrometro.cpp
│   ├── consumo_usuario.hpp          (Composite)
│   └── consumo_usuario.cpp
│
├── adapter/                 # Padrão Adapter
│   ├── processador_ocr.hpp          (Target Interface)
│   ├── adaptador_ocr.hpp            (Adapter)
│   └── adaptador_ocr.cpp
│
├── storage/                 # Camada de Persistência
│   ├── leitura_dao.hpp              (Interface DAO)
│   ├── leitura_dao_memoria.hpp      (Implementação em memória)
│   └── leitura_dao_memoria.cpp
│
└── services/                # Serviços e Lógica de Negócio
    ├── monitoramento_service.hpp
    ├── monitoramento_service.cpp
    └── monitoramento_service_factory.hpp (Factory)
```

---

## 🎨 Padrões de Projeto Implementados

### 1. Padrão Adapter

**Problema:** O sistema precisa processar imagens de hidrômetros usando OCR, mas não pode depender diretamente de uma biblioteca OCR específica.

**Solução:** O padrão Adapter converte a interface da biblioteca OCR externa para a interface esperada pelo sistema.

```cpp
// Interface esperada pelo sistema (Target)
class ProcessadorOCR {
public:
    virtual int extrairNumeros(const std::string& caminhoImagem) = 0;
    virtual bool validarCaminho(const std::string& caminhoImagem) = 0;
};

// Adapter que adapta a biblioteca externa
class AdaptadorOCR : public ProcessadorOCR {
public:
    int extrairNumeros(const std::string& caminhoImagem) override;
    bool validarCaminho(const std::string& caminhoImagem) override;
private:
    int simularExtracao(const std::string& caminhoImagem);
    int extrairValorDoNome(const std::string& nomeArquivo);
};
```

**Benefícios:**
- ✅ Desacoplamento da biblioteca OCR específica
- ✅ Facilita testes (mock da interface)
- ✅ Permite trocar implementação OCR sem afetar o sistema
- ✅ Interface simplificada e padronizada

---

### 2. Padrão Composite

**Problema:** O sistema precisa calcular consumo tanto para um hidrômetro individual quanto para um conjunto de hidrômetros de um usuário, de forma uniforme.

**Solução:** O padrão Composite permite tratar objetos individuais e composições de objetos de maneira uniforme.

```cpp
// Component (Interface base)
class ConsumoMonitoravel {
public:
    virtual double calcularConsumo(time_t dataInicio, time_t dataFim) = 0;
    virtual std::string obterIdentificador() const = 0;
    virtual std::string obterDescricao() const = 0;
};

// Leaf (Hidrômetro individual)
class ConsumoHidrometro : public ConsumoMonitoravel {
private:
    std::string idSha_;
    std::shared_ptr<LeituraDAO> repositorio_;
public:
    double calcularConsumo(time_t dataInicio, time_t dataFim) override;
};

// Composite (Agregação de hidrômetros de um usuário)
class ConsumoUsuario : public ConsumoMonitoravel {
private:
    int idUsuario_;
    std::vector<std::shared_ptr<ConsumoMonitoravel>> hidrometros_;
public:
    void adicionarHidrometro(std::shared_ptr<ConsumoMonitoravel> hidrometro);
    double calcularConsumo(time_t dataInicio, time_t dataFim) override;
};
```

**Benefícios:**
- ✅ Interface uniforme para consulta de consumo
- ✅ Agregação transparente e automática
- ✅ Fácil adicionar novos níveis de agregação
- ✅ Código cliente simplificado

**Exemplo de uso:**

```cpp
// Mesma interface para ambos!
auto consumoHidrometro = servico->construirConsumoHidrometro("SHA001");
auto consumoUsuario = servico->construirConsumoUsuario(42, {"SHA001", "SHA002"});

// Ambos respondem ao mesmo método
double consumo1 = consumoHidrometro->calcularConsumo(inicio, fim);  // 50L
double consumo2 = consumoUsuario->calcularConsumo(inicio, fim);     // 150L (soma automática)
```

---

### 3. Padrão Factory

**Finalidade:** Simplificar a criação do MonitoramentoService com suas dependências.

```cpp
class MonitoramentoServiceFactory {
public:
    enum class TipoArmazenamento {
        MEMORIA,    // Para testes
        SQLITE      // Para produção (futuro)
    };
    
    static std::shared_ptr<MonitoramentoService> criar(
        TipoArmazenamento tipo = TipoArmazenamento::MEMORIA);
};

// Uso:
auto servico = MonitoramentoServiceFactory::criar();
```

---

## 💾 Camada de Persistência

### Interface LeituraDAO

Define o contrato para persistência de leituras:

```cpp
class LeituraDAO {
public:
    virtual bool salvarLeitura(const Leitura& leitura) = 0;
    virtual Leitura buscarLeitura(int id) = 0;
    virtual std::vector<Leitura> consultarLeituras(...) = 0;
    virtual double consultarConsumo(...) = 0;
    virtual double consultarConsumoAgregado(...) = 0;
    virtual int removerLeituras(const std::string& idSha) = 0;
    virtual int contarLeituras(const std::string& idSha) = 0;
};
```

### Implementação em Memória

A classe `LeituraDAOMemoria` implementa o DAO usando estruturas STL:

```cpp
class LeituraDAOMemoria : public LeituraDAO {
private:
    std::map<int, Leitura> leituras_;
    std::map<std::string, std::vector<int>> leiturasporSha_;
    int proximoId_;
    std::mutex mutex_;  // Thread-safe
};
```

**Características:**
- ✅ Thread-safe (usa mutex)
- ✅ Rápido para testes e protótipos
- ✅ Sem dependências externas
- ✅ Implementação completa da interface

---

## 🎯 MonitoramentoService

O serviço principal coordena todas as operações:

### Responsabilidades

1. **Processamento de Imagens**
   ```cpp
   int processarLeitura(const std::string& idSha, const std::string& caminhoImagem);
   ```
   - Usa o OCR para extrair valor da imagem
   - Cria e persiste a leitura

2. **Registro Manual**
   ```cpp
   int registrarLeituraManual(const std::string& idSha, int valor);
   ```
   - Registra leituras sem processamento OCR

3. **Construção de Composite**
   ```cpp
   std::shared_ptr<ConsumoMonitoravel> construirConsumoHidrometro(const std::string& idSha);
   std::shared_ptr<ConsumoMonitoravel> construirConsumoUsuario(int idUsuario, 
       const std::vector<std::string>& listaShas);
   ```

4. **Consultas de Consumo**
   ```cpp
   double consultarConsumo(std::shared_ptr<ConsumoMonitoravel> monitoravel, ...);
   double consultarConsumoHidrometro(const std::string& idSha, ...);
   double consultarConsumoAgregado(const std::vector<std::string>& listaShas, ...);
   double calcularConsumoRecente(const std::string& idSha, int periodoHoras);
   ```

---

## 📊 Fluxo de Dados

### 1. Processamento de Leitura via OCR

```
Imagem → AdaptadorOCR → Valor Numérico → Leitura → LeituraDAO → Persistência
```

### 2. Consulta de Consumo Individual

```
Cliente → MonitoramentoService → ConsumoHidrometro → LeituraDAO → Cálculo → Resultado
```

### 3. Consulta de Consumo Agregado

```
Cliente → MonitoramentoService → ConsumoUsuario
                                     ├→ ConsumoHidrometro 1 → LeituraDAO
                                     ├→ ConsumoHidrometro 2 → LeituraDAO
                                     └→ ConsumoHidrometro N → LeituraDAO
                                     → Soma Automática → Resultado
```

---

## 🧪 Testes

### Arquivo de Teste

`test_monitoramento.cpp` - Demonstra todas as funcionalidades

### Executar Testes

```bash
make test-monitoramento
```

### Cobertura dos Testes

1. ✅ **Teste 1:** Processamento OCR (Padrão Adapter)
2. ✅ **Teste 2:** Registro de Leituras Manuais
3. ✅ **Teste 3:** Consumo de Hidrômetro Individual (Composite - Leaf)
4. ✅ **Teste 4:** Consumo de Usuário (Composite - Composite)
5. ✅ **Teste 5:** Consultas Avançadas
6. ✅ **Teste 6:** Integração dos Padrões

---

## 🔗 Integração com Outros Subsistemas

### Integração com Subsistema de Usuários

O MonitoramentoService precisa da lista de hidrômetros de um usuário:

```cpp
// Obtém do UsuarioService
auto hidrometros = usuarioService->listarHidrometrosPorUsuario(idUsuario);

// Constrói o Composite
auto consumoUsuario = monitoramentoService->construirConsumoUsuario(idUsuario, hidrometros);
```

### Integração com Subsistema de Alertas (Futuro)

O subsistema de alertas pode usar:

```cpp
// Calcula consumo recente para verificar regras
double consumoRecente = monitoramentoService->calcularConsumoRecente("SHA001", 24);

if (consumoRecente > limiteUsuario) {
    alertaService->dispararAlerta(idUsuario, consumoRecente);
}
```

---

## 📈 Métricas do Subsistema

- **Arquivos criados:** 16 arquivos (.hpp e .cpp)
- **Classes implementadas:** 8 classes
- **Padrões de projeto:** 3 (Adapter, Composite, Factory)
- **Linhas de código:** ~1800 linhas
- **Tempo de execução dos testes:** < 1 segundo

---

## 🎨 Diagramas

### Diagrama de Classes

Consulte: `docs/diagrams/ocr_diagram.puml`

---

## 🚀 Uso na Fachada

O subsistema será usado pela Fachada da seguinte forma:

```cpp
class FachadaSSMH {
private:
    std::shared_ptr<MonitoramentoService> monitoramentoService_;
    
public:
    // Método unificado de monitoramento (Composite permite isso!)
    double monitorarConsumo(
        std::shared_ptr<ConsumoMonitoravel> monitoravel,
        time_t dataInicio,
        time_t dataFim) {
        
        return monitoramentoService_->consultarConsumo(
            monitoravel, dataInicio, dataFim);
    }
    
    // Construção facilitada
    std::shared_ptr<ConsumoMonitoravel> obterMonitoravelUsuario(int idUsuario) {
        auto hidrometros = usuarioService_->listarHidrometrosPorUsuario(idUsuario);
        return monitoramentoService_->construirConsumoUsuario(idUsuario, hidrometros);
    }
    
    std::shared_ptr<ConsumoMonitoravel> obterMonitoravelHidrometro(
        const std::string& idSha) {
        return monitoramentoService_->construirConsumoHidrometro(idSha);
    }
};
```

---

## ✅ Checklist de Implementação

- [x] Entidade Leitura (domínio)
- [x] Interface ConsumoMonitoravel (Component)
- [x] ConsumoHidrometro (Leaf)
- [x] ConsumoUsuario (Composite)
- [x] Interface ProcessadorOCR (Target)
- [x] AdaptadorOCR (Adapter)
- [x] Interface LeituraDAO
- [x] LeituraDAOMemoria
- [x] MonitoramentoService
- [x] MonitoramentoServiceFactory
- [x] Testes completos
- [x] Documentação
- [x] Integração no Makefile

---

## 🔮 Próximos Passos

### Curto Prazo
- [ ] Implementar LeituraDAOSqlite para persistência em banco
- [ ] Integrar com biblioteca OCR real (Tesseract)
- [ ] Adicionar cache para consultas frequentes

### Médio Prazo
- [ ] Implementar subsistema de Alertas
- [ ] Criar Fachada integrando todos os subsistemas
- [ ] Interface gráfica para visualização de consumo

### Longo Prazo
- [ ] Dashboard web com gráficos
- [ ] API REST
- [ ] Exportação de relatórios

---

## 📚 Referências

- **Design Patterns:** Gang of Four (GoF)
- **Clean Architecture:** Robert C. Martin
- **Especificação do projeto:** README.md principal

---

**Autor:** Marcos Belo  
**Data:** 07/12/2025  
**Versão:** 1.0 - Implementação completa do subsistema de monitoramento
