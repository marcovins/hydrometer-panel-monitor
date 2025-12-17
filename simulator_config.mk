# ==============================================================================
# CONFIGURAÇÃO DO SIMULADOR
# ==============================================================================
# Este arquivo permite escolher qual simulador será usado no projeto.
# 
# OPÇÕES DISPONÍVEIS:
#   1. hydrometer-project-simulator (padrão)
#      - Simulador completo com multi-threading
#      - Visualização gráfica com Cairo
#      - Cálculos hidráulicos avançados
#      - Controle interativo de vazão
#
#   2. simulador-hidrometro
#      - Simulador simplificado
#      - Foco em funcionalidades básicas
#      - Mais leve e fácil de integrar
#
# ==============================================================================

# Escolha o simulador: hydrometer-project-simulator ou simulador-hidrometro
SIMULATOR_TYPE = hydrometer-project-simulator

# ==============================================================================
# CONFIGURAÇÕES AUTOMÁTICAS (Não modificar)
# ==============================================================================

ifeq ($(SIMULATOR_TYPE),hydrometer-project-simulator)
    # Configurações para hydrometer-project-simulator
    SIMULATOR_BASE_DIR = $(SRC_DIR)/simulator/hydrometer-project-simulator
    SIMULATOR_SRC_DIR = $(SIMULATOR_BASE_DIR)/src
    SIMULATOR_INCLUDE = -I$(SIMULATOR_BASE_DIR)
    
    # Arquivos do simulador
    SIMULATOR_SOURCES = $(SIMULATOR_SRC_DIR)/modules/simulator.cpp \
                        $(SIMULATOR_SRC_DIR)/modules/hidrometer.cpp \
                        $(SIMULATOR_SRC_DIR)/modules/pipe.cpp \
                        $(SIMULATOR_SRC_DIR)/modules/hidrometer_manager.cpp
    
    SIMULATOR_UTILS = $(SIMULATOR_SRC_DIR)/utils/image.cpp \
                      $(SIMULATOR_SRC_DIR)/utils/logger.cpp
    
    # Bibliotecas necessárias
    SIMULATOR_LIBS = -pthread $(CAIRO_LIBS)
    
    # Flag de identificação
    SIMULATOR_FLAG = -DUSE_HYDROMETER_PROJECT_SIMULATOR
    
else ifeq ($(SIMULATOR_TYPE),simulador-hidrometro)
    # Configurações para simulador-hidrometro (FachadaSHA + CLI + OpenCV)
    SIMULATOR_BASE_DIR = $(SRC_DIR)/simulator/simulador-hidrometro
    SIMULATOR_SRC_DIR = $(SIMULATOR_BASE_DIR)/src
    SIMULATOR_INCLUDE = -I$(SIMULATOR_BASE_DIR)/include
    
    # Arquivos do simulador
    SIMULATOR_SOURCES = $(SIMULATOR_SRC_DIR)/cano.cpp \
                        $(SIMULATOR_SRC_DIR)/controlador.cpp \
                        $(SIMULATOR_SRC_DIR)/display.cpp \
                        $(SIMULATOR_SRC_DIR)/hidrometro.cpp \
                        $(SIMULATOR_SRC_DIR)/fachada.cpp \
                        $(SIMULATOR_SRC_DIR)/cli.cpp
    
    SIMULATOR_UTILS =
    
    # Bibliotecas necessárias (OpenCV)
    OPENCV_LIBS = `pkg-config --cflags --libs opencv4`
    SIMULATOR_LIBS = -pthread $(OPENCV_LIBS)
    
    # Flag de identificação
    SIMULATOR_FLAG = -DUSE_SIMULADOR_HIDROMETRO
    
else
    $(error SIMULATOR_TYPE inválido: $(SIMULATOR_TYPE). Use 'hydrometer-project-simulator' ou 'simulador-hidrometro')
endif

# ==============================================================================
# INFORMAÇÕES DE CONFIGURAÇÃO
# ==============================================================================
$(info )
$(info ========================================)
$(info Configuração do Simulador)
$(info ========================================)
$(info Simulador selecionado: $(SIMULATOR_TYPE))
$(info Diretório base: $(SIMULATOR_BASE_DIR))
$(info Arquivos fonte: $(words $(SIMULATOR_SOURCES)) arquivo(s))
$(info Bibliotecas: $(SIMULATOR_LIBS))
$(info ========================================)
$(info )
