#include "boveda.h"
#include "exceptions.h"
#include <sstream>
#include <iomanip>

Boveda::Boveda(const std::string& id, const std::string& ubicacion) 
    : id(id), ubicacion(ubicacion) {
    // Inicializar todos los tipos de activos en 0
    activos[TipoActivo::SOLES] = 0.0;
    activos[TipoActivo::DOLARES] = 0.0;
    activos[TipoActivo::JOYAS] = 0.0;
}

std::string Boveda::getId() const {
    return id;
}

std::string Boveda::getUbicacion() const {
    return ubicacion;
}

double Boveda::getSaldo(TipoActivo tipo) const {
    auto it = activos.find(tipo);
    return (it != activos.end()) ? it->second : 0.0;
}

std::map<TipoActivo, double> Boveda::getTodosLosActivos() const {
    return activos;
}

void Boveda::agregarActivo(const Activo& activo) {
    if (activo.getCantidad() <= 0) {
        throw DatosInvalidosException("No se puede agregar una cantidad negativa o cero");
    }
    activos[activo.getTipo()] += activo.getCantidad();
}

void Boveda::retirarActivo(const Activo& activo) {
    if (activo.getCantidad() <= 0) {
        throw DatosInvalidosException("No se puede retirar una cantidad negativa o cero");
    }
    
    double saldoActual = getSaldo(activo.getTipo());
    if (saldoActual < activo.getCantidad()) {
        throw SaldoInsuficienteException("Saldo insuficiente de " + activo.getTipoString() + 
                                        " en bóveda " + id + ". Disponible: " + 
                                        std::to_string(saldoActual) + ", Solicitado: " + 
                                        std::to_string(activo.getCantidad()));
    }
    
    activos[activo.getTipo()] -= activo.getCantidad();
}

bool Boveda::tieneActivo(const Activo& activo) const {
    return getSaldo(activo.getTipo()) >= activo.getCantidad();
}

double Boveda::getValorTotalEnDolares() const {
    // Tasas de conversión aproximadas (en un sistema real vendrían de una API)
    const double tasaSolesToDolares = 0.27; // 1 sol ≈ 0.27 USD
    const double tasaJoyasToDolares = 50.0;  // 1 unidad de joya ≈ 50 USD
    
    double total = 0.0;
    total += getSaldo(TipoActivo::DOLARES);
    total += getSaldo(TipoActivo::SOLES) * tasaSolesToDolares;
    total += getSaldo(TipoActivo::JOYAS) * tasaJoyasToDolares;
    
    return total;
}

std::string Boveda::getResumen() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Bóveda: " << id << " (" << ubicacion << ")\n";
    ss << "  Soles: S/ " << getSaldo(TipoActivo::SOLES) << "\n";
    ss << "  Dólares: $ " << getSaldo(TipoActivo::DOLARES) << "\n";
    ss << "  Joyas: " << getSaldo(TipoActivo::JOYAS) << " unidades\n";
    ss << "  Valor total: $ " << getValorTotalEnDolares();
    return ss.str();
}
