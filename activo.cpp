#include "activo.h"
#include "exceptions.h"

Activo::Activo(TipoActivo tipo, double cantidad) : tipo(tipo), cantidad(cantidad) {
    if (cantidad < 0) {
        throw DatosInvalidosException("La cantidad de activo no puede ser negativa");
    }
}

TipoActivo Activo::getTipo() const {
    return tipo;
}

double Activo::getCantidad() const {
    return cantidad;
}

void Activo::setCantidad(double cantidad) {
    if (cantidad < 0) {
        throw DatosInvalidosException("La cantidad de activo no puede ser negativa");
    }
    this->cantidad = cantidad;
}

std::string Activo::getTipoString() const {
    return tipoActivoToString(tipo);
}

std::string Activo::tipoActivoToString(TipoActivo tipo) {
    switch (tipo) {
        case TipoActivo::SOLES: return "Soles";
        case TipoActivo::DOLARES: return "Dólares";
        case TipoActivo::JOYAS: return "Joyas";
        default: return "Desconocido";
    }
}

TipoActivo Activo::stringToTipoActivo(const std::string& str) {
    if (str == "Soles") return TipoActivo::SOLES;
    if (str == "Dólares") return TipoActivo::DOLARES;
    if (str == "Joyas") return TipoActivo::JOYAS;
    throw DatosInvalidosException("Tipo de activo desconocido: " + str);
}

Activo Activo::operator+(const Activo& otro) const {
    if (tipo != otro.tipo) {
        throw OperacionInvalidaException("No se pueden sumar activos de diferentes tipos");
    }
    return Activo(tipo, cantidad + otro.cantidad);
}

Activo Activo::operator-(const Activo& otro) const {
    if (tipo != otro.tipo) {
        throw OperacionInvalidaException("No se pueden restar activos de diferentes tipos");
    }
    if (cantidad < otro.cantidad) {
        throw SaldoInsuficienteException("No hay suficiente activo para realizar la operación");
    }
    return Activo(tipo, cantidad - otro.cantidad);
}

bool Activo::operator>=(const Activo& otro) const {
    if (tipo != otro.tipo) {
        throw OperacionInvalidaException("No se pueden comparar activos de diferentes tipos");
    }
    return cantidad >= otro.cantidad;
}

bool Activo::operator<(const Activo& otro) const {
    if (tipo != otro.tipo) {
        throw OperacionInvalidaException("No se pueden comparar activos de diferentes tipos");
    }
    return cantidad < otro.cantidad;
}
