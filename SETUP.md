# Configuración por Sistema Operativo

## 🐧 Linux (Ubuntu/Fedora)

### Ubuntu 20.04/22.04/24.04
```bash
# Instalar Qt
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential

# Ruta típica de Qt
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/qt6")
```

### Fedora 38/39/40
```bash
# Instalar Qt
sudo dnf install qt6-qtbase-devel qt6-qttools-devel cmake gcc-c++

# Ruta típica de Qt
set(CMAKE_PREFIX_PATH "/usr/lib64/qt6")
```

### Instalación manual de Qt
```bash
# Descargar desde qt.io
# Ruta típica después de instalación manual:
set(CMAKE_PREFIX_PATH "/home/usuario/Qt/6.9.0/gcc_64")
```

## 🪟 Windows 10/11

### Con Qt Online Installer
```cmake
# Ruta típica con Visual Studio 2019
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/msvc2019_64")

# Ruta típica con Visual Studio 2022  
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/msvc2022_64")

# Ruta típica con MinGW
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/mingw_64")
```

### Variables de entorno (opcional)
```cmd
set Qt6_DIR=C:\Qt\6.9.0\msvc2019_64\lib\cmake\Qt6
set PATH=%PATH%;C:\Qt\6.9.0\msvc2019_64\bin
```

## 🍎 macOS

### Con Homebrew
```bash
brew install qt6 cmake

# Ruta típica con Homebrew
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt6")
```

### Instalación manual
```cmake
# Ruta típica después de instalación manual
set(CMAKE_PREFIX_PATH "/Users/usuario/Qt/6.9.0/macos")
```

## 🔍 Encontrar tu ruta de Qt

### Método 1: Qt Creator
1. Abrir Qt Creator
2. `Edit` → `Preferences` → `Qt Versions`
3. Copiar ruta de "qmake location"
4. Remover `/bin/qmake` del final

### Método 2: Terminal/CMD
```bash
# Linux/macOS
which qmake6
find /usr -name "Qt6Config.cmake" 2>/dev/null

# Windows (PowerShell)
where qmake
Get-ChildItem -Path "C:\Qt" -Recurse -Name "Qt6Config.cmake"
```

### Método 3: Variables de entorno
```bash
echo $Qt6_DIR
echo $CMAKE_PREFIX_PATH
```

## ⚠️ Problemas Comunes

### "Qt6Config.cmake not found"
- Verificar que CMAKE_PREFIX_PATH apunte al directorio correcto
- Asegurar que Qt6 esté instalado completamente
- En Windows, verificar que el compilador coincida (msvc2019 vs msvc2022)

### Compilador incompatible
- Linux: Usar gcc 7+ para C++17
- Windows: Visual Studio 2017+ o MinGW 7+
- macOS: Xcode 10+ o Clang 7+

### Versión de Qt incompatible
- Usar Qt 6.0 o superior
- Qt 5.15+ también funciona con modificaciones menores
- Evitar Qt versiones anteriores a 5.12
