# ==== Platform detection ====
# ==== Platform detection ====
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        PLATFORM := macOS
    else
        PLATFORM := Linux
    endif
endif

# Default target shows quick help
.DEFAULT_GOAL := help

CXX ?= clang++

# ==== Common compiler flags ====
CXXFLAGS_COMMON = -std=c++17 -g \
                  -Iinclude \
                  -Ithird_party/imgui \
                  -Ithird_party/imgui/backends \
                  -Ithird_party/tinyfiledialogs

# ==== Platform‑specific overrides ====
ifeq ($(PLATFORM),macOS)
    CXXFLAGS = $(CXXFLAGS_COMMON) -I/opt/homebrew/include -DGL_SILENCE_DEPRECATION
    LDFLAGS  = -L/opt/homebrew/lib -lglfw \
               -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
else ifeq ($(PLATFORM),Windows)
    CXXFLAGS = $(CXXFLAGS_COMMON)
    LDFLAGS  = -lglfw3 -lopengl32 -lgdi32 -static
else   # Linux
    CXXFLAGS = $(CXXFLAGS_COMMON) -pthread
    LDFLAGS  = -lglfw -lGL -lX11 -lpthread -ldl
endif

SRCS = src/gui_main.cpp src/des.cpp \
       third_party/imgui/imgui.cpp \
       third_party/imgui/imgui_draw.cpp \
       third_party/imgui/imgui_widgets.cpp \
       third_party/imgui/imgui_tables.cpp \
       third_party/imgui/imgui_demo.cpp \
       third_party/imgui/backends/imgui_impl_glfw.cpp \
       third_party/imgui/backends/imgui_impl_opengl3.cpp \
       third_party/tinyfiledialogs/tinyfiledialogs.c
TARGET = dist/des_gui_app

# Alias: 'all' builds the application
.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -rf dist/*

.PHONY: run
run: $(TARGET)
	@echo "Launching DES GUI..."
	@$(TARGET)

.PHONY: help
help:
	@echo ""
	@echo "DES GUI – cross‑platform Makefile"
	@echo "---------------------------------"
	@echo "make            → Show this help message"
	@echo "make all        → Build the application"
	@echo "make run        → Build (if needed) and launch the application"
	@echo "make package    → Build and create platform‑specific installer/portable archive"
	@echo "make clean      → Remove build artifacts"
	@echo ""
	@echo "Tip: On a fresh clone, simply run 'make run' to compile then execute."

.PHONY: package
package: $(TARGET)
ifeq ($(PLATFORM),macOS)
	@echo "Packaging macOS .app bundle..."
	@mkdir -p dist/DES_GUI.app/Contents/MacOS
	@cp $(TARGET) dist/DES_GUI.app/Contents/MacOS/DES_GUI
	@echo "Bundle created at dist/DES_GUI.app"
else ifeq ($(PLATFORM),Windows)
	@echo "Packaging Windows zip..."
	@mkdir -p dist
	@zip -j dist/des_gui-win64.zip $(TARGET)
	@echo "Zip created at dist/des_gui-win64.zip"
else
	@echo "Packaging Linux AppImage stub..."
	@echo "Run scripts/build_linux.sh for full AppImage generation."
endif