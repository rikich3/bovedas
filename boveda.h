#ifndef BOVEDA_H
#define BOVEDA_H

#include "activo.h"
#include <map>
#include <string>

class Boveda {
private:
    std::string id;
    std::string ubicacion;
    std::map<TipoActivo, double> activos;

public:
    Boveda(const std::string& id, const std::string& ubicacion);
    
    // Getters
    std::string getId() const;
    std::string getUbicacion() const;
    double getSaldo(TipoActivo tipo) const;
    std::map<TipoActivo, double> getTodosLosActivos() const;
    
    // Operaciones con activos
    void agregarActivo(const Activo& activo);
    void retirarActivo(const Activo& activo);
    bool tieneActivo(const Activo& activo) const;
    
    // Cálculo del valor total en dólares (asumiendo conversiones)
    double getValorTotalEnDolares() const;
    
    // Información para mostrar
    std::string getResumen() const;
};

#endif // BOVEDA_H
