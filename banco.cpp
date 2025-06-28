#include "banco.h"
#include "exceptions.h"
#include <sstream>
#include <iomanip>

Banco::Banco(const std::string& nombre, const std::string& codigo) 
    : nombre(nombre), codigo(codigo) {
}

std::string Banco::getNombre() const {
    return nombre;
}

std::string Banco::getCodigo() const {
    return codigo;
}

const std::vector<std::unique_ptr<Boveda>>& Banco::getBovedas() const {
    return bovedas;
}

void Banco::agregarBoveda(std::unique_ptr<Boveda> boveda) {
    if (!boveda) {
        throw DatosInvalidosException("No se puede agregar una bóveda nula");
    }
    
    // Verificar que no existe una bóveda con el mismo ID
    for (const auto& b : bovedas) {
        if (b->getId() == boveda->getId()) {
            throw OperacionInvalidaException("Ya existe una bóveda con ID: " + boveda->getId());
        }
    }
    
    bovedas.push_back(std::move(boveda));
}

Boveda* Banco::buscarBoveda(const std::string& idBoveda) {
    for (auto& boveda : bovedas) {
        if (boveda->getId() == idBoveda) {
            return boveda.get();
        }
    }
    throw BovedaNoEncontradaException("Bóveda no encontrada: " + idBoveda);
}

const Boveda* Banco::buscarBoveda(const std::string& idBoveda) const {
    for (const auto& boveda : bovedas) {
        if (boveda->getId() == idBoveda) {
            return boveda.get();
        }
    }
    throw BovedaNoEncontradaException("Bóveda no encontrada: " + idBoveda);
}

double Banco::getActivosTotales() const {
    double total = 0.0;
    for (const auto& boveda : bovedas) {
        total += boveda->getValorTotalEnDolares();
    }
    return total;
}

std::string Banco::getResumen() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "=== " << nombre << " (" << codigo << ") ===\n";
    ss << "Activos totales: $ " << getActivosTotales() << "\n";
    ss << "Número de bóvedas: " << bovedas.size() << "\n\n";
    
    for (const auto& boveda : bovedas) {
        ss << boveda->getResumen() << "\n\n";
    }
    
    return ss.str();
}

void Banco::transferirEntreBovedas(const std::string& bovedaOrigenId, 
                                  const std::string& bovedaDestinoId, 
                                  const Activo& activo) {
    if (bovedaOrigenId == bovedaDestinoId) {
        throw OperacionInvalidaException("La bóveda de origen no puede ser la misma que la de destino");
    }
    
    Boveda* origen = buscarBoveda(bovedaOrigenId);
    Boveda* destino = buscarBoveda(bovedaDestinoId);
    
    // Verificar que la bóveda de origen tiene suficientes activos
    if (!origen->tieneActivo(activo)) {
        throw SaldoInsuficienteException("Bóveda de origen no tiene suficientes activos");
    }
    
    // Realizar la transferencia
    origen->retirarActivo(activo);
    destino->agregarActivo(activo);
}
