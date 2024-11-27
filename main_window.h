#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtkmm.h>
#include "DeviceManager.h"
#include "MemoryManager.h"
#include "ProcessManager.h"
#include "FileSystemManager.h"

class ProcessColumns : public Gtk::TreeModel::ColumnRecord {
public:
    ProcessColumns() {
        add(pid);
        add(name);
        add(memory);
    }

    Gtk::TreeModelColumn<unsigned int> pid;     // Cambiado a unsigned int
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<unsigned long> memory; // Cambiado a unsigned long
};


// Modelo de columnas para la lista de archivos
class FileColumns : public Gtk::TreeModel::ColumnRecord {
public:
    FileColumns() {
        add(name);
        add(type);
        add(size);
    }

    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> type;
    Gtk::TreeModelColumn<unsigned long> size;
};

class MainWindow : public Gtk::Window {
public:
    MainWindow();

private:
    // Managers
    DeviceManager deviceManager;
    ProcessManager processManager;
    FileSystemManager fileSystem;
    ProcessColumns process_columns;
    FileColumns file_columns;

    // Widgets principales
    Gtk::Notebook notebook;
    Gtk::Box main_box;

    // Páginas para cada subsistema
    void setup_process_page();
    void setup_memory_page();
    void setup_filesystem_page();
    void setup_device_page();

    // Widgets para la página de procesos
    Gtk::Box process_box;
    Gtk::TreeView process_tree;
    Gtk::Button refresh_processes_btn;
    Gtk::Button kill_process_btn;
    Gtk::ScrolledWindow process_scroll;
    Glib::RefPtr<Gtk::ListStore> process_store;
    void refresh_process_list();
    void kill_selected_process();

    // Widgets para la página de memoria
    Gtk::Box memory_box;
    Gtk::Label memory_status;
    Gtk::Entry process_id_entry;
    Gtk::Button allocate_memory_btn;
    Gtk::Button free_memory_btn;
    void update_memory_status();
    void allocate_memory();
    void free_memory();

    // Widgets para la página del sistema de archivos
    Gtk::Box filesystem_box;
    Gtk::TreeView file_tree;
    Gtk::Entry path_entry;
    Gtk::Button navigate_btn;
    Gtk::Button create_dir_btn;
    Gtk::Button delete_btn;
    Gtk::ScrolledWindow file_scroll;
    Glib::RefPtr<Gtk::ListStore> file_store;
    void refresh_file_list();
    void navigate_to_path();
    void create_directory();
    void delete_selected();

    // Widgets para la página de dispositivos
    Gtk::Box device_box;
    Gtk::TextView device_output;
    Gtk::Entry device_input;
    Gtk::Button device_send_btn;
    void send_to_device();
};

#endif // MAIN_WINDOW_H