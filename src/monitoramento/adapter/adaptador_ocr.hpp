#ifndef ADAPTADOR_OCR_HPP
#define ADAPTADOR_OCR_HPP

#include "processador_ocr.hpp"
#include <memory>

/**
 * @brief Adapter concreto do padrão Adapter
 * 
 * Adapta uma biblioteca externa de OCR (ou simulação) para a interface
 * ProcessadorOCR esperada pelo sistema. Por enquanto, implementa uma
 * versão simplificada que simula a extração de números.
 * 
 * Padrão de Projeto: Adapter (Adapter)
 * 
 * NOTA: Em produção, esta classe seria conectada a uma biblioteca
 * real de OCR como Tesseract ou similar.
 */
class AdaptadorOCR : public ProcessadorOCR {
public:
    /**
     * @brief Construtor
     */
    AdaptadorOCR();
    
    /**
     * @brief Destrutor
     */
    virtual ~AdaptadorOCR() = default;
    
    /**
     * @brief Extrai números simulando um processamento OCR
     * 
     * Esta implementação simplificada:
     * 1. Valida o caminho da imagem
     * 2. Simula o processamento OCR
     * 3. Retorna um valor baseado no nome do arquivo (para testes)
     * 
     * @param caminhoImagem Caminho para o arquivo de imagem
     * @return Valor numérico extraído
     * @throws std::runtime_error se houver erro no processamento
     */
    int extrairNumeros(const std::string& caminhoImagem) override;
    
    /**
     * @brief Valida se o caminho da imagem existe e é válido
     * @param caminhoImagem Caminho a validar
     * @return true se válido, false caso contrário
     */
    bool validarCaminho(const std::string& caminhoImagem) override;
    
private:
    /**
     * @brief Simula o processamento OCR de uma imagem
     * @param caminhoImagem Caminho da imagem
     * @return Valor simulado
     */
    int simularExtracao(const std::string& caminhoImagem);
    
    /**
     * @brief Extrai um valor numérico do nome do arquivo (para simulação)
     * @param nomeArquivo Nome do arquivo
     * @return Valor extraído ou valor aleatório
     */
    int extrairValorDoNome(const std::string& nomeArquivo);
};

#endif // ADAPTADOR_OCR_HPP
