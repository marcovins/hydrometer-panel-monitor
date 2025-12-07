#ifndef USUARIO_HPP
#define USUARIO_HPP

#include <string>
#include <ctime>
#include <vector>
#include <map>

// Enum para tipos de perfil de usuário
enum class TipoPerfil {
    ADMIN,
    LEITOR
};

// Classe Usuario - Entidade de domínio
class Usuario {
private:
    int id;
    std::string nome;
    std::string email;
    TipoPerfil tipoPerfil;

public:
    Usuario();
    Usuario(int id, const std::string& nome, const std::string& email, TipoPerfil tipo = TipoPerfil::LEITOR);
    
    // Getters
    int getId() const;
    std::string getNome() const;
    std::string getEmail() const;
    TipoPerfil getTipoPerfil() const;
    
    // Setters
    void setId(int id);
    void setNome(const std::string& nome);
    void setEmail(const std::string& email);
    void setTipoPerfil(TipoPerfil tipo);
    
    // Conversão de string para TipoPerfil
    static TipoPerfil stringToTipoPerfil(const std::string& tipo);
    static std::string tipoPerfilToString(TipoPerfil tipo);
};

// Classe Fatura - Entidade de domínio
class Fatura {
private:
    int id;
    int idUsuario;
    double valor;
    std::time_t dataVencimento;
    std::string status;

public:
    Fatura();
    Fatura(int id, int idUsuario, double valor, std::time_t dataVencimento, const std::string& status);
    
    // Getters
    int getId() const;
    int getIdUsuario() const;
    double getValor() const;
    std::time_t getDataVencimento() const;
    std::string getStatus() const;
    
    // Setters
    void setId(int id);
    void setIdUsuario(int idUsuario);
    void setValor(double valor);
    void setDataVencimento(std::time_t data);
    void setStatus(const std::string& status);
};

#endif
