#include "main_window.h"
#include <iomanip>
#include <sstream>

MainWindow::MainWindow() {
    set_title("Sistema Operativo GUI");
    set_default_size(800, 600);

    // Configurar el contenedor principal
    main_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    add(main_box);

    // Agregar notebook al contenedor principal
    main_box.pack_start(notebook);

    // Configurar páginas
    setup_process_page();
    setup_memory_page();
    setup_filesystem_page();
    setup_device_page();

    // Mostrar todos los widgets
    show_all();
}

void MainWindow::setup_process_page() {
    process_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    process_box.set_spacing(5);

    // Configurar la vista de árbol para procesos
    auto process_store = Gtk::ListStore::create(process_columns);
    process_tree.set_model(process_store);
    process_tree.append_column("PID", process_columns.pid);
    process_tree.append_column("Nombre", process_columns.name);
    process_tree.append_column("Memoria", process_columns.memory);

    // Configurar botones
    refresh_processes_btn.set_label("Actualizar");
    kill_process_btn.set_label("Terminar Proceso");

    refresh_processes_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::refresh_process_list));
    kill_process_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::kill_selected_process));

    // Agregar widgets a la página
    process_scroll.add(process_tree);
    process_box.pack_start(process_scroll, true, true, 0);
    process_box.pack_start(refresh_processes_btn, false, false, 0);
    process_box.pack_start(kill_process_btn, false, false, 0);

    notebook.append_page(process_box, "Procesos");
    refresh_process_list();
}

void MainWindow::refresh_process_list() {
    auto processes = processManager.listProcesses();
    auto store = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(process_tree.get_model());
    store->clear();

    for (const auto& proc : processes) {
        auto row = store->append();
        row->set_value(process_columns.pid, static_cast<unsigned int>(proc.processId));
        row->set_value(process_columns.name, Glib::ustring(proc.processName));
        row->set_value(process_columns.memory, static_cast<unsigned long>(proc.workingSetSize / 1024));
    }
}

void MainWindow::setup_memory_page() {
    memory_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    memory_box.set_spacing(5);

    update_memory_status();

    memory_box.pack_start(memory_status, false, false, 0);
    memory_box.pack_start(process_id_entry, false, false, 0);
    memory_box.pack_start(allocate_memory_btn, false, false, 0);
    memory_box.pack_start(free_memory_btn, false, false, 0);

    allocate_memory_btn.set_label("Asignar Memoria");
    free_memory_btn.set_label("Liberar Memoria");

    allocate_memory_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::allocate_memory));
    free_memory_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::free_memory));

    notebook.append_page(memory_box, "Memoria");
}

void MainWindow::update_memory_status() {
    int availableMem = freeMem();
    std::stringstream ss;
    ss << "Memoria disponible: " << availableMem << " KB";
    memory_status.set_text(ss.str());
}

void MainWindow::setup_filesystem_page() {
    filesystem_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    filesystem_box.set_spacing(5);

    // Configurar la vista de árbol para archivos
    auto file_store = Gtk::ListStore::create(file_columns);
    file_tree.set_model(file_store);
    file_tree.append_column("Nombre", file_columns.name);
    file_tree.append_column("Tipo", file_columns.type);
    file_tree.append_column("Tamaño", file_columns.size);

    // Configurar botones y entrada
    navigate_btn.set_label("Ir");
    create_dir_btn.set_label("Crear Directorio");
    delete_btn.set_label("Eliminar");

    navigate_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::navigate_to_path));
    create_dir_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::create_directory));
    delete_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::delete_selected));

    // Agregar widgets a la página
    file_scroll.add(file_tree);
    filesystem_box.pack_start(path_entry, false, false, 0);
    filesystem_box.pack_start(navigate_btn, false, false, 0);
    filesystem_box.pack_start(file_scroll, true, true, 0);
    filesystem_box.pack_start(create_dir_btn, false, false, 0);
    filesystem_box.pack_start(delete_btn, false, false, 0);

    notebook.append_page(filesystem_box, "Sistema de Archivos");
    refresh_file_list();
}

void MainWindow::refresh_file_list() {
    auto files = fileSystem.listDirectory(".");
    auto store = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(file_tree.get_model());
    store->clear();

    for (const auto& file : files) {
        auto row = store->append();
        row->set_value(file_columns.name, Glib::ustring(file.name));
        row->set_value(file_columns.type, Glib::ustring(file.isDirectory ? "Directorio" : "Archivo"));
        row->set_value(file_columns.size, static_cast<unsigned long>(file.size));
    }
}

void MainWindow::setup_device_page() {
    device_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    device_box.set_spacing(5);

    device_send_btn.set_label("Enviar");
    device_send_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::send_to_device));

    device_box.pack_start(device_output, true, true, 0);
    device_box.pack_start(device_input, false, false, 0);
    device_box.pack_start(device_send_btn, false, false, 0);

    notebook.append_page(device_box, "Dispositivos");
}

// Implementaciones para la página de procesos
void MainWindow::kill_selected_process() {
    auto selection = process_tree.get_selection();
    if (auto iter = selection->get_selected()) {
        unsigned int pid = (*iter)[process_columns.pid];
        if (processManager.terminateProcess(pid)) {
            refresh_process_list();
            Gtk::MessageDialog dialog(*this, "Proceso terminado exitosamente");
            dialog.run();
        } else {
            Gtk::MessageDialog dialog(*this, "Error al terminar el proceso", false, Gtk::MESSAGE_ERROR);
            dialog.run();
        }
    }
}

// Implementaciones para la página de memoria
void MainWindow::allocate_memory() {
    try {
        int process_id = std::stoi(process_id_entry.get_text());
        if (memoryAllocation(process_id)) {
            update_memory_status();
            Gtk::MessageDialog dialog(*this, "Memoria asignada exitosamente");
            dialog.run();
        } else {
            Gtk::MessageDialog dialog(*this, "Error al asignar memoria", false, Gtk::MESSAGE_ERROR);
            dialog.run();
        }
    } catch (const std::exception& e) {
        Gtk::MessageDialog dialog(*this, "ID de proceso inválido", false, Gtk::MESSAGE_ERROR);
        dialog.run();
    }
}

void MainWindow::free_memory() {
    try {
        int process_id = std::stoi(process_id_entry.get_text());
        releaseMemory(process_id);
        update_memory_status();
        Gtk::MessageDialog dialog(*this, "Memoria liberada exitosamente");
        dialog.run();
    } catch (const std::exception& e) {
        Gtk::MessageDialog dialog(*this, "ID de proceso inválido", false, Gtk::MESSAGE_ERROR);
        dialog.run();
    }
}

// Implementaciones para la página del sistema de archivos
void MainWindow::navigate_to_path() {
    std::string new_path = path_entry.get_text();
    if (fileSystem.changeDirectory(new_path)) {
        refresh_file_list();
        path_entry.set_text(fileSystem.getCurrentDirectory());
    } else {
        Gtk::MessageDialog dialog(*this, "Error al cambiar de directorio", false, Gtk::MESSAGE_ERROR);
        dialog.run();
    }
}

void MainWindow::create_directory() {
    Gtk::Dialog dialog("Crear Directorio", *this);
    Gtk::Entry name_entry;
    dialog.get_content_area()->pack_start(name_entry);
    dialog.add_button("Cancelar", Gtk::RESPONSE_CANCEL);
    dialog.add_button("Crear", Gtk::RESPONSE_OK);
    dialog.show_all();

    if (dialog.run() == Gtk::RESPONSE_OK) {
        std::string dir_name = name_entry.get_text();
        if (fileSystem.createDirectory(dir_name)) {
            refresh_file_list();
        } else {
            Gtk::MessageDialog error_dialog(*this, "Error al crear directorio", false, Gtk::MESSAGE_ERROR);
            error_dialog.run();
        }
    }
}

void MainWindow::delete_selected() {
    auto selection = file_tree.get_selection();
    if (auto iter = selection->get_selected()) {
        // Convertir explícitamente de Glib::ustring a std::string
        std::string name = static_cast<Glib::ustring>((*iter)[file_columns.name]);
        std::string type = static_cast<Glib::ustring>((*iter)[file_columns.type]);

        Gtk::MessageDialog confirm(*this, 
            "¿Está seguro de eliminar " + name + "?",
            false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);

        if (confirm.run() == Gtk::RESPONSE_YES) {
            bool success;
            if (type == "Directorio") {
                success = fileSystem.removeDirectory(name);
            } else {
                success = fileSystem.deleteFile(name);
            }

            if (success) {
                refresh_file_list();
            } else {
                Gtk::MessageDialog error(*this, "Error al eliminar", false, Gtk::MESSAGE_ERROR);
                error.run();
            }
        }
    }
}

// Implementación para la página de dispositivos
void MainWindow::send_to_device() {
    std::string input = device_input.get_text();
    if (!input.empty()) {
        IORequest request;
        request.processId = 1;
        request.device = DeviceType::Display;
        request.operation = OperationType::Write;
        request.data = input;

        deviceManager.submitRequest(request);
        deviceManager.processRequests();

        // Actualizar la salida
        auto buffer = device_output.get_buffer();
        buffer->insert(buffer->end(), input + "\n");
        
        // Limpiar entrada
        device_input.set_text("");
    }
}