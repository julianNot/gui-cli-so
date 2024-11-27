#ifndef IOREQUEST_H
#define IOREQUEST_H

#include <string>

// Enumeración para los tipos de dispositivos soportados
enum class DeviceType {
    Storage,   // Almacenamiento
    Keyboard,  // Teclado
    Display    // Pantalla
};

// Enumeración para los tipos de operaciones de I/O
enum class OperationType {
    Read,   // Leer
    Write   // Escribir
};

// Estructura que representa una solicitud de I/O
struct IORequest {
    int processId;                // ID único del proceso que realiza la solicitud
    DeviceType device;            // Tipo de dispositivo (Storage, Keyboard, Display)
    OperationType operation;      // Tipo de operación (Read, Write)
    std::string data;             // Datos a escribir (solo para operaciones de Write)
    int address;                  // Dirección en el almacenamiento (solo para Storage)
};

#endif // IOREQUEST_H