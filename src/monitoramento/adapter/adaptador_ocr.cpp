#include "adaptador_ocr.hpp"
#include "../../utils/logger.hpp"
#include <fstream>
#include <stdexcept>
#include <random>
#include <regex>
#include <sys/stat.h>

AdaptadorOCR::AdaptadorOCR() {
    Logger::getInstance().log(LogLevel::INFO, 
        "AdaptadorOCR::AdaptadorOCR", 
        "Processador OCR inicializado");
}

bool AdaptadorOCR::validarCaminho(const std::string& caminhoImagem) {
    // Verifica se o arquivo existe
    struct stat buffer;
    bool existe = (stat(caminhoImagem.c_str(), &buffer) == 0);
    
    if (!existe) {
        Logger::getInstance().log(LogLevel::WARNING, 
            "AdaptadorOCR::validarCaminho", 
            "Arquivo não encontrado: " + caminhoImagem);
        return false;
    }
    
    // Verifica se tem extensão de imagem válida
    std::string extensao = caminhoImagem.substr(caminhoImagem.find_last_of(".") + 1);
    bool extensaoValida = (extensao == "jpg" || extensao == "jpeg" || 
                          extensao == "png" || extensao == "bmp");
    
    if (!extensaoValida) {
        Logger::getInstance().log(LogLevel::WARNING, 
            "AdaptadorOCR::validarCaminho", 
            "Extensão de imagem inválida: " + extensao);
        return false;
    }
    
    return true;
}

int AdaptadorOCR::extrairNumeros(const std::string& caminhoImagem) {
    Logger::getInstance().log(LogLevel::INFO, 
        "AdaptadorOCR::extrairNumeros", 
        "Processando imagem: " + caminhoImagem);
    
    // Valida o caminho
    if (!validarCaminho(caminhoImagem)) {
        throw std::runtime_error("Caminho de imagem inválido: " + caminhoImagem);
    }
    
    // Simula o processamento OCR
    int valor = simularExtracao(caminhoImagem);
    
    Logger::getInstance().log(LogLevel::INFO, 
        "AdaptadorOCR::extrairNumeros", 
        "Valor extraído: " + std::to_string(valor) + "L");
    
    return valor;
}

int AdaptadorOCR::simularExtracao(const std::string& caminhoImagem) {
    // Em uma implementação real, aqui seria chamada a biblioteca OCR externa
    // Por exemplo: tesseract::TessBaseAPI, OpenCV, etc.
    
    // Para simulação, tenta extrair valor do nome do arquivo
    int valor = extrairValorDoNome(caminhoImagem);
    
    // Se não conseguiu extrair do nome, gera valor aleatório
    if (valor == 0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10, 100);
        valor = dis(gen);
        
        Logger::getInstance().log(LogLevel::DEBUG, 
            "AdaptadorOCR::simularExtracao", 
            "Valor simulado gerado: " + std::to_string(valor));
    }
    
    return valor;
}

int AdaptadorOCR::extrairValorDoNome(const std::string& nomeArquivo) {
    // Tenta extrair números do nome do arquivo
    // Por exemplo: "leitura_123.jpg" -> 123
    std::regex numRegex("\\d+");
    std::smatch match;
    
    if (std::regex_search(nomeArquivo, match, numRegex)) {
        try {
            return std::stoi(match.str());
        } catch (const std::exception&) {
            return 0;
        }
    }
    
    return 0;
}
