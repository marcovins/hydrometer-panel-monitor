#include <iostream>
#include <iomanip>
#include "src/modules/usuario.hpp"
#include "src/modules/usuario_service.hpp"
#include "src/modules/armazenamento_volatil.hpp"
#include "src/modules/user_commands.hpp"
#include "src/modules/command_invoker.hpp"

void imprimirUsuario(const Usuario& user) {
    std::cout << "  ID: " << user.getId() << std::endl;
    std::cout << "  Nome: " << user.getNome() << std::endl;
    std::cout << "  Email: " << user.getEmail() << std::endl;
    std::cout << "  Perfil: " << Usuario::tipoPerfilToString(user.getTipoPerfil()) << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "TESTE DO SUBSISTEMA DE GERENCIAMENTO DE USUARIOS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Cria o serviço de usuários com armazenamento em memória
    UsuarioService usuarioService;
    
    // Cria o invoker para gerenciar comandos
    CommandInvoker invoker;
    
    std::cout << "[1] Criando usuarios..." << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // Cria primeiro usuário (ADMIN)
    std::map<std::string, std::string> dadosAdmin;
    dadosAdmin["nome"] = "Maria Silva";
    dadosAdmin["email"] = "maria@cagepa.pb.gov.br";
    dadosAdmin["tipoPerfil"] = "ADMIN";
    
    auto cmdCriarAdmin = std::make_unique<CriarUsuarioCommand>(&usuarioService, dadosAdmin);
    invoker.executarComando(std::move(cmdCriarAdmin));
    std::cout << "Usuario ADMIN criado!" << std::endl;
    
    // Cria segundo usuário (LEITOR)
    std::map<std::string, std::string> dadosLeitor;
    dadosLeitor["nome"] = "Joao Santos";
    dadosLeitor["email"] = "joao@exemplo.com";
    dadosLeitor["tipoPerfil"] = "LEITOR";
    
    auto cmdCriarLeitor = std::make_unique<CriarUsuarioCommand>(&usuarioService, dadosLeitor);
    invoker.executarComando(std::move(cmdCriarLeitor));
    std::cout << "Usuario LEITOR criado!" << std::endl;
    std::cout << std::endl;
    
    // Lista todos os usuários
    std::cout << "[2] Listando todos os usuarios:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    auto usuarios = usuarioService.listarUsuarios();
    for (const auto& user : usuarios) {
        imprimirUsuario(user);
        std::cout << std::endl;
    }
    
    // Vincula hidrômetros ao usuário 2
    std::cout << "[3] Vinculando hidrometros ao usuario 2..." << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    auto cmdVincular1 = std::make_unique<VincularHidrometroCommand>(&usuarioService, 2, "SHA-001");
    invoker.executarComando(std::move(cmdVincular1));
    std::cout << "Hidrometro SHA-001 vinculado!" << std::endl;
    
    auto cmdVincular2 = std::make_unique<VincularHidrometroCommand>(&usuarioService, 2, "SHA-002");
    invoker.executarComando(std::move(cmdVincular2));
    std::cout << "Hidrometro SHA-002 vinculado!" << std::endl;
    std::cout << std::endl;
    
    // Lista hidrômetros do usuário 2
    std::cout << "[4] Hidrometros do usuario 2:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    auto hidrometros = usuarioService.listarHidrometros(2);
    for (const auto& sha : hidrometros) {
        std::cout << "  - " << sha << std::endl;
    }
    std::cout << std::endl;
    
    // Atualiza dados do usuário 2
    std::cout << "[5] Atualizando email do usuario 2..." << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::map<std::string, std::string> dadosAtualizacao;
    dadosAtualizacao["email"] = "joao.santos@novodominio.com";
    
    auto cmdAtualizar = std::make_unique<AtualizarUsuarioCommand>(&usuarioService, 2, dadosAtualizacao);
    invoker.executarComando(std::move(cmdAtualizar));
    
    Usuario usuarioAtualizado = usuarioService.buscarUsuario(2);
    std::cout << "Usuario apos atualizacao:" << std::endl;
    imprimirUsuario(usuarioAtualizado);
    std::cout << std::endl;
    
    // Demonstra funcionalidade de UNDO
    std::cout << "[6] Testando funcionalidade de UNDO..." << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Desfazendo ultima atualizacao..." << std::endl;
    invoker.desfazer();
    
    Usuario usuarioRestaurado = usuarioService.buscarUsuario(2);
    std::cout << "Usuario apos UNDO:" << std::endl;
    imprimirUsuario(usuarioRestaurado);
    std::cout << std::endl;
    
    // Adiciona faturas
    std::cout << "[7] Adicionando faturas ao usuario 2..." << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    std::time_t hoje = std::time(nullptr);
    std::time_t mesQueVem = hoje + (30 * 24 * 60 * 60); // +30 dias
    
    usuarioService.adicionarFatura(2, 45.80, mesQueVem, "PENDENTE");
    std::cout << "Fatura de R$ 45.80 adicionada!" << std::endl;
    
    usuarioService.adicionarFatura(2, 52.30, mesQueVem, "PENDENTE");
    std::cout << "Fatura de R$ 52.30 adicionada!" << std::endl;
    std::cout << std::endl;
    
    // Lista faturas
    std::cout << "[8] Listando faturas do usuario 2:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    auto faturas = usuarioService.listarContasDeAgua(2);
    for (const auto& fatura : faturas) {
        std::cout << "  Fatura ID: " << fatura.getId() << std::endl;
        std::cout << "  Valor: R$ " << std::fixed << std::setprecision(2) << fatura.getValor() << std::endl;
        std::cout << "  Status: " << fatura.getStatus() << std::endl;
        std::cout << std::endl;
    }
    
    // Informações sobre o histórico de comandos
    std::cout << "[9] Informacoes do historico:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Comandos no historico: " << invoker.tamanhoHistorico() << std::endl;
    std::cout << "Pode desfazer: " << (invoker.podeDesfazer() ? "Sim" : "Nao") << std::endl;
    std::cout << "Pode refazer: " << (invoker.podeRefazer() ? "Sim" : "Nao") << std::endl;
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "TESTE CONCLUIDO COM SUCESSO!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
