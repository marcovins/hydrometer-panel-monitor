#include "fachada_ssmh.hpp"
#include <stdexcept>
#include <sstream>
#include <iostream>

// ==================== CONSTRUTOR E INICIALIZAÇÃO ====================

FachadaSSMH::FachadaSSMH(
    std::shared_ptr<UsuarioService> usuarioSvc,
    std::shared_ptr<MonitoramentoService> monitoramentoSvc,
    std::shared_ptr<AlertaService> alertaSvc
) : usuarioService(usuarioSvc),
    monitoramentoService(monitoramentoSvc),
    alertaService(alertaSvc),
    commandInvoker(std::make_unique<CommandInvoker>()),
    logManager(Logger::getInstance())
{
    logManager.registrarInfo("FachadaSSMH::Construtor", "Fachada inicializada com sucesso");
}

void FachadaSSMH::inicializar() {
    logManager.registrarInfo("FachadaSSMH::inicializar", 
        "Inicializando Sistema de Monitoramento de Hidrômetros");
    
    // Subsistemas já foram inicializados, apenas registra
    logManager.registrarInfo("FachadaSSMH::inicializar", 
        "✓ Subsistema de Usuários: Operacional");
    logManager.registrarInfo("FachadaSSMH::inicializar", 
        "✓ Subsistema de Monitoramento: Operacional");
    logManager.registrarInfo("FachadaSSMH::inicializar", 
        "✓ Subsistema de Alertas: Operacional");
    logManager.registrarInfo("FachadaSSMH::inicializar", 
        "Sistema pronto para uso");
}

void FachadaSSMH::finalizar() {
    logManager.registrarInfo("FachadaSSMH::finalizar", 
        "Finalizando Sistema de Monitoramento de Hidrômetros");
    
    commandInvoker->limparHistorico();
    
    logManager.registrarInfo("FachadaSSMH::finalizar", 
        "Sistema finalizado com sucesso");
}

// ==================== FUNCIONALIDADE 1: GESTÃO DE USUÁRIOS ====================

void FachadaSSMH::executarComandoUsuario(std::unique_ptr<UserCommand> comando) {
    try {
        std::string descricao = comando->getDescricao();
        logManager.registrarInfo("FachadaSSMH::executarComandoUsuario", 
            "Executando: " + descricao);
        
        commandInvoker->executarComando(std::move(comando));
        
        logManager.registrarInfo("FachadaSSMH::executarComandoUsuario", 
            "Comando executado com sucesso");
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::executarComandoUsuario", 
            std::string("Erro ao executar comando: ") + e.what());
        throw;
    }
}

bool FachadaSSMH::desfazerUltimoComando() {
    try {
        if (!commandInvoker->podeDesfazer()) {
            logManager.registrarAviso("FachadaSSMH::desfazerUltimoComando", 
                "Nenhum comando para desfazer");
            return false;
        }
        
        bool resultado = commandInvoker->desfazer();
        
        if (resultado) {
            logManager.registrarInfo("FachadaSSMH::desfazerUltimoComando", 
                "Comando desfeito com sucesso");
        } else {
            logManager.registrarErro("FachadaSSMH::desfazerUltimoComando", 
                "Falha ao desfazer comando");
        }
        
        return resultado;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::desfazerUltimoComando", 
            std::string("Erro ao desfazer: ") + e.what());
        return false;
    }
}

bool FachadaSSMH::refazerComando() {
    try {
        if (!commandInvoker->podeRefazer()) {
            logManager.registrarAviso("FachadaSSMH::refazerComando", 
                "Nenhum comando para refazer");
            return false;
        }
        
        bool resultado = commandInvoker->refazer();
        
        if (resultado) {
            logManager.registrarInfo("FachadaSSMH::refazerComando", 
                "Comando refeito com sucesso");
        } else {
            logManager.registrarErro("FachadaSSMH::refazerComando", 
                "Falha ao refazer comando");
        }
        
        return resultado;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::refazerComando", 
            std::string("Erro ao refazer: ") + e.what());
        return false;
    }
}

Usuario FachadaSSMH::criarUsuario(const std::map<std::string, std::string>& dados) {
    try {
        std::string nome = dados.count("nome") ? dados.at("nome") : "Desconhecido";
        logManager.registrarInfo("FachadaSSMH::criarUsuario", 
            "Criando usuário: " + nome);
        
        Usuario usuario = usuarioService->criarUsuario(dados);
        
        logManager.registrarInfo("FachadaSSMH::criarUsuario", 
            "Usuário criado com ID: " + std::to_string(usuario.getId()));
        
        return usuario;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::criarUsuario", 
            std::string("Erro ao criar usuário: ") + e.what());
        throw;
    }
}

Usuario FachadaSSMH::buscarUsuario(int id) {
    try {
        logManager.registrarDebug("FachadaSSMH::buscarUsuario", 
            "Buscando usuário ID: " + std::to_string(id));
        
        Usuario usuario = usuarioService->buscarUsuario(id);
        
        logManager.registrarDebug("FachadaSSMH::buscarUsuario", 
            "Usuário encontrado: " + usuario.getNome());
        
        return usuario;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::buscarUsuario", 
            std::string("Erro ao buscar usuário: ") + e.what());
        throw;
    }
}

void FachadaSSMH::atualizarUsuario(int id, const std::map<std::string, std::string>& dados) {
    try {
        logManager.registrarInfo("FachadaSSMH::atualizarUsuario", 
            "Atualizando usuário ID: " + std::to_string(id));
        
        usuarioService->atualizarUsuario(id, dados);
        
        logManager.registrarInfo("FachadaSSMH::atualizarUsuario", 
            "Usuário atualizado com sucesso");
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::atualizarUsuario", 
            std::string("Erro ao atualizar usuário: ") + e.what());
        throw;
    }
}

void FachadaSSMH::deletarUsuario(int id) {
    try {
        logManager.registrarInfo("FachadaSSMH::deletarUsuario", 
            "Deletando usuário ID: " + std::to_string(id));
        
        usuarioService->deletarUsuario(id);
        
        logManager.registrarInfo("FachadaSSMH::deletarUsuario", 
            "Usuário deletado com sucesso");
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::deletarUsuario", 
            std::string("Erro ao deletar usuário: ") + e.what());
        throw;
    }
}

std::vector<Usuario> FachadaSSMH::listarUsuarios() {
    try {
        logManager.registrarDebug("FachadaSSMH::listarUsuarios", 
            "Listando todos os usuários");
        
        std::vector<Usuario> usuarios = usuarioService->listarUsuarios();
        
        logManager.registrarDebug("FachadaSSMH::listarUsuarios", 
            "Total de usuários: " + std::to_string(usuarios.size()));
        
        return usuarios;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::listarUsuarios", 
            std::string("Erro ao listar usuários: ") + e.what());
        throw;
    }
}

void FachadaSSMH::vincularHidrometro(int idUser, const std::string& idSha) {
    try {
        logManager.registrarInfo("FachadaSSMH::vincularHidrometro", 
            "Vinculando SHA " + idSha + " ao usuário " + std::to_string(idUser));
        
        usuarioService->vincularHidrometro(idUser, idSha);
        
        logManager.registrarInfo("FachadaSSMH::vincularHidrometro", 
            "Hidrômetro vinculado com sucesso");
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::vincularHidrometro", 
            std::string("Erro ao vincular hidrômetro: ") + e.what());
        throw;
    }
}

void FachadaSSMH::desvincularHidrometro(int idUser, const std::string& idSha) {
    try {
        logManager.registrarInfo("FachadaSSMH::desvincularHidrometro", 
            "Desvinculando SHA " + idSha + " do usuário " + std::to_string(idUser));
        
        usuarioService->desvincularHidrometro(idUser, idSha);
        
        logManager.registrarInfo("FachadaSSMH::desvincularHidrometro", 
            "Hidrômetro desvinculado com sucesso");
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::desvincularHidrometro", 
            std::string("Erro ao desvincular hidrômetro: ") + e.what());
        throw;
    }
}

std::vector<std::string> FachadaSSMH::listarHidrometros(int idUser) {
    try {
        logManager.registrarDebug("FachadaSSMH::listarHidrometros", 
            "Listando hidrômetros do usuário " + std::to_string(idUser));
        
        std::vector<std::string> hidrometros = usuarioService->listarHidrometros(idUser);
        
        logManager.registrarDebug("FachadaSSMH::listarHidrometros", 
            "Total de hidrômetros: " + std::to_string(hidrometros.size()));
        
        return hidrometros;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::listarHidrometros", 
            std::string("Erro ao listar hidrômetros: ") + e.what());
        throw;
    }
}

std::vector<Fatura> FachadaSSMH::listarContasDeAgua(int idUser) {
    try {
        logManager.registrarDebug("FachadaSSMH::listarContasDeAgua", 
            "Listando contas do usuário " + std::to_string(idUser));
        
        std::vector<Fatura> faturas = usuarioService->listarContasDeAgua(idUser);
        
        logManager.registrarDebug("FachadaSSMH::listarContasDeAgua", 
            "Total de faturas: " + std::to_string(faturas.size()));
        
        return faturas;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::listarContasDeAgua", 
            std::string("Erro ao listar contas: ") + e.what());
        throw;
    }
}

void FachadaSSMH::adicionarFatura(int idUser, double valor, std::time_t dataVencimento, 
                                  const std::string& status) {
    try {
        logManager.registrarInfo("FachadaSSMH::adicionarFatura", 
            "Adicionando fatura para usuário " + std::to_string(idUser) + 
            " no valor de R$ " + std::to_string(valor));
        
        usuarioService->adicionarFatura(idUser, valor, dataVencimento, status);
        
        logManager.registrarInfo("FachadaSSMH::adicionarFatura", 
            "Fatura adicionada com sucesso");
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::adicionarFatura", 
            std::string("Erro ao adicionar fatura: ") + e.what());
        throw;
    }
}

// ==================== FUNCIONALIDADE 2: MONITORAMENTO DE CONSUMO ====================

int FachadaSSMH::processarLeituraOCR(const std::string& idSha, const std::string& caminhoImagem) {
    try {
        logManager.registrarInfo("FachadaSSMH::processarLeituraOCR", 
            "Processando imagem para SHA " + idSha);
        
        int idLeitura = monitoramentoService->processarLeitura(idSha, caminhoImagem);
        
        if (idLeitura > 0) {
            logManager.registrarInfo("FachadaSSMH::processarLeituraOCR", 
                "Leitura processada com sucesso. ID: " + std::to_string(idLeitura));
        } else {
            logManager.registrarAviso("FachadaSSMH::processarLeituraOCR", 
                "Falha ao processar leitura");
        }
        
        return idLeitura;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::processarLeituraOCR", 
            std::string("Erro ao processar OCR: ") + e.what());
        throw;
    }
}

int FachadaSSMH::registrarLeituraManual(const std::string& idSha, int valor) {
    try {
        logManager.registrarInfo("FachadaSSMH::registrarLeituraManual", 
            "Registrando leitura manual para SHA " + idSha + ": " + std::to_string(valor) + "L");
        
        int idLeitura = monitoramentoService->registrarLeituraManual(idSha, valor);
        
        logManager.registrarInfo("FachadaSSMH::registrarLeituraManual", 
            "Leitura registrada com ID: " + std::to_string(idLeitura));
        
        return idLeitura;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::registrarLeituraManual", 
            std::string("Erro ao registrar leitura: ") + e.what());
        throw;
    }
}

double FachadaSSMH::monitorarConsumo(
    std::shared_ptr<ConsumoMonitoravel> monitoravel,
    std::time_t dataInicio,
    std::time_t dataFim
) {
    try {
        logManager.registrarDebug("FachadaSSMH::monitorarConsumo", 
            "Consultando consumo (método Composite)");
        
        double consumo = monitoramentoService->consultarConsumo(monitoravel, dataInicio, dataFim);
        
        logManager.registrarDebug("FachadaSSMH::monitorarConsumo", 
            "Consumo calculado: " + std::to_string(consumo) + "L");
        
        return consumo;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::monitorarConsumo", 
            std::string("Erro ao monitorar consumo: ") + e.what());
        throw;
    }
}

double FachadaSSMH::monitorarConsumoPorHidrometro(
    const std::string& idSha,
    std::time_t dataInicio,
    std::time_t dataFim
) {
    try {
        logManager.registrarInfo("FachadaSSMH::monitorarConsumoPorHidrometro", 
            "Consultando consumo do hidrômetro " + idSha);
        
        // Constrói objeto Composite (Leaf)
        auto consumoHidrometro = monitoramentoService->construirConsumoHidrometro(idSha);
        
        // Calcula consumo
        double consumo = monitoramentoService->consultarConsumo(
            consumoHidrometro, dataInicio, dataFim);
        
        logManager.registrarInfo("FachadaSSMH::monitorarConsumoPorHidrometro", 
            "Consumo: " + std::to_string(consumo) + "L");
        
        return consumo;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::monitorarConsumoPorHidrometro", 
            std::string("Erro ao monitorar hidrômetro: ") + e.what());
        throw;
    }
}

double FachadaSSMH::monitorarConsumoPorUsuario(
    int idUsuario,
    std::time_t dataInicio,
    std::time_t dataFim
) {
    try {
        logManager.registrarInfo("FachadaSSMH::monitorarConsumoPorUsuario", 
            "Consultando consumo do usuário " + std::to_string(idUsuario));
        
        // Coordena subsistemas: Usuários + Monitoramento
        // 1. Busca hidrômetros do usuário (Subsistema de Usuários)
        std::vector<std::string> hidrometros = usuarioService->listarHidrometros(idUsuario);
        
        if (hidrometros.empty()) {
            logManager.registrarAviso("FachadaSSMH::monitorarConsumoPorUsuario", 
                "Usuário não possui hidrômetros vinculados");
            return 0.0;
        }
        
        // 2. Constrói objeto Composite (Subsistema de Monitoramento)
        auto consumoUsuario = monitoramentoService->construirConsumoUsuario(
            idUsuario, hidrometros);
        
        // 3. Calcula consumo agregado
        double consumo = monitoramentoService->consultarConsumo(
            consumoUsuario, dataInicio, dataFim);
        
        logManager.registrarInfo("FachadaSSMH::monitorarConsumoPorUsuario", 
            "Consumo total de " + std::to_string(hidrometros.size()) + 
            " hidrômetros: " + std::to_string(consumo) + "L");
        
        return consumo;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::monitorarConsumoPorUsuario", 
            std::string("Erro ao monitorar usuário: ") + e.what());
        throw;
    }
}

// ==================== FUNCIONALIDADE 3: SISTEMA DE ALERTAS ====================

int FachadaSSMH::configurarRegraDeAlerta(
    int idUsuario,
    const std::string& tipoEstrategia,
    const std::string& valorParametro
) {
    try {
        logManager.registrarInfo("FachadaSSMH::configurarRegraDeAlerta", 
            "Configurando regra " + tipoEstrategia + " para usuário " + 
            std::to_string(idUsuario) + " com parâmetro: " + valorParametro);
        
        // Verifica se usuário existe (coordena subsistemas)
        if (!usuarioService->usuarioExiste(idUsuario)) {
            throw std::runtime_error("Usuário não encontrado: " + std::to_string(idUsuario));
        }
        
        // Delega ao subsistema de alertas
        int regraId = alertaService->salvarRegra(idUsuario, tipoEstrategia, valorParametro);
        
        logManager.registrarInfo("FachadaSSMH::configurarRegraDeAlerta", 
            "Regra criada com ID: " + std::to_string(regraId));
        
        return regraId;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::configurarRegraDeAlerta", 
            std::string("Erro ao configurar regra: ") + e.what());
        throw;
    }
}

void FachadaSSMH::verificarAlertasUsuario(int idUsuario, double consumo) {
    try {
        logManager.registrarDebug("FachadaSSMH::verificarAlertasUsuario", 
            "Verificando alertas para usuário " + std::to_string(idUsuario) + 
            " com consumo de " + std::to_string(consumo) + "L");
        
        // Verifica se usuário existe
        if (!usuarioService->usuarioExiste(idUsuario)) {
            throw std::runtime_error("Usuário não encontrado: " + std::to_string(idUsuario));
        }
        
        // Delega ao subsistema de alertas
        // O AlertaService usa padrão Observer para notificar automaticamente
        alertaService->verificarRegras(idUsuario, consumo);
        
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::verificarAlertasUsuario", 
            std::string("Erro ao verificar alertas: ") + e.what());
        throw;
    }
}

std::vector<AlertaAtivo> FachadaSSMH::listarAlertasAtivos() {
    try {
        logManager.registrarDebug("FachadaSSMH::listarAlertasAtivos", 
            "Listando alertas ativos");
        
        std::vector<AlertaAtivo> alertas = alertaService->buscarAlertasAtivos();
        
        logManager.registrarDebug("FachadaSSMH::listarAlertasAtivos", 
            "Total de alertas: " + std::to_string(alertas.size()));
        
        return alertas;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::listarAlertasAtivos", 
            std::string("Erro ao listar alertas: ") + e.what());
        throw;
    }
}

bool FachadaSSMH::reconhecerAlerta(int alertaId) {
    try {
        logManager.registrarInfo("FachadaSSMH::reconhecerAlerta", 
            "Reconhecendo alerta ID: " + std::to_string(alertaId));
        
        bool resultado = alertaService->resolverAlerta(alertaId);
        
        if (resultado) {
            logManager.registrarInfo("FachadaSSMH::reconhecerAlerta", 
                "Alerta reconhecido com sucesso");
        } else {
            logManager.registrarAviso("FachadaSSMH::reconhecerAlerta", 
                "Alerta não encontrado ou já reconhecido");
        }
        
        return resultado;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::reconhecerAlerta", 
            std::string("Erro ao reconhecer alerta: ") + e.what());
        return false;
    }
}

void FachadaSSMH::configurarCanalNotificacao(const std::string& tipoNotificacao) {
    try {
        logManager.registrarInfo("FachadaSSMH::configurarCanalNotificacao", 
            "Configurando canal de notificação: " + tipoNotificacao);
        
        // Por enquanto apenas loga, pois a implementação depende do AlertaService
        // TODO: Implementar troca de estratégia de notificação
        logManager.registrarAviso("FachadaSSMH::configurarCanalNotificacao", 
            "Método ainda não implementado no AlertaService");
        
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::configurarCanalNotificacao", 
            std::string("Erro ao configurar canal: ") + e.what());
        throw;
    }
}

bool FachadaSSMH::desativarRegraAlerta(int regraId) {
    try {
        logManager.registrarInfo("FachadaSSMH::desativarRegraAlerta", 
            "Desativando regra ID: " + std::to_string(regraId));
        
        bool resultado = alertaService->desativarRegra(regraId);
        
        if (resultado) {
            logManager.registrarInfo("FachadaSSMH::desativarRegraAlerta", 
                "Regra desativada com sucesso");
        } else {
            logManager.registrarAviso("FachadaSSMH::desativarRegraAlerta", 
                "Regra não encontrada");
        }
        
        return resultado;
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::desativarRegraAlerta", 
            std::string("Erro ao desativar regra: ") + e.what());
        return false;
    }
}

// ==================== MÉTODOS AUXILIARES ====================

bool FachadaSSMH::usuarioExiste(int id) {
    try {
        return usuarioService->usuarioExiste(id);
    } catch (const std::exception& e) {
        logManager.registrarErro("FachadaSSMH::usuarioExiste", 
            std::string("Erro ao verificar existência: ") + e.what());
        return false;
    }
}
