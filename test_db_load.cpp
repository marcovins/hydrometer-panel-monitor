// Teste rápido de carregamento do banco de dados
#include "src/usuarios/storage/armazenamento_sqlite.hpp"
#include "src/usuarios/services/usuario_service.hpp"
#include <iostream>
#include <memory>

int main() {
    try {
        std::cout << "Testando carregamento do banco de dados...\n\n";
        
        auto armazenamento = std::make_unique<ArmazenamentoSqlite>("usuarios.db");
        auto usuarioService = std::make_shared<UsuarioService>(std::move(armazenamento));
        
        std::cout << "Listando usuários do banco:\n";
        auto usuarios = usuarioService->listarUsuarios();
        
        if (usuarios.empty()) {
            std::cout << "❌ Nenhum usuário encontrado!\n";
        } else {
            std::cout << "✅ " << usuarios.size() << " usuário(s) encontrado(s):\n\n";
            for (const auto& u : usuarios) {
                std::cout << "  ID: " << u.getId() << "\n";
                std::cout << "  Nome: " << u.getNome() << "\n";
                std::cout << "  Email: " << u.getEmail() << "\n";
                
                auto hidros = usuarioService->listarHidrometros(u.getId());
                std::cout << "  Hidrômetros: " << hidros.size() << "\n";
                for (const auto& sha : hidros) {
                    std::cout << "    - " << sha << "\n";
                }
                std::cout << "\n";
            }
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro: " << e.what() << "\n";
        return 1;
    }
}
