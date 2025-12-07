#include <iostream>
#include <iomanip>
#include "src/usuarios/domain/usuario.hpp"
#include "src/usuarios/services/usuario_service.hpp"
#include "src/usuarios/storage/armazenamento_volatil.hpp"
#include "src/usuarios/storage/armazenamento_sqlite.hpp"
#include "src/usuarios/commands/user_commands.hpp"
#include "src/usuarios/commands/command_invoker.hpp"

void imprimirUsuario(const Usuario& user) {
    std::cout << "  ID: " << user.getId() << std::endl;
    std::cout << "  Nome: " << user.getNome() << std::endl;
    std::cout << "  Email: " << user.getEmail() << std::endl;
    std::cout << "  Perfil: " << Usuario::tipoPerfilToString(user.getTipoPerfil()) << std::endl;
}

void testarEstrategia(const std::string& nomeEstrategia, UsuarioService& service) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "TESTANDO ESTRATEGIA: " << nomeEstrategia << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    CommandInvoker invoker;
    
    // Cria usuários
    std::cout << "[1] Criando usuarios..." << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    std::map<std::string, std::string> dadosAdmin;
    dadosAdmin["nome"] = "Ana Costa";
    dadosAdmin["email"] = "ana@ssmh.com.br";
    dadosAdmin["tipoPerfil"] = "ADMIN";
    
    auto cmdCriarAdmin = std::make_unique<CriarUsuarioCommand>(&service, dadosAdmin);
    invoker.executarComando(std::move(cmdCriarAdmin));
    std::cout << "Usuario ADMIN criado!" << std::endl;
    
    std::map<std::string, std::string> dadosLeitor;
    dadosLeitor["nome"] = "Pedro Lima";
    dadosLeitor["email"] = "pedro@exemplo.com";
    dadosLeitor["tipoPerfil"] = "LEITOR";
    
    auto cmdCriarLeitor = std::make_unique<CriarUsuarioCommand>(&service, dadosLeitor);
    invoker.executarComando(std::move(cmdCriarLeitor));
    std::cout << "Usuario LEITOR criado!" << std::endl;
    std::cout << std::endl;
    
    // Lista usuários
    std::cout << "[2] Listando usuarios:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    auto usuarios = service.listarUsuarios();
    for (const auto& user : usuarios) {
        imprimirUsuario(user);
        std::cout << std::endl;
    }
    
    // Vincula hidrômetros
    std::cout << "[3] Vinculando hidrometros..." << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // Pega o último usuário criado
    int ultimoId = usuarios.back().getId();
    
    auto cmdVincular1 = std::make_unique<VincularHidrometroCommand>(&service, ultimoId, "SHA-100");
    invoker.executarComando(std::move(cmdVincular1));
    std::cout << "Hidrometro SHA-100 vinculado!" << std::endl;
    
    auto cmdVincular2 = std::make_unique<VincularHidrometroCommand>(&service, ultimoId, "SHA-101");
    invoker.executarComando(std::move(cmdVincular2));
    std::cout << "Hidrometro SHA-101 vinculado!" << std::endl;
    std::cout << std::endl;
    
    // Lista hidrômetros
    std::cout << "[4] Hidrometros do usuario " << ultimoId << ":" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    auto hidrometros = service.listarHidrometros(ultimoId);
    for (const auto& sha : hidrometros) {
        std::cout << "  - " << sha << std::endl;
    }
    std::cout << std::endl;
    
    // Adiciona faturas
    std::cout << "[5] Adicionando faturas..." << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    std::time_t hoje = std::time(nullptr);
    std::time_t mesQueVem = hoje + (30 * 24 * 60 * 60);
    
    service.adicionarFatura(ultimoId, 67.50, mesQueVem, "PENDENTE");
    std::cout << "Fatura de R$ 67.50 adicionada!" << std::endl;
    std::cout << std::endl;
    
    // Lista faturas
    std::cout << "[6] Listando faturas:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    auto faturas = service.listarContasDeAgua(ultimoId);
    for (const auto& fatura : faturas) {
        std::cout << "  Fatura ID: " << fatura.getId() << std::endl;
        std::cout << "  Valor: R$ " << std::fixed << std::setprecision(2) << fatura.getValor() << std::endl;
        std::cout << "  Status: " << fatura.getStatus() << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "========================================" << std::endl;
    std::cout << "TESTE COM " << nomeEstrategia << " CONCLUIDO!" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "TESTE DO SUBSISTEMA DE USUARIOS" << std::endl;
    std::cout << "COM MULTIPLAS ESTRATEGIAS" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::string estrategia = "volatil";
    
    // Verifica argumento de linha de comando
    if (argc > 1) {
        estrategia = argv[1];
    }
    
    try {
        if (estrategia == "sqlite") {
            // Teste com SQLite
            std::cout << ">>> Usando estrategia: SQLITE (persistente)\n" << std::endl;
            
            UsuarioService service(std::make_unique<ArmazenamentoSqlite>("ssmh_test.db"));
            testarEstrategia("SQLITE", service);
            
            std::cout << "\n[INFO] Dados persistidos em: ssmh_test.db" << std::endl;
            std::cout << "[INFO] Execute novamente para ver os dados persistidos!\n" << std::endl;
            
        } else if (estrategia == "volatil") {
            // Teste com memória volátil
            std::cout << ">>> Usando estrategia: VOLATIL (memoria)\n" << std::endl;
            
            UsuarioService service(std::make_unique<ArmazenamentoVolatil>());
            testarEstrategia("VOLATIL", service);
            
            std::cout << "\n[INFO] Dados armazenados em memoria (serao perdidos)\n" << std::endl;
            
        } else if (estrategia == "ambos") {
            // Testa ambas as estratégias
            std::cout << ">>> Testando AMBAS as estrategias\n" << std::endl;
            
            // Primeiro volátil
            {
                UsuarioService serviceVolatil(std::make_unique<ArmazenamentoVolatil>());
                testarEstrategia("VOLATIL", serviceVolatil);
            }
            
            std::cout << "\n\n";
            
            // Depois SQLite
            {
                UsuarioService serviceSqlite(std::make_unique<ArmazenamentoSqlite>("ssmh_test.db"));
                testarEstrategia("SQLITE", serviceSqlite);
            }
            
            std::cout << "\n[INFO] Comparacao completa!" << std::endl;
            std::cout << "[INFO] Dados SQLite persistidos em: ssmh_test.db\n" << std::endl;
            
        } else {
            std::cerr << "Estrategia invalida: " << estrategia << std::endl;
            std::cerr << "Use: volatil, sqlite ou ambos" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\n[ERRO] " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "TESTE FINALIZADO COM SUCESSO!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\nUSO:" << std::endl;
    std::cout << "  ./test_usuarios_db volatil  - Usa memoria (padrao)" << std::endl;
    std::cout << "  ./test_usuarios_db sqlite   - Usa banco SQLite" << std::endl;
    std::cout << "  ./test_usuarios_db ambos    - Testa ambas" << std::endl;
    std::cout << std::endl;
    
    return 0;
}
