#include "sistema_bovedas.h"
#include "exceptions.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

SistemaBovedas::SistemaBovedas() : generador(std::random_device{}()), contadorTransacciones(1) {
}

void SistemaBovedas::inicializarSistema() {
    crearBancosIniciales();
    asignarActivosAleatorios();
}

void SistemaBovedas::crearBancosIniciales() {
    // Crear los 3 bancos peruanos principales
    auto bcp = std::make_unique<Banco>("Banco de Crédito del Perú", "BCP");
    bcp->agregarBoveda(std::make_unique<Boveda>("BCP-001", "Lima Centro"));
    bcp->agregarBoveda(std::make_unique<Boveda>("BCP-002", "San Isidro"));
    bcp->agregarBoveda(std::make_unique<Boveda>("BCP-003", "Miraflores"));
    
    auto scotia = std::make_unique<Banco>("Scotiabank Perú", "SCOTIA");
    scotia->agregarBoveda(std::make_unique<Boveda>("SCOTIA-001", "Lima Centro"));
    scotia->agregarBoveda(std::make_unique<Boveda>("SCOTIA-002", "San Borja"));
    
    auto bbva = std::make_unique<Banco>("BBVA Continental", "BBVA");
    bbva->agregarBoveda(std::make_unique<Boveda>("BBVA-001", "Lima Centro"));
    bbva->agregarBoveda(std::make_unique<Boveda>("BBVA-002", "La Molina"));
    bbva->agregarBoveda(std::make_unique<Boveda>("BBVA-003", "Surco"));
    
    agregarBanco(std::move(bcp));
    agregarBanco(std::move(scotia));
    agregarBanco(std::move(bbva));
}

void SistemaBovedas::asignarActivosAleatorios() {
    // Distribución para generar valores aleatorios entre 10M y 100M USD equivalentes
    std::uniform_real_distribution<double> distribValorTotal(10000000.0, 100000000.0);
    std::uniform_real_distribution<double> distribPorcentaje(0.0, 1.0);
    
    for (auto& [codigo, banco] : bancos) {
        double valorTotalBanco = distribValorTotal(generador);
        
        // Dividir los activos entre las bóvedas del banco
        const auto& bovedas = banco->getBovedas();
        for (const auto& boveda : bovedas) {
            // Asignar una porción del valor total del banco a cada bóveda
            double valorBoveda = valorTotalBanco / bovedas.size();
            
            // Distribuir entre los tipos de activos
            double porcentajeSoles = distribPorcentaje(generador) * 0.4; // 0-40%
            double porcentajeDolares = distribPorcentaje(generador) * 0.5; // 0-50%
            double porcentajeJoyas = 1.0 - porcentajeSoles - porcentajeDolares;
            
            if (porcentajeJoyas < 0) {
                porcentajeJoyas = 0.1; // Mínimo 10% en joyas
                porcentajeDolares = 0.9 - porcentajeSoles;
            }
            
            // Convertir valores a cantidades (considerando tasas de cambio)
            double cantidadSoles = (valorBoveda * porcentajeSoles) / 0.27; // 1 USD = 3.7 PEN aprox
            double cantidadDolares = valorBoveda * porcentajeDolares;
            double cantidadJoyas = (valorBoveda * porcentajeJoyas) / 50.0; // 1 joya = 50 USD aprox
            
            // Agregar activos a la bóveda
            if (cantidadSoles > 0) {
                boveda->agregarActivo(Activo(TipoActivo::SOLES, cantidadSoles));
            }
            if (cantidadDolares > 0) {
                boveda->agregarActivo(Activo(TipoActivo::DOLARES, cantidadDolares));
            }
            if (cantidadJoyas > 0) {
                boveda->agregarActivo(Activo(TipoActivo::JOYAS, cantidadJoyas));
            }
        }
    }
}

void SistemaBovedas::agregarBanco(std::unique_ptr<Banco> banco) {
    if (!banco) {
        throw DatosInvalidosException("No se puede agregar un banco nulo");
    }
    
    std::string codigo = banco->getCodigo();
    if (bancos.find(codigo) != bancos.end()) {
        throw OperacionInvalidaException("Ya existe un banco con código: " + codigo);
    }
    
    bancos[codigo] = std::move(banco);
}

Banco* SistemaBovedas::buscarBanco(const std::string& codigo) {
    auto it = bancos.find(codigo);
    if (it == bancos.end()) {
        throw EntidadBancariaNoEncontradaException("Banco no encontrado: " + codigo);
    }
    return it->second.get();
}

const std::map<std::string, std::unique_ptr<Banco>>& SistemaBovedas::getBancos() const {
    return bancos;
}

std::string SistemaBovedas::iniciarTransferencia(const std::string& bancoOrigenCodigo,
                                               const std::string& bovedaOrigenId,
                                               const std::string& bancoDestinoCodigo,
                                               const std::string& bovedaDestinoId,
                                               TipoActivo tipoActivo,
                                               double cantidad,
                                               const std::string& transportadora,
                                               double porcentajeComision) {
    
    Activo activo(tipoActivo, cantidad);
    validarTransferencia(bancoOrigenCodigo, bovedaOrigenId, bancoDestinoCodigo, bovedaDestinoId, activo);
    
    std::string transaccionId = generarIdTransaccion();
    auto transaccion = std::make_unique<Transaccion>(
        transaccionId, bancoOrigenCodigo, bovedaOrigenId,
        bancoDestinoCodigo, bovedaDestinoId, activo, transportadora, porcentajeComision
    );
    
    transacciones.push_back(std::move(transaccion));
    return transaccionId;
}

void SistemaBovedas::procesarTransaccion(const std::string& transaccionId) {
    Transaccion* transaccion = buscarTransaccion(transaccionId);
    
    if (transaccion->estaCompletada()) {
        throw OperacionInvalidaException("La transacción ya está completada");
    }
    
    if (transaccion->getEstado() == EstadoTransaccion::PREPARACION) {
        // Retirar activos de la bóveda de origen
        Banco* bancoOrigen = buscarBanco(transaccion->getBancoOrigenCodigo());
        Boveda* bovedaOrigen = bancoOrigen->buscarBoveda(transaccion->getBovedaOrigenId());
        bovedaOrigen->retirarActivo(transaccion->getActivo());
    }
    
    // Avanzar todos los estados hasta completar
    while (!transaccion->estaCompletada() && transaccion->getEstado() != EstadoTransaccion::CANCELADA) {
        transaccion->avanzarEstado();
        
        if (transaccion->estaCompletada()) {
            // Agregar activos a la bóveda de destino (descontando comisión)
            Banco* bancoDestino = buscarBanco(transaccion->getBancoDestinoCodigo());
            Boveda* bovedaDestino = bancoDestino->buscarBoveda(transaccion->getBovedaDestinoId());
            bovedaDestino->agregarActivo(transaccion->getActivoNeto());
        }
    }
}

void SistemaBovedas::avanzarEstadoTransaccion(const std::string& transaccionId) {
    Transaccion* transaccion = buscarTransaccion(transaccionId);
    transaccion->avanzarEstado();
}

void SistemaBovedas::cancelarTransaccion(const std::string& transaccionId, const std::string& razon) {
    Transaccion* transaccion = buscarTransaccion(transaccionId);
    
    // Si la transacción ya retiró activos, devolverlos
    if (transaccion->getEstado() != EstadoTransaccion::PREPARACION && 
        transaccion->getEstado() != EstadoTransaccion::CANCELADA &&
        transaccion->getEstado() != EstadoTransaccion::COMPLETADA) {
        
        Banco* bancoOrigen = buscarBanco(transaccion->getBancoOrigenCodigo());
        Boveda* bovedaOrigen = bancoOrigen->buscarBoveda(transaccion->getBovedaOrigenId());
        bovedaOrigen->agregarActivo(transaccion->getActivo());
    }
    
    transaccion->cancelar(razon);
}

Transaccion* SistemaBovedas::buscarTransaccion(const std::string& id) {
    auto it = std::find_if(transacciones.begin(), transacciones.end(),
                          [&id](const std::unique_ptr<Transaccion>& t) {
                              return t->getId() == id;
                          });
    
    if (it == transacciones.end()) {
        throw OperacionInvalidaException("Transacción no encontrada: " + id);
    }
    
    return it->get();
}

std::vector<Transaccion*> SistemaBovedas::getTransaccionesActivas() {
    std::vector<Transaccion*> activas;
    for (const auto& transaccion : transacciones) {
        if (!transaccion->estaCompletada() && transaccion->getEstado() != EstadoTransaccion::CANCELADA) {
            activas.push_back(transaccion.get());
        }
    }
    return activas;
}

std::vector<Transaccion*> SistemaBovedas::getTodasLasTransacciones() {
    std::vector<Transaccion*> todas;
    for (const auto& transaccion : transacciones) {
        todas.push_back(transaccion.get());
    }
    return todas;
}

std::string SistemaBovedas::getResumenGeneral() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "=== SISTEMA DE BÓVEDAS ===\n\n";
    ss << "Bancos registrados: " << bancos.size() << "\n";
    ss << "Transacciones totales: " << transacciones.size() << "\n\n";
    
    double valorTotal = 0.0;
    for (const auto& [codigo, banco] : bancos) {
        valorTotal += banco->getActivosTotales();
    }
    ss << "Valor total en el sistema: $ " << valorTotal << "\n\n";
    
    return ss.str();
}

std::string SistemaBovedas::getEstadoBancos() const {
    std::stringstream ss;
    for (const auto& [codigo, banco] : bancos) {
        ss << banco->getResumen() << "\n";
    }
    return ss.str();
}

std::string SistemaBovedas::getEstadoTransacciones() const {
    std::stringstream ss;
    ss << "=== TRANSACCIONES ===\n\n";
    
    for (const auto& transaccion : transacciones) {
        ss << transaccion->getResumen() << "\n";
    }
    
    if (transacciones.empty()) {
        ss << "No hay transacciones registradas.\n";
    }
    
    return ss.str();
}

std::string SistemaBovedas::generarIdTransaccion() {
    std::stringstream ss;
    ss << "TXN-" << std::setfill('0') << std::setw(6) << contadorTransacciones++;
    return ss.str();
}

double SistemaBovedas::generarCantidadAleatoria(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(generador);
}

void SistemaBovedas::validarTransferencia(const std::string& bancoOrigenCodigo,
                                        const std::string& bovedaOrigenId,
                                        const std::string& bancoDestinoCodigo,
                                        const std::string& bovedaDestinoId,
                                        const Activo& activo) {
    
    if (bancoOrigenCodigo == bancoDestinoCodigo && bovedaOrigenId == bovedaDestinoId) {
        throw OperacionInvalidaException("La bóveda de origen no puede ser la misma que la de destino");
    }
    
    // Verificar que los bancos existen
    Banco* bancoOrigen = buscarBanco(bancoOrigenCodigo);
    Banco* bancoDestino = buscarBanco(bancoDestinoCodigo);
    
    // Verificar que las bóvedas existen
    Boveda* bovedaOrigen = bancoOrigen->buscarBoveda(bovedaOrigenId);
    Boveda* bovedaDestino = bancoDestino->buscarBoveda(bovedaDestinoId);
    
    // Verificar que la bóveda de origen tiene suficientes activos
    if (!bovedaOrigen->tieneActivo(activo)) {
        throw SaldoInsuficienteException("La bóveda de origen no tiene suficientes activos para la transferencia");
    }
}
