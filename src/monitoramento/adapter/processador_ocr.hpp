#ifndef PROCESSADOR_OCR_HPP
#define PROCESSADOR_OCR_HPP

#include <string>

/**
 * @brief Interface Target do padrão Adapter
 * 
 * Define a interface esperada pelo sistema para processar
 * imagens de hidrômetros e extrair valores numéricos.
 * 
 * Padrão de Projeto: Adapter (Target)
 */
class ProcessadorOCR {
public:
    virtual ~ProcessadorOCR() = default;
    
    /**
     * @brief Extrai números de uma imagem de hidrômetro
     * @param caminhoImagem Caminho para o arquivo de imagem
     * @return Valor numérico extraído da imagem (em litros)
     * @throws std::runtime_error se a imagem não puder ser processada
     */
    virtual int extrairNumeros(const std::string& caminhoImagem) = 0;
    
    /**
     * @brief Valida se um caminho de imagem é válido
     * @param caminhoImagem Caminho para validar
     * @return true se o caminho é válido, false caso contrário
     */
    virtual bool validarCaminho(const std::string& caminhoImagem) = 0;
};

#endif // PROCESSADOR_OCR_HPP
