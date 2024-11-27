#include "DeviceManager.h"
#include <iostream>

// Enviar una solicitud de I/O al Administrador de Dispositivos
void DeviceManager::submitRequest(const IORequest& request) {
    std::lock_guard<std::mutex> lock(queueMutex);
    ioQueue.push(request);
}

// Procesar todas las solicitudes de I/O pendientes
void DeviceManager::processRequests() {
    while (true) {
        IORequest request;

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (ioQueue.empty()) {
                break;
            }
            request = ioQueue.front();
            ioQueue.pop();
        }

        switch (request.device) {
            case DeviceType::Storage:
                handleStorage(request);
                break;
            case DeviceType::Keyboard:
                handleKeyboard(request);
                break;
            case DeviceType::Display:
                handleDisplay(request);
                break;
        }
    }
}

// Manejar operaciones de almacenamiento
void DeviceManager::handleStorage(const IORequest& request) {
    if (request.operation == OperationType::Read) {
        std::string data = readStorage(request.address);
        std::cout << "Proceso " << request.processId << " leyó de almacenamiento: " << data << std::endl;
    } else if (request.operation == OperationType::Write) {
        writeStorage(request.address, request.data);
        std::cout << "Proceso " << request.processId << " escribió en almacenamiento en la dirección "
                  << request.address << ": " << request.data << std::endl;
    }
}

// Manejar operaciones de teclado
void DeviceManager::handleKeyboard(const IORequest& request) {
    if (request.operation == OperationType::Read) {
        std::string input = getKeyboardBuffer();
        std::cout << "Proceso " << request.processId << " leyó del teclado: " << input << std::endl;
    }
}

// Manejar operaciones de pantalla
void DeviceManager::handleDisplay(const IORequest& request) {
    if (request.operation == OperationType::Write) {
        writeToDisplay(request.data);
        std::cout << "Proceso " << request.processId << " escribió en la pantalla: " << request.data << std::endl;
    }
}

// Métodos para simular el buffer del teclado
void DeviceManager::setKeyboardBuffer(const std::string& input) {
    keyboardBuffer = input;
}

std::string DeviceManager::getKeyboardBuffer() {
    std::string temp = keyboardBuffer;
    keyboardBuffer.clear(); // Limpiar el buffer después de leer
    return temp;
}

// Métodos para simular la pantalla
void DeviceManager::writeToDisplay(const std::string& text) {
    displayBuffer += text + "\n"; // Añade una nueva línea por cada escritura
}

std::string DeviceManager::getDisplayBuffer() {
    return displayBuffer;
}

// Métodos para simular el almacenamiento
void DeviceManager::writeStorage(int address, const std::string& data) {
    if(address >= 0 && address < 100) { // Verifica que la dirección esté dentro del rango
        storage[address] = data;
    } else {
        std::cerr << "Error: Dirección de almacenamiento inválida." << std::endl;
    }
}

std::string DeviceManager::readStorage(int address) {
    if(address >= 0 && address < 100) { // Verifica que la dirección esté dentro del rango
        return storage[address];
    } else {
        std::cerr << "Error: Dirección de almacenamiento inválida." << std::endl;
        return "";
    }
}