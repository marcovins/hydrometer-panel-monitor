#ifndef FACHADA_SSMH_HPP
#define FACHADA_SSMH_HPP

#include "../usuarios/services/usuario_service.hpp"
#include "../usuarios/commands/command_invoker.hpp"
#include "../usuarios/commands/user_command.hpp"
#include "../usuarios/domain/usuario.hpp"
#include "../monitoramento/services/monitoramento_service.hpp"
#include "../monitoramento/composite/consumo_monitoravel.hpp"
#include "../alertas/services/alerta_service.hpp"
#include "../alertas/domain/alerta_ativo.hpp"
#include "../utils/logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <ctime>

/**
 * @brief Fachada do Sistema de Monitoramento de Hidrômetros (SSMH)
 * 
 * Implementa o padrão Facade para fornecer uma interface simplificada
 * e unificada para os três subsistemas:
 * - Subsistema de Usuários (Gestão de usuários e contas)
 * - Subsistema de Monitoramento (Coleta e consulta de consumo)
 * - Subsistema de Alertas (Regras e notificações)
 * 
 * A fachada coordena a comunicação entre os subsistemas e serve como
 * ponto de entrada único para o cliente (GUI/CLI).
 * 
 * Responsabilidades:
 * - Orquestrar chamadas entre subsistemas
 * - Simplificar interface complexa
 * - Gerenciar log unificado via Logger (Singleton)
 * - Coordenar padrão Command para operações de usuário
 */
class FachadaSSMH {
private:
    // Subsistemas
    std::shared_ptr<UsuarioService> usuarioService;
    std::shared_ptr<MonitoramentoService> monitoramentoService;
    std::shared_ptr<AlertaService> alertaService;
    
    // Invoker do padrão Command
    std::unique_ptr<CommandInvoker> commandInvoker;
    
    // Logger Singleton (referência)
    Logger& logManager;

public:
    /**
     * @brief Construtor da Fachada
     * 
     * @param usuarioSvc Serviço de gerenciamento de usuários
     * @param monitoramentoSvc Serviço de monitoramento de consumo
     * @param alertaSvc Serviço de alertas e notificações
     */
    FachadaSSMH(
        std::shared_ptr<UsuarioService> usuarioSvc,
        std::shared_ptr<MonitoramentoService> monitoramentoSvc,
        std::shared_ptr<AlertaService> alertaSvc
    );
    
    /**
     * @brief Destrutor
     */
    ~FachadaSSMH() = default;
    
    // ==================== FUNCIONALIDADE 1: GESTÃO DE USUÁRIOS ====================
    // Usa Padrão Command para permitir desfazer operações
    
    /**
     * @brief Executa um comando de usuário (padrão Command)
     * 
     * @param comando Comando a ser executado (CriarUsuarioCommand, etc)
     */
    void executarComandoUsuario(std::unique_ptr<UserCommand> comando);
    
    /**
     * @brief Desfaz o último comando executado
     * @return true se conseguiu desfazer, false caso contrário
     */
    bool desfazerUltimoComando();
    
    /**
     * @brief Refaz o último comando desfeito
     * @return true se conseguiu refazer, false caso contrário
     */
    bool refazerComando();
    
    // Métodos diretos de gestão de usuários (alternativa ao Command)
    
    /**
     * @brief Cria um novo usuário
     * 
     * @param dados Mapa com dados do usuário (nome, cpf, email, endereco)
     * @return Usuario criado
     */
    Usuario criarUsuario(const std::map<std::string, std::string>& dados);
    
    /**
     * @brief Busca um usuário pelo ID
     * 
     * @param id ID do usuário
     * @return Usuario encontrado
     * @throws std::runtime_error se não encontrado
     */
    Usuario buscarUsuario(int id);
    
    /**
     * @brief Atualiza dados de um usuário
     * 
     * @param id ID do usuário
     * @param dados Mapa com dados a serem atualizados
     */
    void atualizarUsuario(int id, const std::map<std::string, std::string>& dados);
    
    /**
     * @brief Remove um usuário do sistema
     * 
     * @param id ID do usuário
     */
    void deletarUsuario(int id);
    
    /**
     * @brief Lista todos os usuários cadastrados
     * 
     * @return Vetor com todos os usuários
     */
    std::vector<Usuario> listarUsuarios();
    
    /**
     * @brief Vincula um hidrômetro a um usuário
     * 
     * @param idUser ID do usuário
     * @param idSha ID do hidrômetro (SHA)
     */
    void vincularHidrometro(int idUser, const std::string& idSha);
    
    /**
     * @brief Desvincula um hidrômetro de um usuário
     * 
     * @param idUser ID do usuário
     * @param idSha ID do hidrômetro (SHA)
     */
    void desvincularHidrometro(int idUser, const std::string& idSha);
    
    /**
     * @brief Lista hidrômetros vinculados a um usuário
     * 
     * @param idUser ID do usuário
     * @return Vetor com IDs dos hidrômetros
     */
    std::vector<std::string> listarHidrometros(int idUser);
    
    /**
     * @brief Lista contas de água (faturas) de um usuário
     * 
     * @param idUser ID do usuário
     * @return Vetor com faturas
     */
    std::vector<Fatura> listarContasDeAgua(int idUser);
    
    /**
     * @brief Adiciona uma fatura para um usuário
     * 
     * @param idUser ID do usuário
     * @param valor Valor da fatura
     * @param dataVencimento Data de vencimento
     * @param status Status da fatura ("PENDENTE", "PAGA", etc)
     */
    void adicionarFatura(int idUser, double valor, std::time_t dataVencimento, 
                        const std::string& status);
    
    // ==================== FUNCIONALIDADE 2: MONITORAMENTO DE CONSUMO ====================
    // Usa Padrão Composite para consulta unificada
    
    /**
     * @brief Processa uma imagem de hidrômetro via OCR
     * 
     * Delega ao subsistema de monitoramento para:
     * 1. Extrair valor via OCR (padrão Adapter)
     * 2. Salvar leitura no banco de dados
     * 
     * @param idSha ID do hidrômetro
     * @param caminhoImagem Caminho para a imagem
     * @return ID da leitura processada, ou 0 se falhou
     */
    int processarLeituraOCR(const std::string& idSha, const std::string& caminhoImagem);
    
    /**
     * @brief Registra uma leitura manual (sem OCR)
     * 
     * @param idSha ID do hidrômetro
     * @param valor Valor da leitura em litros
     * @return ID da leitura registrada
     */
    int registrarLeituraManual(const std::string& idSha, int valor);
    
    /**
     * @brief Monitora consumo usando padrão Composite
     * 
     * Aceita tanto ConsumoHidrometro (Leaf) quanto ConsumoUsuario (Composite),
     * calculando o consumo de forma transparente.
     * 
     * @param monitoravel Objeto que implementa ConsumoMonitoravel
     * @param dataInicio Timestamp de início do período
     * @param dataFim Timestamp de fim do período
     * @return Consumo total em litros
     */
    double monitorarConsumo(
        std::shared_ptr<ConsumoMonitoravel> monitoravel,
        std::time_t dataInicio,
        std::time_t dataFim
    );
    
    /**
     * @brief Monitora consumo de um hidrômetro específico
     * 
     * Método de conveniência que constrói o objeto Composite internamente.
     * 
     * @param idSha ID do hidrômetro
     * @param dataInicio Timestamp de início
     * @param dataFim Timestamp de fim
     * @return Consumo em litros
     */
    double monitorarConsumoPorHidrometro(
        const std::string& idSha,
        std::time_t dataInicio,
        std::time_t dataFim
    );
    
    /**
     * @brief Monitora consumo agregado de todos os hidrômetros de um usuário
     * 
     * Método de conveniência que:
     * 1. Busca hidrômetros do usuário
     * 2. Constrói objeto Composite (ConsumoUsuario)
     * 3. Calcula consumo agregado
     * 
     * @param idUsuario ID do usuário
     * @param dataInicio Timestamp de início
     * @param dataFim Timestamp de fim
     * @return Consumo total em litros
     */
    double monitorarConsumoPorUsuario(
        int idUsuario,
        std::time_t dataInicio,
        std::time_t dataFim
    );
    
    // ==================== FUNCIONALIDADE 3: SISTEMA DE ALERTAS ====================
    // Usa Padrão Strategy para regras configuráveis
    
    /**
     * @brief Configura uma regra de alerta para um usuário
     * 
     * Usa padrão Strategy para permitir diferentes algoritmos de análise:
     * - "LIMITE_DIARIO": Limite fixo por dia
     * - "MEDIA_MOVEL": Baseado em média histórica
     * - "DETECCAO_VAZAMENTO": Detecção de consumo anormal
     * 
     * @param idUsuario ID do usuário
     * @param tipoEstrategia Tipo de análise ("LIMITE_DIARIO", etc)
     * @param valorParametro Parâmetro da regra (ex: "70" para limite)
     * @return ID da regra criada
     */
    int configurarRegraDeAlerta(
        int idUsuario,
        const std::string& tipoEstrategia,
        const std::string& valorParametro
    );
    
    /**
     * @brief Verifica regras de alerta para um usuário
     * 
     * Delega ao subsistema de alertas para:
     * 1. Buscar regras ativas do usuário
     * 2. Consultar consumo recente
     * 3. Aplicar estratégia de análise
     * 4. Disparar notificações se necessário (padrão Observer)
     * 
     * @param idUsuario ID do usuário
     * @param consumo Consumo atual em litros
     */
    void verificarAlertasUsuario(int idUsuario, double consumo);
    
    /**
     * @brief Lista todos os alertas ativos no sistema
     * 
     * @return Vetor com alertas disparados
     */
    std::vector<AlertaAtivo> listarAlertasAtivos();
    
    /**
     * @brief Reconhece (marca como lido) um alerta
     * 
     * @param alertaId ID do alerta
     * @return true se conseguiu reconhecer, false caso contrário
     */
    bool reconhecerAlerta(int alertaId);
    
    /**
     * @brief Configura o canal de notificação (padrão Strategy)
     * 
     * Permite trocar em tempo de execução como as notificações são enviadas:
     * - "CONSOLE_LOG": Imprime no console
     * - "WINDOWS_POPUP": Mostra popup no Windows
     * - "EMAIL": Envia por e-mail
     * 
     * @param tipoNotificacao Tipo de canal ("CONSOLE_LOG", etc)
     */
    void configurarCanalNotificacao(const std::string& tipoNotificacao);
    
    /**
     * @brief Desativa uma regra de alerta
     * 
     * @param regraId ID da regra
     * @return true se conseguiu desativar, false caso contrário
     */
    bool desativarRegraAlerta(int regraId);
    
    // ==================== MÉTODOS AUXILIARES ====================
    
    /**
     * @brief Verifica se um usuário existe no sistema
     * 
     * @param id ID do usuário
     * @return true se existe, false caso contrário
     */
    bool usuarioExiste(int id);
    
    /**
     * @brief Inicializa a fachada e os subsistemas
     * 
     * Configura logging e prepara os subsistemas para uso.
     */
    void inicializar();
    
    /**
     * @brief Finaliza a fachada e libera recursos
     */
    void finalizar();
};

#endif // FACHADA_SSMH_HPP
