#ifndef BANCO_H
#define BANCO_H

#include "boveda.h"
#include <vector>
#include <memory>
#include <string>

class Banco {
private:
    std::string nombre;
    std::string codigo;
    std::vector<std::unique_ptr<Boveda>> bovedas;

public:
    Banco(const std::string& nombre, const std::string& codigo);
    ~Banco() = default;
    
    // Getters
    std::string getNombre() const;
    std::string getCodigo() const;
    const std::vector<std::unique_ptr<Boveda>>& getBovedas() const;
    
    // Manejo de bóvedas
    void agregarBoveda(std::unique_ptr<Boveda> boveda);
    Boveda* buscarBoveda(const std::string& idBoveda);
    const Boveda* buscarBoveda(const std::string& idBoveda) const;
    
    // Cálculos
    double getActivosTotales() const;
    std::string getResumen() const;
    
    // Operaciones
    void transferirEntreBovedas(const std::string& bovedaOrigenId, 
                               const std::string& bovedaDestinoId, 
                               const Activo& activo);
};

#endif // BANCO_H
