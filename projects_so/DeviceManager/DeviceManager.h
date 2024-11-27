#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "IORequest.h"
#include <queue>
#include <mutex>
#include <string>

class DeviceManager {
public:
    // Métodos públicos
    void submitRequest(const IORequest& request);
    void processRequests();

    // Métodos para simular entradas y salidas de dispositivos
    void setKeyboardBuffer(const std::string& input);
    std::string getKeyboardBuffer();

    void writeToDisplay(const std::string& text);
    std::string getDisplayBuffer();

    void writeStorage(int address, const std::string& data);
    std::string readStorage(int address);

private:
    std::queue<IORequest> ioQueue;
    std::mutex queueMutex;

    // Métodos privados para manejar los dispositivos
    void handleStorage(const IORequest& request);
    void handleKeyboard(const IORequest& request);
    void handleDisplay(const IORequest& request);

    // Simulación de dispositivos
    std::string storage[100];    // Almacenamiento simulado
    std::string keyboardBuffer;  // Buffer del teclado
    std::string displayBuffer;   // Buffer de la pantalla
};

#endif // DEVICEMANAGER_H