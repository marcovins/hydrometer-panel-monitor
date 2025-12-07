#include "leitura.hpp"
#include <sstream>
#include <iomanip>

Leitura::Leitura() 
    : id_(0), idSha_(""), valor_(0), dataHora_(std::time(nullptr)) {
}

Leitura::Leitura(int id, const std::string& idSha, int valor, std::time_t dataHora)
    : id_(id), idSha_(idSha), valor_(valor), dataHora_(dataHora) {
}

std::string Leitura::getDataHoraFormatada() const {
    std::tm* tm = std::localtime(&dataHora_);
    std::stringstream ss;
    ss << std::put_time(tm, "%d/%m/%Y %H:%M:%S");
    return ss.str();
}
