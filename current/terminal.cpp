#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <functional>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <iomanip>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <signal.h>
#include <pty.h>
#include <errno.h>
#include <cstring>

// Colores para la terminal
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define RED     "\033[31m"

class CustomTerminal {
private:
    int master_fd;
    int slave_fd;
    struct termios orig_termios;
    struct winsize win_size;
    volatile bool running;

    void initializeTerminal() {
        // Obtener la configuración actual de la terminal
        if (tcgetattr(STDIN_FILENO, &orig_termios) < 0) {
            std::cerr << "Error al obtener la configuración de la terminal: " << strerror(errno) << std::endl;
            exit(1);
        }

        // Obtener el tamaño actual de la terminal
        if (ioctl(STDIN_FILENO, TIOCGWINSZ, &win_size) < 0) {
            std::cerr << "Error al obtener el tamaño de la terminal: " << strerror(errno) << std::endl;
            exit(1);
        }

        // Abrir un nuevo pseudoterminal
        if (openpty(&master_fd, &slave_fd, nullptr, &orig_termios, &win_size) < 0) {
            std::cerr << "Error al abrir el pseudoterminal: " << strerror(errno) << std::endl;
            exit(1);
        }

        // Configurar el nuevo terminal
        struct termios term = orig_termios;
        
        // Configurar modo raw
        term.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        term.c_cflag &= ~(CSIZE | PARENB);
        term.c_cflag |= CS8;
        term.c_oflag &= ~(OPOST);
        
        // Establecer timeouts
        term.c_cc[VMIN] = 1;
        term.c_cc[VTIME] = 0;

        if (tcsetattr(slave_fd, TCSAFLUSH, &term) < 0) {
            std::cerr << "Error al configurar el terminal: " << strerror(errno) << std::endl;
            exit(1);
        }
    }

    void restoreTerminal() {
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) < 0) {
            std::cerr << "Error al restaurar la terminal: " << strerror(errno) << std::endl;
        }
        close(master_fd);
        close(slave_fd);
    }

    void handleInput() {
        char buf[4096];
        ssize_t n;

        while (running && (n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
            if (write(master_fd, buf, n) != n) {
                std::cerr << "Error al escribir en el master PTY: " << strerror(errno) << std::endl;
                break;
            }
        }
    }

    void handleOutput() {
        char buf[4096];
        ssize_t n;

        while (running && (n = read(master_fd, buf, sizeof(buf))) > 0) {
            if (write(STDOUT_FILENO, buf, n) != n) {
                std::cerr << "Error al escribir en stdout: " << strerror(errno) << std::endl;
                break;
            }
        }
    }

    static void handleSignal(int signo) {
        if (signo == SIGWINCH) {
            struct winsize ws;
            if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == 0) {
                ioctl(STDOUT_FILENO, TIOCSWINSZ, &ws);
            }
        }
    }

public:
    CustomTerminal() : running(true) {
        initializeTerminal();
    }

    ~CustomTerminal() {
        running = false;
        restoreTerminal();
    }

    void run() {
        pid_t pid = fork();

        if (pid < 0) {
            std::cerr << "Error en fork(): " << strerror(errno) << std::endl;
            exit(1);
        }

        if (pid == 0) {  // Proceso hijo
            close(master_fd);

            // Crear una nueva sesión
            setsid();

            // Configurar el terminal esclavo como stdin/stdout/stderr
            dup2(slave_fd, STDIN_FILENO);
            dup2(slave_fd, STDOUT_FILENO);
            dup2(slave_fd, STDERR_FILENO);

            if (slave_fd > STDERR_FILENO) {
                close(slave_fd);
            }

            // Ejecutar la shell
            // char* args[] = {(char*)"/bin/bash", nullptr};
            char* args[] = {(char*)"./myshell", nullptr};
            execvp(args[0], args);
            std::cerr << "Error al ejecutar la shell: " << strerror(errno) << std::endl;
            exit(1);
        }

        // Proceso padre
        close(slave_fd);

        // Manejar señales
        struct sigaction sa;
        sa.sa_handler = handleSignal;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGWINCH, &sa, nullptr);

        // Crear proceso para manejar la entrada
        pid_t input_pid = fork();
        if (input_pid == 0) {
            handleInput();
            exit(0);
        } else if (input_pid < 0) {
            std::cerr << "Error al crear el proceso de entrada: " << strerror(errno) << std::endl;
            exit(1);
        }

        // Manejar la salida en el proceso principal
        handleOutput();

        // Limpiar
        running = false;
        kill(input_pid, SIGTERM);
        int status;
        waitpid(input_pid, &status, 0);
        waitpid(pid, &status, 0);
    }
};

int main(int argc, char* argv[]) {
    try {
        CustomTerminal terminal;
        std::cout << CYAN << "Terminal Personalizada v1.0" << RESET << std::endl;
        std::cout << "Presiona Ctrl+D para salir\n" << std::endl;
        terminal.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}