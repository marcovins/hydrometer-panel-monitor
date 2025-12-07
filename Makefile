# Makefile para o projeto Hydrometer Simulator
# Autor: Marcos Belo
# Data: 04/09/2025

# Configurações do compilador
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2
DEBUG_FLAGS = -std=c++14 -Wall -Wextra -g -DDEBUG
INCLUDE_DIR = -I./src

# Arquivos e diretórios
TARGET = hidrometer_simulator
TARGET_DEBUG = hidrometer_simulator_debug
TARGET_TEST_USUARIOS = test_usuarios
MAIN_FILE = main.cpp
TEST_USUARIOS_FILE = test_usuarios.cpp
SRC_DIR = src
CAIRO_LIBS = `pkg-config --cflags --libs cairo`
HEADER_FILES = $(wildcard $(SRC_DIR)/modules/*.h) $(wildcard $(SRC_DIR)/utils/*.h)
CPP_FILES = $(wildcard $(SRC_DIR)/modules/*.cpp) $(wildcard $(SRC_DIR)/utils/*.cpp)
ALL_SOURCES = $(MAIN_FILE) $(CPP_FILES)

# Arquivos para o teste de usuários
USUARIO_SOURCES = src/modules/usuario.cpp \
                  src/modules/armazenamento_volatil.cpp \
                  src/modules/usuario_service.cpp \
                  src/modules/user_commands.cpp \
                  src/modules/command_invoker.cpp

SOURCES = main.cpp src/modules/hidrometer.cpp src/modules/pipe.cpp src/modules/simulator.cpp src/utils/image.cpp src/utils/logger.cpp

# Cores para output
GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m # No Color

# Regra padrão
all: $(TARGET)

# Compilação em modo release
$(TARGET): $(ALL_SOURCES) $(HEADER_FILES)
	@echo "$(GREEN)Compilando projeto em modo release...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(TARGET) $(ALL_SOURCES) -pthread $(CAIRO_LIBS)
	@echo "$(GREEN)Compilação concluída com sucesso!$(NC)"

# Compilação em modo debug
debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(ALL_SOURCES) $(HEADER_FILES)
	@echo "$(YELLOW)Compilando projeto em modo debug...$(NC)"
	$(CXX) $(DEBUG_FLAGS) $(INCLUDE_DIR) -o $(TARGET_DEBUG) $(ALL_SOURCES) -pthread
	@echo "$(YELLOW)Compilação debug concluída com sucesso!$(NC)"

# Executar o programa
run: $(TARGET)
	@echo "$(GREEN)Executando $(TARGET)...$(NC)"
	@echo "$(GREEN)================================$(NC)"
	./$(TARGET)
	@echo "$(GREEN)================================$(NC)"

# Executar em modo debug
run-debug: $(TARGET_DEBUG)
	@echo "$(YELLOW)Executando $(TARGET_DEBUG)...$(NC)"
	@echo "$(YELLOW)================================$(NC)"
	./$(TARGET_DEBUG)
	@echo "$(YELLOW)================================$(NC)"

# Compilar e executar em uma única etapa
build-run: clean $(TARGET) run

# Compilar e executar em modo debug
build-run-debug: clean debug run-debug

# Verificar sintaxe sem gerar executável
check: $(MAIN_FILE) $(HEADER_FILES)
	@echo "$(YELLOW)Verificando sintaxe...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -fsyntax-only $(MAIN_FILE)
	@echo "$(GREEN)Sintaxe OK!$(NC)"

# Limpar arquivos gerados
clean:
	@echo "$(RED)Limpando arquivos compilados...$(NC)"
	rm -f $(TARGET) $(TARGET_DEBUG) $(TARGET_TEST_USUARIOS)
	@echo "$(RED)Limpeza concluída!$(NC)"

# Compilar e executar teste do subsistema de usuários
test-usuarios: $(TARGET_TEST_USUARIOS)
	@echo "$(GREEN)Executando teste do subsistema de usuários...$(NC)"
	@echo "$(GREEN)================================$(NC)"
	./$(TARGET_TEST_USUARIOS)
	@echo "$(GREEN)================================$(NC)"

# Compilação do teste de usuários
$(TARGET_TEST_USUARIOS): $(TEST_USUARIOS_FILE) $(USUARIO_SOURCES)
	@echo "$(GREEN)Compilando teste do subsistema de usuários...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(TARGET_TEST_USUARIOS) $(TEST_USUARIOS_FILE) $(USUARIO_SOURCES)
	@echo "$(GREEN)Compilação concluída com sucesso!$(NC)"

# Mostrar informações do projeto
info:
	@echo "$(GREEN)========== INFORMAÇÕES DO PROJETO ===========$(NC)"
	@echo "$(YELLOW)Projeto:$(NC) Hydrometer Simulator"
	@echo "$(YELLOW)Arquivo principal:$(NC) $(MAIN_FILE)"
	@echo "$(YELLOW)Executável:$(NC) $(TARGET)"
	@echo "$(YELLOW)Compilador:$(NC) $(CXX)"
	@echo "$(YELLOW)Flags de compilação:$(NC) $(CXXFLAGS)"
	@echo "$(YELLOW)Headers encontrados:$(NC)"
	@for header in $(HEADER_FILES); do echo "  - $$header"; done
	@echo "$(GREEN)==============================================$(NC)"

# Instalar dependências (se necessário no futuro)
install-deps:
	@echo "$(YELLOW)Verificando dependências...$(NC)"
	@which $(CXX) > /dev/null || (echo "$(RED)g++ não encontrado! Instale o build-essential$(NC)" && exit 1)
	@echo "$(GREEN)Todas as dependências estão instaladas!$(NC)"

# Criar um arquivo de distribuição
dist: clean
	@echo "$(YELLOW)Criando arquivo de distribuição...$(NC)"
	tar -czf hydrometer-simulator-$(shell date +%Y%m%d).tar.gz \
		--exclude='*.tar.gz' \
		--exclude='.git*' \
		--exclude='$(TARGET)*' \
		.
	@echo "$(GREEN)Arquivo de distribuição criado!$(NC)"

# Ajuda
help:
	@echo "$(GREEN)========== COMANDOS DISPONÍVEIS ===========$(NC)"
	@echo "$(YELLOW)make$(NC)           - Compila o projeto (modo release)"
	@echo "$(YELLOW)make debug$(NC)     - Compila o projeto (modo debug)"
	@echo "$(YELLOW)make run$(NC)       - Executa o programa"
	@echo "$(YELLOW)make run-debug$(NC) - Executa o programa em modo debug"
	@echo "$(YELLOW)make build-run$(NC) - Limpa, compila e executa"
	@echo "$(YELLOW)make build-run-debug$(NC) - Limpa, compila e executa (debug)"
	@echo "$(YELLOW)make test-usuarios$(NC) - Compila e executa teste do subsistema de usuarios"
	@echo "$(YELLOW)make check$(NC)     - Verifica a sintaxe do código"
	@echo "$(YELLOW)make clean$(NC)     - Remove arquivos compilados"
	@echo "$(YELLOW)make info$(NC)      - Mostra informações do projeto"
	@echo "$(YELLOW)make install-deps$(NC) - Verifica dependências"
	@echo "$(YELLOW)make dist$(NC)      - Cria arquivo de distribuição"
	@echo "$(YELLOW)make help$(NC)      - Mostra esta ajuda"
	@echo "$(GREEN)==========================================$(NC)"

# Evitar conflitos com arquivos de mesmo nome
.PHONY: all debug run run-debug build-run build-run-debug check clean info install-deps dist help test-usuarios

# Detectar mudanças nos headers
$(MAIN_FILE): $(HEADER_FILES)
