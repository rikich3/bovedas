#ifndef SISTEMA_BOVEDAS_H
#define SISTEMA_BOVEDAS_H

#include "banco.h"
#include "transaccion.h"
#include <map>
#include <vector>
#include <memory>
#include <random>

class SistemaBovedas {
private:
    std::map<std::string, std::unique_ptr<Banco>> bancos;
    std::vector<std::unique_ptr<Transaccion>> transacciones;
    std::mt19937 generador;
    int contadorTransacciones;

public:
    SistemaBovedas();
    
    // Inicialización del sistema
    void inicializarSistema();
    void crearBancosIniciales();
    void asignarActivosAleatorios();
    
    // Manejo de bancos
    void agregarBanco(std::unique_ptr<Banco> banco);
    Banco* buscarBanco(const std::string& codigo);
    const std::map<std::string, std::unique_ptr<Banco>>& getBancos() const;
    
    // Operaciones de transferencia
    std::string iniciarTransferencia(const std::string& bancoOrigenCodigo,
                                   const std::string& bovedaOrigenId,
                                   const std::string& bancoDestinoCodigo,
                                   const std::string& bovedaDestinoId,
                                   TipoActivo tipoActivo,
                                   double cantidad,
                                   const std::string& transportadora = "Transportes Seguros SA",
                                   double porcentajeComision = 0.05);
    
    void procesarTransaccion(const std::string& transaccionId);
    void avanzarEstadoTransaccion(const std::string& transaccionId);
    void cancelarTransaccion(const std::string& transaccionId, const std::string& razon);
    
    // Consultas
    Transaccion* buscarTransaccion(const std::string& id);
    std::vector<Transaccion*> getTransaccionesActivas();
    std::vector<Transaccion*> getTodasLasTransacciones();
    
    // Información del sistema
    std::string getResumenGeneral() const;
    std::string getEstadoBancos() const;
    std::string getEstadoTransacciones() const;
    
private:
    std::string generarIdTransaccion();
    double generarCantidadAleatoria(double min, double max);
    void validarTransferencia(const std::string& bancoOrigenCodigo,
                             const std::string& bovedaOrigenId,
                             const std::string& bancoDestinoCodigo,
                             const std::string& bovedaDestinoId,
                             const Activo& activo);
};

#endif // SISTEMA_BOVEDAS_H
