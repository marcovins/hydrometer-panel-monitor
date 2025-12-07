#include <iostream>
#include "src/usuarios/services/usuario_service_factory.hpp"
#include "src/usuarios/commands/user_commands.hpp"
#include "src/usuarios/commands/command_invoker.hpp"

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "EXEMPLO DE USO DA FACTORY" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Exemplo 1: Criar serviço com SQLite usando enum
    std::cout << "[1] Criando servico com SQLite (usando enum)..." << std::endl;
    auto serviceSqlite = UsuarioServiceFactory::criar(
        UsuarioServiceFactory::TipoArmazenamento::SQLITE,
        "exemplo.db"
    );
    std::cout << "Service criado com sucesso!\n" << std::endl;
    
    // Exemplo 2: Criar serviço com nome (útil para configuração)
    std::cout << "[2] Criando servico por nome..." << std::endl;
    auto serviceVolatil = UsuarioServiceFactory::criarPorNome("volatil");
    std::cout << "Service volatil criado!\n" << std::endl;
    
    // Exemplo 3: Usar o serviço criado
    std::cout << "[3] Usando o servico SQLite..." << std::endl;
    CommandInvoker invoker;
    
    std::map<std::string, std::string> dados;
    dados["nome"] = "Usuario Factory";
    dados["email"] = "factory@exemplo.com";
    dados["tipoPerfil"] = "ADMIN";
    
    auto comando = std::make_unique<CriarUsuarioCommand>(serviceSqlite.get(), dados);
    invoker.executarComando(std::move(comando));
    std::cout << "Usuario criado via Factory!\n" << std::endl;
    
    // Lista usuários
    std::cout << "[4] Listando usuarios..." << std::endl;
    auto usuarios = serviceSqlite->listarUsuarios();
    std::cout << "Total de usuarios: " << usuarios.size() << std::endl;
    for (const auto& user : usuarios) {
        std::cout << "  - " << user.getNome() << " (" << user.getEmail() << ")" << std::endl;
    }
    std::cout << std::endl;
    
    // Exemplo 4: Simulação de mudança de ambiente
    std::cout << "[5] Simulando mudanca de ambiente..." << std::endl;
    std::string ambiente = "producao"; // ou "teste"
    
    auto service = (ambiente == "teste") 
        ? UsuarioServiceFactory::criarPorNome("volatil")
        : UsuarioServiceFactory::criarPorNome("sqlite", "producao.db");
    
    std::cout << "Servico configurado para ambiente: " << ambiente << std::endl;
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "EXEMPLO CONCLUIDO!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\nVANTAGENS DA FACTORY:" << std::endl;
    std::cout << "  1. Criacao simplificada" << std::endl;
    std::cout << "  2. Configuracao por string (arquivos config)" << std::endl;
    std::cout << "  3. Mudanca facil de estrategia" << std::endl;
    std::cout << "  4. Menos acoplamento no codigo cliente" << std::endl;
    std::cout << std::endl;
    
    return 0;
}
