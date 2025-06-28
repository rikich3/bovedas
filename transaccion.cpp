#include "transaccion.h"
#include "exceptions.h"
#include <sstream>
#include <iomanip>

Transaccion::Transaccion(const std::string& id,
                        const std::string& bancoOrigenCodigo,
                        const std::string& bovedaOrigenId,
                        const std::string& bancoDestinoCodigo,
                        const std::string& bovedaDestinoId,
                        const Activo& activo,
                        const std::string& transportadora,
                        double porcentajeComision)
    : id(id), bancoOrigenCodigo(bancoOrigenCodigo), bovedaOrigenId(bovedaOrigenId),
      bancoDestinoCodigo(bancoDestinoCodigo), bovedaDestinoId(bovedaDestinoId),
      activo(activo), estado(EstadoTransaccion::PREPARACION),
      transportadora(transportadora), porcentajeComision(porcentajeComision),
      fechaCreacion(std::chrono::system_clock::now()) {
    
    if (porcentajeComision < 0 || porcentajeComision > 1) {
        throw DatosInvalidosException("El porcentaje de comisión debe estar entre 0 y 1");
    }
    
    // Determinar el tipo de transacción
    tipo = (bancoOrigenCodigo == bancoDestinoCodigo) ? 
           TipoTransaccion::INTRABANCARIA : TipoTransaccion::INTERBANCARIA;
}

std::string Transaccion::getId() const {
    return id;
}

std::string Transaccion::getBancoOrigenCodigo() const {
    return bancoOrigenCodigo;
}

std::string Transaccion::getBovedaOrigenId() const {
    return bovedaOrigenId;
}

std::string Transaccion::getBancoDestinoCodigo() const {
    return bancoDestinoCodigo;
}

std::string Transaccion::getBovedaDestinoId() const {
    return bovedaDestinoId;
}

Activo Transaccion::getActivo() const {
    return activo;
}

EstadoTransaccion Transaccion::getEstado() const {
    return estado;
}

TipoTransaccion Transaccion::getTipo() const {
    return tipo;
}

std::string Transaccion::getTransportadora() const {
    return transportadora;
}

double Transaccion::getPorcentajeComision() const {
    return porcentajeComision;
}

std::string Transaccion::getObservaciones() const {
    return observaciones;
}

void Transaccion::avanzarEstado() {
    switch (estado) {
        case EstadoTransaccion::PREPARACION:
            estado = EstadoTransaccion::RECOJO;
            break;
        case EstadoTransaccion::RECOJO:
            estado = EstadoTransaccion::TRANSPORTE;
            break;
        case EstadoTransaccion::TRANSPORTE:
            estado = EstadoTransaccion::ENTREGA;
            break;
        case EstadoTransaccion::ENTREGA:
            estado = EstadoTransaccion::COMPLETADA;
            fechaCompletada = std::chrono::system_clock::now();
            break;
        case EstadoTransaccion::COMPLETADA:
            throw OperacionInvalidaException("La transacción ya está completada");
        case EstadoTransaccion::CANCELADA:
            throw OperacionInvalidaException("No se puede avanzar una transacción cancelada");
    }
}

void Transaccion::cancelar(const std::string& razon) {
    if (estado == EstadoTransaccion::COMPLETADA) {
        throw OperacionInvalidaException("No se puede cancelar una transacción completada");
    }
    estado = EstadoTransaccion::CANCELADA;
    observaciones = razon;
}

bool Transaccion::esIntrabancaria() const {
    return tipo == TipoTransaccion::INTRABANCARIA;
}

bool Transaccion::estaCompletada() const {
    return estado == EstadoTransaccion::COMPLETADA;
}

double Transaccion::getComision() const {
    // Para joyas usamos el valor estimado, para monedas el valor nominal
    double valorParaComision = activo.getCantidad();
    if (activo.getTipo() == TipoActivo::JOYAS) {
        valorParaComision *= 50.0; // Valor estimado por unidad de joya
    }
    return valorParaComision * porcentajeComision;
}

Activo Transaccion::getActivoNeto() const {
    // La comisión se descuenta del activo original
    double cantidadNeta = activo.getCantidad();
    
    if (activo.getTipo() != TipoActivo::JOYAS) {
        // Para dinero, la comisión se descuenta directamente
        cantidadNeta -= activo.getCantidad() * porcentajeComision;
    }
    // Para joyas, la comisión se maneja por separado ya que no se pueden "dividir"
    
    return Activo(activo.getTipo(), cantidadNeta);
}

std::string Transaccion::getEstadoString() const {
    return estadoToString(estado);
}

std::string Transaccion::getTipoString() const {
    return tipoToString(tipo);
}

std::string Transaccion::getResumen() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Transacción ID: " << id << "\n";
    ss << "Tipo: " << getTipoString() << "\n";
    ss << "Estado: " << getEstadoString() << "\n";
    ss << "Origen: " << bancoOrigenCodigo << " - Bóveda " << bovedaOrigenId << "\n";
    ss << "Destino: " << bancoDestinoCodigo << " - Bóveda " << bovedaDestinoId << "\n";
    ss << "Activo: " << activo.getCantidad() << " " << activo.getTipoString() << "\n";
    ss << "Transportadora: " << transportadora << "\n";
    ss << "Comisión: " << (porcentajeComision * 100) << "% ($ " << getComision() << ")\n";
    if (!observaciones.empty()) {
        ss << "Observaciones: " << observaciones << "\n";
    }
    return ss.str();
}

std::string Transaccion::estadoToString(EstadoTransaccion estado) {
    switch (estado) {
        case EstadoTransaccion::PREPARACION: return "Preparación";
        case EstadoTransaccion::RECOJO: return "Recojo";
        case EstadoTransaccion::TRANSPORTE: return "Transporte";
        case EstadoTransaccion::ENTREGA: return "Entrega";
        case EstadoTransaccion::COMPLETADA: return "Completada";
        case EstadoTransaccion::CANCELADA: return "Cancelada";
        default: return "Desconocido";
    }
}

std::string Transaccion::tipoToString(TipoTransaccion tipo) {
    switch (tipo) {
        case TipoTransaccion::INTRABANCARIA: return "Intrabancaria";
        case TipoTransaccion::INTERBANCARIA: return "Interbancaria";
        default: return "Desconocido";
    }
}
