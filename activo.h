#ifndef ACTIVO_H
#define ACTIVO_H

#include <string>

enum class TipoActivo {
    SOLES,
    DOLARES,
    JOYAS
};

class Activo {
private:
    TipoActivo tipo;
    double cantidad;

public:
    Activo(TipoActivo tipo, double cantidad);
    
    TipoActivo getTipo() const;
    double getCantidad() const;
    void setCantidad(double cantidad);
    
    std::string getTipoString() const;
    static std::string tipoActivoToString(TipoActivo tipo);
    static TipoActivo stringToTipoActivo(const std::string& str);
    
    // Operadores para facilitar el manejo
    Activo operator+(const Activo& otro) const;
    Activo operator-(const Activo& otro) const;
    bool operator>=(const Activo& otro) const;
    bool operator<(const Activo& otro) const;
};

#endif // ACTIVO_H
