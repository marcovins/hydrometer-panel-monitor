# Makefile para o projeto Hydrometer Simulator
# Autor: Marcos Belo
# Data: 07/12/2025
# Versão: 2.0 - Estrutura Modular

# Configurações do compilador
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
DEBUG_FLAGS = -std=c++17 -Wall -Wextra -g -DDEBUG
INCLUDE_DIR = -I./src

# Bibliotecas
CAIRO_LIBS = `pkg-config --cflags --libs cairo`
SQLITE_LIBS = -lsqlite3

# Arquivos principais
TARGET = hidrometer_simulator
TARGET_DEBUG = hidrometer_simulator_debug
TARGET_TEST_USUARIOS = test_usuarios
TARGET_TEST_USUARIOS_DB = test_usuarios_db
TARGET_EXEMPLO_FACTORY = exemplo_factory
TARGET_TEST_MULTITHREAD = test_multithread_hidrometros
TARGET_DEMO_MULTITHREAD = demo_multithread
TARGET_TEST_MONITORAMENTO = test_monitoramento
TARGET_TEST_ALERTAS = test_alertas

MAIN_FILE = main.cpp
TEST_USUARIOS_FILE = test_usuarios.cpp
TEST_USUARIOS_DB_FILE = test_usuarios_db.cpp
EXEMPLO_FACTORY_FILE = exemplo_factory.cpp
TEST_MULTITHREAD_FILE = test_multithread_hidrometros.cpp
DEMO_MULTITHREAD_FILE = demo_multithread.cpp
TEST_MONITORAMENTO_FILE = test_monitoramento.cpp
TEST_ALERTAS_FILE = test_alertas.cpp

# Diretórios de código fonte
SRC_DIR = src
SIMULATOR_DIR = $(SRC_DIR)/simulator
USUARIOS_DIR = $(SRC_DIR)/usuarios
MONITORAMENTO_DIR = $(SRC_DIR)/monitoramento
UTILS_DIR = $(SRC_DIR)/utils

# Arquivos do simulador
SIMULATOR_SOURCES = $(SIMULATOR_DIR)/simulator.cpp \
                    $(SIMULATOR_DIR)/hidrometer.cpp \
                    $(SIMULATOR_DIR)/pipe.cpp \
                    $(SIMULATOR_DIR)/hidrometer_manager.cpp

# Arquivos de utilitários
UTILS_SOURCES = $(UTILS_DIR)/image.cpp \
                $(UTILS_DIR)/logger.cpp

# Arquivos do subsistema de monitoramento
MONITORAMENTO_DOMAIN = $(MONITORAMENTO_DIR)/domain/leitura.cpp

MONITORAMENTO_COMPOSITE = $(MONITORAMENTO_DIR)/composite/consumo_hidrometro.cpp \
                          $(MONITORAMENTO_DIR)/composite/consumo_usuario.cpp

MONITORAMENTO_ADAPTER = $(MONITORAMENTO_DIR)/adapter/adaptador_ocr.cpp

MONITORAMENTO_STORAGE = $(MONITORAMENTO_DIR)/storage/leitura_dao_memoria.cpp

MONITORAMENTO_SERVICES = $(MONITORAMENTO_DIR)/services/monitoramento_service.cpp

MONITORAMENTO_SOURCES = $(MONITORAMENTO_DOMAIN) \
                        $(MONITORAMENTO_COMPOSITE) \
                        $(MONITORAMENTO_ADAPTER) \
                        $(MONITORAMENTO_STORAGE) \
                        $(MONITORAMENTO_SERVICES)

# Arquivos do subsistema de alertas
ALERTAS_DIR = $(SRC_DIR)/alertas

ALERTAS_DOMAIN = $(ALERTAS_DIR)/domain/regra_alerta.cpp \
                 $(ALERTAS_DIR)/domain/alerta_ativo.cpp

ALERTAS_STRATEGIES = $(ALERTAS_DIR)/strategies/limite_diario_strategy.cpp \
                     $(ALERTAS_DIR)/strategies/media_movel_strategy.cpp \
                     $(ALERTAS_DIR)/strategies/deteccao_vazamento_strategy.cpp

ALERTAS_NOTIFICATIONS = $(ALERTAS_DIR)/notifications/notificacao_console_log.cpp \
                        $(ALERTAS_DIR)/notifications/notificacao_windows_popup.cpp \
                        $(ALERTAS_DIR)/notifications/notificacao_email.cpp

ALERTAS_OBSERVERS = $(ALERTAS_DIR)/observers/painel_observer.cpp \
                    $(ALERTAS_DIR)/observers/logger_observer.cpp \
                    $(ALERTAS_DIR)/observers/notificacao_observer.cpp

ALERTAS_SERVICES = $(ALERTAS_DIR)/services/alerta_service.cpp \
                   $(ALERTAS_DIR)/services/alerta_service_factory.cpp

ALERTAS_SOURCES = $(ALERTAS_DOMAIN) \
                  $(ALERTAS_STRATEGIES) \
                  $(ALERTAS_NOTIFICATIONS) \
                  $(ALERTAS_OBSERVERS) \
                  $(ALERTAS_SERVICES)

# Arquivos do subsistema de usuários
USUARIOS_DOMAIN = $(USUARIOS_DIR)/domain/usuario.cpp

USUARIOS_STORAGE = $(USUARIOS_DIR)/storage/armazenamento_volatil.cpp

USUARIOS_STORAGE_DB = $(USUARIOS_DIR)/storage/armazenamento_volatil.cpp \
                      $(USUARIOS_DIR)/storage/armazenamento_sqlite.cpp

USUARIOS_COMMANDS = $(USUARIOS_DIR)/commands/user_commands.cpp \
                    $(USUARIOS_DIR)/commands/command_invoker.cpp

USUARIOS_SERVICES = $(USUARIOS_DIR)/services/usuario_service.cpp

# Combinações para compilação
ALL_SIMULATOR_SOURCES = $(MAIN_FILE) $(SIMULATOR_SOURCES) $(UTILS_SOURCES)

USUARIO_SOURCES = $(USUARIOS_DOMAIN) \
                  $(USUARIOS_STORAGE) \
                  $(USUARIOS_SERVICES) \
                  $(USUARIOS_COMMANDS)

USUARIO_DB_SOURCES = $(USUARIOS_DOMAIN) \
                     $(USUARIOS_STORAGE_DB) \
                     $(USUARIOS_SERVICES) \
                     $(USUARIOS_COMMANDS)

# Cores para output
GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
BLUE = \033[0;34m
NC = \033[0m # No Color

# Regra padrão
all: $(TARGET)

# Compilação do simulador em modo release
$(TARGET): $(ALL_SIMULATOR_SOURCES)
	@echo "$(GREEN)Compilando simulador de hidrómetro (release)...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(TARGET) $(ALL_SIMULATOR_SOURCES) -pthread $(CAIRO_LIBS)
	@echo "$(GREEN)✓ Compilação concluída com sucesso!$(NC)"

# Compilação em modo debug
debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(ALL_SIMULATOR_SOURCES)
	@echo "$(YELLOW)Compilando simulador (debug)...$(NC)"
	$(CXX) $(DEBUG_FLAGS) $(INCLUDE_DIR) -o $(TARGET_DEBUG) $(ALL_SIMULATOR_SOURCES) -pthread
	@echo "$(YELLOW)✓ Compilação debug concluída!$(NC)"

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
	rm -f $(TARGET) $(TARGET_DEBUG) $(TARGET_TEST_USUARIOS) $(TARGET_TEST_USUARIOS_DB) $(TARGET_EXEMPLO_FACTORY) $(TARGET_TEST_MULTITHREAD) $(TARGET_DEMO_MULTITHREAD) $(TARGET_TEST_MONITORAMENTO) $(TARGET_TEST_ALERTAS)
	rm -f *.db  # Remove bancos de dados de teste
	@echo "$(RED)✓ Limpeza concluída!$(NC)"

# Compilar e executar teste do subsistema de usuários (volátil)
test-usuarios: $(TARGET_TEST_USUARIOS)
	@echo "$(GREEN)Executando teste do subsistema de usuários...$(NC)"
	@echo "$(GREEN)================================$(NC)"
	./$(TARGET_TEST_USUARIOS)
	@echo "$(GREEN)================================$(NC)"

# Compilação do teste de usuários (volátil)
$(TARGET_TEST_USUARIOS): $(TEST_USUARIOS_FILE) $(USUARIO_SOURCES)
	@echo "$(GREEN)Compilando teste de usuários (volátil)...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(TARGET_TEST_USUARIOS) $(TEST_USUARIOS_FILE) $(USUARIO_SOURCES)
	@echo "$(GREEN)✓ Compilação concluída!$(NC)"

# Compilar e executar teste com banco de dados
test-usuarios-db: $(TARGET_TEST_USUARIOS_DB)
	@echo "$(YELLOW)Executando teste com banco de dados...$(NC)"
	@echo "$(YELLOW)================================$(NC)"
	./$(TARGET_TEST_USUARIOS_DB) ambos
	@echo "$(YELLOW)================================$(NC)"

# Compilação do teste com banco de dados
$(TARGET_TEST_USUARIOS_DB): $(TEST_USUARIOS_DB_FILE) $(USUARIO_DB_SOURCES)
	@echo "$(YELLOW)Compilando teste com SQLite...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(TARGET_TEST_USUARIOS_DB) $(TEST_USUARIOS_DB_FILE) $(USUARIO_DB_SOURCES) $(SQLITE_LIBS)
	@echo "$(YELLOW)✓ Compilação concluída!$(NC)"

# Teste rápido apenas com SQLite
test-sqlite: $(TARGET_TEST_USUARIOS_DB)
	@echo "$(YELLOW)Executando teste com SQLite...$(NC)"
	@echo "$(YELLOW)================================$(NC)"
	./$(TARGET_TEST_USUARIOS_DB) sqlite
	@echo "$(YELLOW)================================$(NC)"

# Teste rápido apenas com Volátil
test-volatil: $(TARGET_TEST_USUARIOS_DB)
	@echo "$(GREEN)Executando teste com Volátil...$(NC)"
	@echo "$(GREEN)================================$(NC)"
	./$(TARGET_TEST_USUARIOS_DB) volatil
	@echo "$(GREEN)================================$(NC)"

# Compilar exemplo da Factory
exemplo-factory: $(TARGET_EXEMPLO_FACTORY)
	@echo "$(BLUE)Executando exemplo da Factory...$(NC)"
	@echo "$(BLUE)================================$(NC)"
	./$(TARGET_EXEMPLO_FACTORY)
	@echo "$(BLUE)================================$(NC)"

# Compilação do exemplo Factory
$(TARGET_EXEMPLO_FACTORY): $(EXEMPLO_FACTORY_FILE) $(USUARIO_DB_SOURCES)
	@echo "$(BLUE)Compilando exemplo da Factory...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(TARGET_EXEMPLO_FACTORY) $(EXEMPLO_FACTORY_FILE) $(USUARIO_DB_SOURCES) $(SQLITE_LIBS)
	@echo "$(BLUE)✓ Compilação concluída!$(NC)"

# Compilar e executar teste multithread de hidrometros
test-multithread: $(TARGET_TEST_MULTITHREAD)
	@echo "$(BLUE)Executando teste multithread de hidrometros...$(NC)"
	@echo "$(BLUE)================================$(NC)"
	./$(TARGET_TEST_MULTITHREAD)
	@echo "$(BLUE)================================$(NC)"

# Compilação do teste multithread
$(TARGET_TEST_MULTITHREAD): $(TEST_MULTITHREAD_FILE) $(SIMULATOR_SOURCES) $(UTILS_SOURCES) $(USUARIO_SOURCES)
	@echo "$(BLUE)Compilando teste multithread...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(TARGET_TEST_MULTITHREAD) $(TEST_MULTITHREAD_FILE) $(SIMULATOR_SOURCES) $(UTILS_SOURCES) $(USUARIO_SOURCES) -pthread $(CAIRO_LIBS)
	@echo "$(BLUE)✓ Compilação concluída!$(NC)"

# Compilar e executar demo multithread
demo-multithread: $(TARGET_DEMO_MULTITHREAD)
	@echo "$(GREEN)Executando demonstracao multithread...$(NC)"
	@echo "$(GREEN)================================$(NC)"
	./$(TARGET_DEMO_MULTITHREAD)
	@echo "$(GREEN)================================$(NC)"

# Compilação da demo multithread
$(TARGET_DEMO_MULTITHREAD): $(DEMO_MULTITHREAD_FILE) $(SIMULATOR_SOURCES) $(UTILS_SOURCES)
	@echo "$(GREEN)Compilando demonstracao multithread...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(TARGET_DEMO_MULTITHREAD) $(DEMO_MULTITHREAD_FILE) $(SIMULATOR_SOURCES) $(UTILS_SOURCES) -pthread $(CAIRO_LIBS)
	@echo "$(GREEN)✓ Compilação concluída!$(NC)"

# Compilar e executar teste do subsistema de monitoramento
test-monitoramento: $(TARGET_TEST_MONITORAMENTO)
	@echo "$(BLUE)Executando teste do subsistema de monitoramento...$(NC)"
	@echo "$(BLUE)================================$(NC)"
	./$(TARGET_TEST_MONITORAMENTO)
	@echo "$(BLUE)================================$(NC)"

# Compilação do teste de monitoramento
$(TARGET_TEST_MONITORAMENTO): $(TEST_MONITORAMENTO_FILE) $(MONITORAMENTO_SOURCES) $(UTILS_SOURCES)
	@echo "$(BLUE)Compilando teste de monitoramento...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(TARGET_TEST_MONITORAMENTO) $(TEST_MONITORAMENTO_FILE) $(MONITORAMENTO_SOURCES) $(UTILS_DIR)/logger.cpp
	@echo "$(BLUE)✓ Compilação concluída!$(NC)"

# Compilar e executar teste do subsistema de alertas
test-alertas: $(TARGET_TEST_ALERTAS)
	@echo "$(GREEN)Executando teste do subsistema de alertas...$(NC)"
	@echo "$(GREEN)================================$(NC)"
	./$(TARGET_TEST_ALERTAS)
	@echo "$(GREEN)================================$(NC)"

# Compilação do teste de alertas
$(TARGET_TEST_ALERTAS): $(TEST_ALERTAS_FILE) $(ALERTAS_SOURCES) $(UTILS_SOURCES)
	@echo "$(GREEN)Compilando teste de alertas...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(TARGET_TEST_ALERTAS) $(TEST_ALERTAS_FILE) $(ALERTAS_SOURCES) $(UTILS_DIR)/logger.cpp
	@echo "$(GREEN)✓ Compilação concluída!$(NC)"

# Mostrar informações do projeto
info:
	@echo "$(GREEN)========== INFORMAÇÕES DO PROJETO ===========$(NC)"
	@echo "$(YELLOW)Projeto:$(NC) Hydrometer Panel Monitor (SSMH)"
	@echo "$(YELLOW)Versão:$(NC) 2.0 - Estrutura Modular"
	@echo "$(YELLOW)Data:$(NC) 07/12/2025"
	@echo "$(YELLOW)Compilador:$(NC) $(CXX) $(CXXFLAGS)"
	@echo ""
	@echo "$(BLUE)Estrutura do Projeto:$(NC)"
	@echo "  src/simulator/       - Simulador de hidrômetro"
	@echo "  src/usuarios/        - Subsistema de usuários"
	@echo "    ├── domain/        - Entidades de domínio"
	@echo "    ├── storage/       - Estratégias de persistência"
	@echo "    ├── commands/      - Padrão Command"
	@echo "    └── services/      - Serviços e lógica de negócio"
	@echo "  src/monitoramento/   - Subsistema de monitoramento"
	@echo "    ├── domain/        - Entidade Leitura"
	@echo "    ├── composite/     - Padrão Composite (consumo)"
	@echo "    ├── adapter/       - Padrão Adapter (OCR)"
	@echo "    ├── storage/       - DAO de leituras"
	@echo "    └── services/      - MonitoramentoService"
	@echo "  src/alertas/         - Subsistema de alertas"
	@echo "    ├── domain/        - RegraAlerta e AlertaAtivo"
	@echo "    ├── strategies/    - Estratégias de análise"
	@echo "    ├── notifications/ - Estratégias de notificação"
	@echo "    ├── observers/     - Padrão Observer"
	@echo "    └── services/      - AlertaService e Factory"
	@echo "  src/utils/           - Utilitários (Logger, Image)"
	@echo ""
	@echo "$(GREEN)Padrões de Projeto Implementados:$(NC)"
	@echo "  • Strategy  - Persistência (Volátil/SQLite)"
	@echo "  • Strategy  - Análise de Consumo (3 tipos)"
	@echo "  • Strategy  - Notificações (3 canais)"
	@echo "  • Command   - Operações com undo/redo"
	@echo "  • Composite - Agregação de consumo"
	@echo "  • Adapter   - Processamento OCR"
	@echo "  • Observer  - Notificação de alertas"
	@echo "  • Factory   - Criação de serviços"
	@echo "  • Singleton - Logger do sistema"
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
	@echo ""
	@echo "$(BLUE)Simulador de Hidrômetro:$(NC)"
	@echo "  $(YELLOW)make$(NC) ou $(YELLOW)make all$(NC)    - Compila o simulador (release)"
	@echo "  $(YELLOW)make debug$(NC)           - Compila em modo debug"
	@echo "  $(YELLOW)make run$(NC)             - Executa o simulador"
	@echo "  $(YELLOW)make run-debug$(NC)       - Executa em modo debug"
	@echo "  $(YELLOW)make build-run$(NC)       - Limpa, compila e executa"
	@echo ""
	@echo "$(BLUE)Subsistema de Usuários:$(NC)"
	@echo "  $(YELLOW)make test-usuarios$(NC)    - Teste básico (memória volátil)"
	@echo "  $(YELLOW)make test-usuarios-db$(NC) - Teste completo (volátil + SQLite)"
	@echo "  $(YELLOW)make test-sqlite$(NC)      - Teste apenas com SQLite"
	@echo "  $(YELLOW)make test-volatil$(NC)     - Teste apenas com memória"
	@echo "  $(YELLOW)make exemplo-factory$(NC)  - Exemplo de uso da Factory"
	@echo ""
	@echo "$(BLUE)Teste Multithread:$(NC)"
	@echo "  $(YELLOW)make test-multithread$(NC)  - Teste interativo de múltiplos hidrometros"
	@echo "  $(YELLOW)make demo-multithread$(NC)  - Demonstração automatica (recomendado)"
	@echo ""
	@echo "$(BLUE)Subsistema de Monitoramento:$(NC)"
	@echo "  $(YELLOW)make test-monitoramento$(NC) - Teste do subsistema de monitoramento"
	@echo ""
	@echo "$(BLUE)Subsistema de Alertas:$(NC)"
	@echo "  $(YELLOW)make test-alertas$(NC)       - Teste completo do subsistema de alertas"
	@echo ""
	@echo "$(BLUE)Utilitários:$(NC)"
	@echo "  $(YELLOW)make clean$(NC)            - Remove arquivos compilados e bancos de teste"
	@echo "  $(YELLOW)make info$(NC)             - Mostra informações do projeto"
	@echo "  $(YELLOW)make install-deps$(NC)     - Verifica dependências"
	@echo "  $(YELLOW)make help$(NC)             - Mostra esta ajuda"
	@echo ""
	@echo "$(GREEN)==========================================$(NC)"
	@echo "$(YELLOW)Dica:$(NC) Use 'make info' para ver a estrutura modular do projeto"
	@echo "$(GREEN)==========================================$(NC)"

# Evitar conflitos com arquivos de mesmo nome
.PHONY: all debug run run-debug build-run build-run-debug clean info install-deps help \
        test-usuarios test-usuarios-db test-sqlite test-volatil exemplo-factory test-multithread demo-multithread test-monitoramento test-alertas

# Detectar mudanças nos headers
$(MAIN_FILE): $(HEADER_FILES)
