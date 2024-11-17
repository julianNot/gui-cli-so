# Shell Personalizada en C++

Una implementación básica de una shell interactiva desarrollada en C++. Esta shell proporciona funcionalidades básicas de navegación y manipulación de archivos en sistemas Unix/Linux.

## Características

- Interfaz de línea de comandos interactiva
- Sistema de comandos modular y extensible
- Soporte para comandos básicos del sistema de archivos:
  - `ls`: Listar contenido de directorios
  - `cd`: Cambiar directorio actual
  - `mkdir`: Crear directorios
  - `clear`: Limpiar pantalla
  - `help`: Mostrar ayuda de comandos
  - `exit`: Salir de la shell

## Requisitos del Sistema

- Sistema operativo Unix/Linux
- Compilador G++ con soporte para C++14
- Make

## Estructura del Proyecto

```
.
├── src/
│   ├── main.cpp
│   ├── Shell.cpp
│   └── commands/
│       ├── CdCommand.cpp
│       ├── ClearCommand.cpp
│       ├── ExitCommand.cpp
│       ├── HelpCommand.cpp
│       ├── LsCommand.cpp
│       └── MkdirCommand.cpp
├── include/
│   ├── Shell.hpp
│   ├── Command.hpp
│   ├── Colors.hpp
│   └── commands/
│       ├── CdCommand.hpp
│       ├── ClearCommand.hpp
│       ├── ExitCommand.hpp
│       ├── HelpCommand.hpp
│       ├── LsCommand.hpp
│       └── MkdirCommand.hpp
├── Makefile
└── README.md
```

## Compilación

Para compilar el proyecto:

```bash
# Crear directorios necesarios
make check-structure

# Compilar el proyecto
make

# Limpiar archivos compilados
make clean
```

## Uso

Para ejecutar la shell:

```bash
./bin/myshell
```

### Comandos Disponibles

- `ls [-a] [ruta]`: Lista el contenido del directorio
  - `-a`: Muestra archivos ocultos
  
- `cd [ruta]`: Cambia el directorio actual
  - Sin argumentos: cambia al directorio home
  
- `mkdir [-p] directorio`: Crea un nuevo directorio
  - `-p`: Crea directorios padres si no existen
  
- `clear`: Limpia la pantalla

- `help [comando]`: Muestra ayuda
  - Sin argumentos: lista todos los comandos
  - Con argumento: muestra ayuda detallada del comando

- `exit`: Salir de la shell

## Diseño y Arquitectura

- Diseño modular basado en el patrón Command
- Sistema extensible para agregar nuevos comandos
- Manejo de errores y excepciones
- Interfaz de usuario con colores para mejor legibilidad

## Limitaciones

- Solo funciona en sistemas Unix/Linux
- No soporta pipes o redirección
- Funcionalidad básica de comandos
- No tiene historial de comandos

## Contribuir

Para agregar nuevos comandos:

1. Crear archivos header y source en los directorios correspondientes
2. Heredar de la clase base `Command`
3. Implementar los métodos virtuales requeridos
4. Registrar el nuevo comando en `Shell::initCommands()`

## Licencia

Este proyecto está disponible bajo la licencia MIT.

## Notas de Desarrollo

- Desarrollado usando C++14
- Uso de smart pointers para gestión de memoria
- Código documentado para fácil comprensión
- Estructura modular para fácil extensión
