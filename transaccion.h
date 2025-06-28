#ifndef TRANSACCION_H
#define TRANSACCION_H

#include "activo.h"
#include <string>
#include <chrono>

enum class EstadoTransaccion {
    PREPARACION,
    RECOJO,
    TRANSPORTE,
    ENTREGA,
    COMPLETADA,
    CANCELADA
};

enum class TipoTransaccion {
    INTRABANCARIA,  // Entre bóvedas del mismo banco
    INTERBANCARIA   // Entre bóvedas de diferentes bancos
};

class Transaccion {
private:
    std::string id;
    std::string bancoOrigenCodigo;
    std::string bovedaOrigenId;
    std::string bancoDestinoCodigo;
    std::string bovedaDestinoId;
    Activo activo;
    EstadoTransaccion estado;
    TipoTransaccion tipo;
    std::string transportadora;
    double porcentajeComision;
    std::chrono::system_clock::time_point fechaCreacion;
    std::chrono::system_clock::time_point fechaCompletada;
    std::string observaciones;

public:
    Transaccion(const std::string& id,
                const std::string& bancoOrigenCodigo,
                const std::string& bovedaOrigenId,
                const std::string& bancoDestinoCodigo,
                const std::string& bovedaDestinoId,
                const Activo& activo,
                const std::string& transportadora = "Transportes Seguros SA",
                double porcentajeComision = 0.05);

    // Getters
    std::string getId() const;
    std::string getBancoOrigenCodigo() const;
    std::string getBovedaOrigenId() const;
    std::string getBancoDestinoCodigo() const;
    std::string getBovedaDestinoId() const;
    Activo getActivo() const;
    EstadoTransaccion getEstado() const;
    TipoTransaccion getTipo() const;
    std::string getTransportadora() const;
    double getPorcentajeComision() const;
    std::string getObservaciones() const;
    
    // Manejo de estado
    void avanzarEstado();
    void cancelar(const std::string& razon);
    bool esIntrabancaria() const;
    bool estaCompletada() const;
    
    // Cálculos
    double getComision() const;
    Activo getActivoNeto() const; // Activo menos comisión
    
    // Información
    std::string getEstadoString() const;
    std::string getTipoString() const;
    std::string getResumen() const;
    
    static std::string estadoToString(EstadoTransaccion estado);
    static std::string tipoToString(TipoTransaccion tipo);
};

#endif // TRANSACCION_H
