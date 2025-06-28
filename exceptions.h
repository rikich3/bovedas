#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>    
#include <stdexcept> 

// Todas nuestras excepciones personalizadas se encuentran aca
// BovedaException hereda de std::runtime_error para integrarse
// con el ecosistema de excepciones estándar de C++.
class BovedaException : public std::runtime_error {
public:
    // Constructor que toma un mensaje de error y lo pasa a std::runtime_error
    explicit BovedaException(const std::string& message)
        : std::runtime_error(message) {
    }
};


// Excepciones relacionadas con el estado y los fondos
class SaldoInsuficienteException : public BovedaException {
public:
    explicit SaldoInsuficienteException(const std::string& message = "Error de fondos: Saldo insuficiente en la bóveda de origen.")
        : BovedaException(message) {}
};

class ActivoNoDisponibleException : public BovedaException {
public:
    explicit ActivoNoDisponibleException(const std::string& message = "Error de activo: El tipo de activo solicitado no está disponible o es insuficiente.")
        : BovedaException(message) {}
};

// Excepciones relacionadas con la validación de datos y la lógica de negocio
class OperacionInvalidaException : public BovedaException {
public:
    explicit OperacionInvalidaException(const std::string& message = "Error de operación: La operación es inválida debido a reglas de negocio o estado.")
        : BovedaException(message) {}
};

class TipoOperacionNoSoportadoException : public BovedaException {
public:
    explicit TipoOperacionNoSoportadoException(const std::string& message = "Error de tipo: El tipo de operación especificado no es soportado.")
        : BovedaException(message) {}
};

class DatosInvalidosException : public BovedaException {
public:
    explicit DatosInvalidosException(const std::string& message = "Error de datos: Los datos de entrada son inválidos o tienen formato incorrecto.")
        : BovedaException(message) {}
};

//Excepciones relacionadas con la disponibilidad y existencia de entidades
class BovedaNoEncontradaException : public BovedaException {
public:
    explicit BovedaNoEncontradaException(const std::string& message = "Error de entidad: La bóveda especificada no fue encontrada.")
        : BovedaException(message) {}
};

class EntidadBancariaNoEncontradaException : public BovedaException {
public:
    explicit EntidadBancariaNoEncontradaException(const std::string& message = "Error de entidad: La entidad bancaria especificada no fue encontrada.")
        : BovedaException(message) {}
};

class TransportadoraNoDisponibleException : public BovedaException {
public:
    explicit TransportadoraNoDisponibleException(const std::string& message = "Error de servicio: La transportadora no está disponible o no cubre la zona.")
        : BovedaException(message) {}
};

//Excepciones de sistema/internas
class ConfiguracionInvalidaException : public BovedaException {
public:
    explicit ConfiguracionInvalidaException(const std::string& message = "Error de configuración: El sistema no está configurado correctamente.")
        : BovedaException(message) {}
};

class ErrorInternoSistemaException : public BovedaException {
public:
    explicit ErrorInternoSistemaException(const std::string& message = "Error interno: Se ha producido un fallo inesperado en el sistema.")
        : BovedaException(message) {}
};

#endif // EXCEPTIONS_H
