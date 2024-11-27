# Sistema Operativo - CLI y GUI

Este proyecto implementa una interfaz para simular funcionalidades básicas de un sistema operativo, incluyendo:
- Gestión de procesos
- Gestión de memoria
- Sistema de archivos
- Gestión de dispositivos

## Versiones

### CLI (Command Line Interface)
Interfaz de línea de comandos que permite interactuar con el sistema mediante comandos específicos.

### GUI (Graphical User Interface)
Interfaz gráfica que proporciona una experiencia más visual e intuitiva usando GTK.

## Requisitos

### Para la versión CLI:
- G++ o compilador C++ compatible
- C++17 o superior
- Windows (para las APIs nativas del sistema)

### Para la versión GUI (adicional):
- MSYS2
- GTK3 y gtkmm-3.0
- pkg-config

## Instalación

1. Instalar MSYS2 (requerido para GUI):
```bash
# Descargar e instalar MSYS2 desde https://www.msys2.org/
```

2. Instalar dependencias (desde MSYS2 MINGW64):
```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-gtkmm3
pacman -S mingw-w64-x86_64-pkg-config
```

3. Agregar al PATH de Windows:
```
C:\msys64\mingw64\bin
```

## Estructura del Proyecto
```
projects_so/
├── DeviceManager/
│   ├── DeviceManager.h
│   ├── DeviceManager.cpp
│   └── IORequest.h
├── MemoryManager/
│   ├── MemoryManager.h
│   ├── MemoryManager.cpp
│   ├── RAM.json
│   └── Swap.json
├── ProcessManager/
│   ├── ProcessManager.h
│   └── ProcessManager.cpp
├── FileSystemManager/
│   ├── FileSystemManager.h
│   └── FileSystemManager.cpp
└── src/
    ├── cli.cpp
    ├── main_window.h     (GUI)
    ├── main_window.cpp   (GUI)
    └── main.cpp          (GUI)
```

## Compilación

### Versión CLI:
```bash
g++ -o os_cli.exe src/cli.cpp \
    projects_so/MemoryManager/MemoryManager.cpp \
    projects_so/DeviceManager/DeviceManager.cpp \
    projects_so/ProcessManager/ProcessManager.cpp \
    projects_so/FileSystemManager/FileSystemManager.cpp \
    -I projects_so/DeviceManager \
    -I projects_so/MemoryManager \
    -I projects_so/ProcessManager \
    -I projects_so/FileSystemManager \
    -std=c++17
```

### Versión GUI (desde MSYS2 MINGW64):
```bash
g++ -o os_gui.exe main.cpp main_window.cpp \
    projects_so/MemoryManager/MemoryManager.cpp \
    projects_so/DeviceManager/DeviceManager.cpp \
    projects_so/ProcessManager/ProcessManager.cpp \
    projects_so/FileSystemManager/FileSystemManager.cpp \
    $(pkg-config --cflags --libs gtkmm-3.0) \
    -I projects_so/DeviceManager \
    -I projects_so/MemoryManager \
    -I projects_so/ProcessManager \
    -I projects_so/FileSystemManager \
    -std=c++17
```

## Uso

### CLI - Comandos Disponibles:

#### Procesos:
```bash
process list                # Lista todos los procesos
process create <ruta>      # Crea un nuevo proceso
process kill <PID>         # Termina un proceso
process suspend <PID>      # Suspende un proceso
process resume <PID>       # Reanuda un proceso
process info <PID>         # Muestra información del proceso
```

#### Memoria:
```bash
memory allocate <process_id>                # Asigna memoria a un proceso
memory free <process_id>                    # Libera memoria de un proceso
memory swap <segment_id> <page> <process_id># Realiza swap de una página
memory status                               # Muestra el estado de la memoria
```

#### Sistema de Archivos:
```bash
fs pwd                     # Muestra el directorio actual
fs cd <ruta>              # Cambia de directorio
fs ls [ruta]              # Lista contenido del directorio
fs mkdir <nombre>         # Crea un directorio
fs rmdir <nombre>         # Elimina un directorio
fs touch <nombre>         # Crea un archivo
fs rm <nombre>            # Elimina un archivo
fs cp <origen> <destino>  # Copia un archivo
fs mv <origen> <destino>  # Mueve un archivo
```

#### Dispositivos:
```bash
device write storage <datos> <dirección>  # Escribe en almacenamiento
device write display <texto>              # Escribe en pantalla
device read storage <dirección>           # Lee del almacenamiento
device read keyboard                      # Lee del teclado
device status                            # Muestra estado del display
```

### GUI:
La interfaz gráfica proporciona pestañas para cada subsistema con botones y controles intuitivos para realizar las mismas operaciones que la CLI.

## Notas
- Asegúrate de que los archivos JSON (RAM.json y Swap.json) estén en la ubicación correcta antes de ejecutar el programa
- La versión GUI requiere que MSYS2 esté correctamente instalado y configurado
- Los comandos deben ejecutarse desde la terminal MINGW64 para la versión GUI

## Limitaciones
- La GUI solo está disponible en Windows con MSYS2
- Algunas funcionalidades pueden requerir permisos de administrador
