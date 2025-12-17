# 🔧 Configuração dos Simuladores

Este projeto suporta **dois simuladores diferentes** de hidrômetro, cada um com suas características e vantagens. Você pode facilmente alternar entre eles através de uma configuração centralizada.

## 📋 Simuladores Disponíveis

### 1. Hydrometer Project Simulator (Padrão)
**Características:**
- ✅ Simulador completo com multi-threading
- ✅ Visualização gráfica em tempo real (Cairo)
- ✅ Cálculos hidráulicos avançados (Darcy-Weisbach)
- ✅ Controle interativo de vazão (setas do teclado)
- ✅ Logger avançado com múltiplos níveis
- ✅ Suporte a múltiplas instâncias simultâneas

**Diretório:** `src/simulator/hydrometer-project-simulator/`

**Dependências:**
- Cairo Graphics Library
- pthread (multi-threading)

### 2. Simulador SHA (Fachada + CLI)
**Características:**
- ✅ Interface CLI interativa
- ✅ Padrão Facade para gerenciamento de instâncias
- ✅ Processamento de imagens com OpenCV
- ✅ Display visual das leituras
- ✅ Controle de vazão programático
- ✅ Geração de imagens configurável

**Diretório:** `src/simulator/simulador-hidrometro/`

**Dependências:**
- OpenCV 4
- pthread (multi-threading)

---

## ⚙️ Como Trocar de Simulador

### Método 1: Editando o arquivo de configuração (Recomendado)

1. Abra o arquivo `simulator_config.mk` na raiz do projeto:
   ```bash
   nano simulator_config.mk
   ```

2. Altere a linha `SIMULATOR_TYPE`:
   ```makefile
   # Para usar o Hydrometer Project Simulator:
   SIMULATOR_TYPE = hydrometer-project-simulator
   
   # OU
   
   # Para usar o Simulador SHA:
   SIMULATOR_TYPE = simulador-hidrometro
   ```

3. Salve o arquivo e recompile:
   ```bash
   make clean
   make
   ```

### Método 2: Via linha de comando

Você pode sobrescrever a configuração diretamente no comando make:

```bash
# Compilar com Hydrometer Project Simulator
make clean
make SIMULATOR_TYPE=hydrometer-project-simulator

# Compilar com Simulador SHA
make clean
make SIMULATOR_TYPE=simulador-hidrometro
```

---

## 🚀 Como Usar

### Executando o Simulador Principal

```bash
# Compilar e executar
make build-run

# Ou separadamente
make
make run
```

### Verificar Qual Simulador Está Configurado

Quando você executa `make`, o sistema exibirá automaticamente:
```
========================================
Configuração do Simulador
========================================
Simulador selecionado: hydrometer-project-simulator
Diretório base: src/simulator/hydrometer-project-simulator
Arquivos fonte: 4 arquivo(s)
Bibliotecas: -pthread [cairo libs]
========================================
```

### Testando Ambos os Simuladores

```bash
# Testar Hydrometer Project Simulator
make clean
make SIMULATOR_TYPE=hydrometer-project-simulator
./hidrometer_simulator

# Testar Simulador SHA
make clean
make SIMULATOR_TYPE=simulador-hidrometro
./hidrometer_simulator
```

---

## 📦 Instalando Dependências

### Para Hydrometer Project Simulator

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libcairo2-dev pkg-config
```

**Fedora/RHEL:**
```bash
sudo dnf install cairo-devel pkgconfig
```

**macOS:**
```bash
brew install cairo pkg-config
```

### Para Simulador SHA

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libopencv-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install opencv-devel
```

**macOS:**
```bash
brew install opencv
```

---

## 🔍 Detalhes Técnicos

### Estrutura do Código

O projeto usa um sistema de **wrapper** para abstrair as diferenças entre os simuladores:

- **`simulator_config.mk`**: Arquivo de configuração principal
- **`src/simulator_wrapper.hpp`**: Header que seleciona o simulador correto
- **`main.cpp`**: Adaptado para suportar ambos os simuladores

### Flags de Compilação

O sistema define automaticamente as flags apropriadas:

- `USE_HYDROMETER_PROJECT_SIMULATOR`: Quando usar o primeiro simulador
- `USE_SIMULADOR_HIDROMETRO`: Quando usar o segundo simulador

### Variáveis do Makefile

Definidas automaticamente pelo `simulator_config.mk`:

```makefile
SIMULATOR_SOURCES    # Arquivos fonte do simulador
SIMULATOR_UTILS      # Utilitários específicos
SIMULATOR_LIBS       # Bibliotecas necessárias
SIMULATOR_INCLUDE    # Diretórios de include
SIMULATOR_FLAG       # Flag de compilação
```

---

## 🎯 Casos de Uso

### Use Hydrometer Project Simulator quando:
- Precisar de visualização gráfica em tempo real
- Quiser controle interativo via teclado
- Trabalhar com cálculos hidráulicos avançados
- Testar múltiplas instâncias simultâneas

### Use Simulador SHA quando:
- Preferir interface CLI
- Precisar processar imagens de hidrômetros
- Trabalhar com padrão Facade
- Integrar com sistemas baseados em OpenCV

---

## ❓ Solução de Problemas

### Erro: "Nenhum simulador foi configurado"
**Solução:** Certifique-se de que `simulator_config.mk` existe e está configurado corretamente.

### Erro: "cairo.h: No such file or directory"
**Solução:** Instale o Cairo:
```bash
sudo apt-get install libcairo2-dev
```

### Erro: "opencv2/opencv.hpp: No such file or directory"
**Solução:** Instale o OpenCV:
```bash
sudo apt-get install libopencv-dev
```

### Erro de linkagem com pthread
**Solução:** Já está incluído automaticamente. Se persistir, verifique se tem pthread instalado:
```bash
sudo apt-get install build-essential
```

---

## 📚 Mais Informações

- **Hydrometer Project Simulator**: Veja `src/simulator/hydrometer-project-simulator/README.md`
- **Simulador SHA**: Veja `src/simulator/simulador-hidrometro/README.md`
- **Documentação do Projeto**: Veja `docs/`

---

## 🤝 Contribuindo

Se você adicionar um novo simulador:

1. Crie um diretório em `src/simulator/`
2. Adicione a configuração em `simulator_config.mk`
3. Atualize `src/simulator_wrapper.hpp` com os includes apropriados
4. Adapte `main.cpp` se necessário
5. Atualize este README

---

**Autor:** Marcos Belo  
**Data:** 16/12/2025  
**Versão:** 2.0
