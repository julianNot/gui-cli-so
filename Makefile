# Compilador
CXX = g++

# Flags de compilación (cambiado de -std=c++11 a -std=c++14)
CXXFLAGS = -std=c++14 -Wall -Wextra -I include

# Directorios
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Nombre del ejecutable
TARGET = $(BINDIR)/myshell

# Obtener todos los archivos fuente automáticamente
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
SOURCES += $(wildcard $(SRCDIR)/commands/*.cpp)

# Generar lista de archivos objeto correspondientes
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Obtener todos los directorios que contienen archivos fuente
DIRS := $(dir $(OBJECTS))

# Crear directorios necesarios
$(shell mkdir -p $(BINDIR) $(OBJDIR) $(DIRS))

# Regla principal
all: $(TARGET)

# Regla para crear el ejecutable
$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@$(CXX) $(OBJECTS) -o $(TARGET)
	@echo "Build successful! Binary created at $(TARGET)"

# Regla para compilar los archivos objeto
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	@echo "Cleaning..."
	@rm -rf $(OBJDIR)/* $(BINDIR)/*
	@echo "Clean complete"

# Phony targets
.PHONY: all clean