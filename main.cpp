#include "main_window.h"

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.example.osystem");
    MainWindow window;
    return app->run(window);
}