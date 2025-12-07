#ifndef USUARIO_SERVICE_FACTORY_HPP
#define USUARIO_SERVICE_FACTORY_HPP

#include "usuario_service.hpp"
#include "../storage/armazenamento_volatil.hpp"
#include "../storage/armazenamento_sqlite.hpp"
#include <memory>
#include <string>

// Factory para criar UsuarioService com a estratégia apropriada
class UsuarioServiceFactory {
public:
    enum class TipoArmazenamento {
        VOLATIL,   // Memória (para testes)
        SQLITE     // Banco de dados persistente
    };
    
    // Cria um UsuarioService com a estratégia especificada
    static std::unique_ptr<UsuarioService> criar(
        TipoArmazenamento tipo, 
        const std::string& caminhoDb = "ssmh.db"
    ) {
        std::unique_ptr<ArmazenamentoStrategy> estrategia;
        
        switch (tipo) {
            case TipoArmazenamento::VOLATIL:
                estrategia = std::make_unique<ArmazenamentoVolatil>();
                break;
                
            case TipoArmazenamento::SQLITE:
                estrategia = std::make_unique<ArmazenamentoSqlite>(caminhoDb);
                break;
                
            default:
                throw std::runtime_error("Tipo de armazenamento invalido");
        }
        
        return std::make_unique<UsuarioService>(std::move(estrategia));
    }
    
    // Cria baseado em string (útil para configuração)
    static std::unique_ptr<UsuarioService> criarPorNome(
        const std::string& nomeEstrategia,
        const std::string& caminhoDb = "ssmh.db"
    ) {
        TipoArmazenamento tipo;
        
        if (nomeEstrategia == "volatil" || nomeEstrategia == "memoria") {
            tipo = TipoArmazenamento::VOLATIL;
        } else if (nomeEstrategia == "sqlite" || nomeEstrategia == "database") {
            tipo = TipoArmazenamento::SQLITE;
        } else {
            throw std::invalid_argument("Estrategia desconhecida: " + nomeEstrategia);
        }
        
        return criar(tipo, caminhoDb);
    }
};

#endif
