# Ice Sliding Puzzle Solver

Program C++ untuk menyelesaikan permainan *ice sliding puzzle* dengan berbagai algoritma pencarian (A*, Weighted A*, UCS, GBFS). Program mendukung dua mode:
- **GUI** — visualisasi interaktif berbasis raylib (dengan pemutar solusi langkah demi langkah)
- **CLI** — mode terminal untuk input/output file

## Anggota

| Nama | NIM |
| --- | --- |
| Agatha Tatianingseto | 13524008 |
| Muhammad Nur Majiid | 13524028 |

---

## Dependencies

| Dependency | Versi | Keterangan |
|------------|-------|------------|
| CMake | ≥ 3.16 | Build system |
| C++ Compiler | C++17 | g++ / clang++ |
| raylib | ≥ 4.0 | **Opsional** — hanya untuk mode GUI |
| Ninja | — | **Opsional** — lebih cepat dari Make |

### Mode CLI
Mode CLI (tanpa GUI) **tidak memerlukan raylib**. Semua OS dapat menjalankan mode CLI asalkan memiliki compiler C++17 dan CMake.

### Mode GUI
Mode GUI memerlukan **raylib** dan sebuah *file dialog backend* sesuai OS:
- **Windows** — tidak memerlukan dependency tambahan (menggunakan Win32 API)
- **Linux** — memerlukan `zenity` atau `kdialog`
- **macOS** — tidak memerlukan dependency tambahan (menggunakan AppleScript)

---

## Instalasi Dependencies

### Windows

1. **Install raylib + w64devkit**
   - Download dari [raylib.com](https://www.raylib.com/) atau ekstrak ke `C:\raylib\w64devkit`
   - Pastikan ada folder `C:\raylib\w64devkit\bin\g++.exe`

2. **Install CMake**
   - Download dari [cmake.org](https://cmake.org/download/) atau via winget:
     ```powershell
     winget install Kitware.CMake
     ```

3. **(Opsional) Install Ninja**
   ```powershell
   winget install Ninja-build.Ninja
   ```

### Linux (Ubuntu / Debian)

```bash
# Compiler & build tools
sudo apt update
sudo apt install -y cmake build-essential ninja-build

# raylib (untuk GUI)
sudo apt install -y libgl1-mesa-dev libx11-dev libxcursor-dev libxinerama-dev libxi-dev libxrandr-dev
# Build raylib dari source atau install dari repo

# File dialog untuk GUI (pilih salah satu)
sudo apt install -y zenity        # rekomendasi
# sudo apt install -y kdialog     # alternatif
```

### Linux (Arch / Manjaro)

```bash
sudo pacman -S cmake gcc ninja
# raylib
sudo pacman -S raylib
# File dialog
sudo pacman -S zenity
```

### macOS

```bash
# Install Homebrew (jika belum ada)
# https://brew.sh

# Compiler & build tools
xcode-select --install
brew install cmake ninja

# raylib (untuk GUI)
brew install raylib
```

---

## Cara Build & Run

> **Catatan:** Setiap kali menambahkan file `.cpp` baru, tidak perlu mengubah `CMakeLists.txt` karena source file dicari secara otomatis dengan `GLOB_RECURSE`.

### Windows

```powershell
# Configure (otomatis detect w64devkit jika ada di C:\raylib\w64devkit)
cmake -S . -B build -G Ninja

# Build
cmake --build build --config Release

# Run
.\bin\game.exe
```

**Atau** via target `run`:
```powershell
cmake --build build --target run
```

### Linux / macOS

```bash
# Configure
cmake -S . -B build -G Ninja

# Build
cmake --build build --config Release

# Run
./bin/game
```

**Atau** via target `run`:
```bash
cmake --build build --target run
```

### Build Tanpa GUI (CLI Only)

Jika raylib tidak terinstall, CMake akan otomatis build dalam mode CLI-only tanpa GUI. Tidak perlu konfigurasi tambahan.

```bash
cmake -S . -B build
cmake --build build
```

---

## Cara Menggunakan Program

### Mode GUI
Saat program berjalan dalam mode GUI (default), gunakan tombol dan panel yang tersedia:
1. Klik **Load File** atau ketik nama file pada kolom input
2. Pilih algoritma (A*, WA*, UCS, GBFS) dan heuristik (H1, H2, H3)
3. Klik **Solve** untuk mencari solusi
4. Gunakan tombol navigasi (▶, ⏸, ⏮, ⏭) untuk melihat animasi solusi
5. Klik **View Board** untuk melihat papan dalam tampilan penuh

### Mode CLI
Untuk menjalankan mode CLI, tambahkan flag saat menjalankan program:

```bash
# Windows
.\bin\game.exe --cli
# atau
.\bin\game.exe -c

# Linux / macOS
./bin/game --cli
# atau
./bin/game -c
```

---

## Troubleshooting

### `raylib not found. Building without GUI support.`
Raylib tidak terdeteksi. Jika ingin GUI, pastikan raylib terinstall dan CMake dapat menemukan header `raylib.h` serta library `libraylib.a` / `libraylib.so`.

### Asset tidak muncul di GUI
Pastikan folder `assets/` berada di direktori yang sama dengan executable, atau jalankan program dari root project (bukan dari dalam folder `bin/`). Saat build dengan CMake, folder `assets/` akan otomatis disalin ke folder output.

### `ninja: build stopped: subcommand failed` pada Windows
Pastikan menggunakan compiler dari **w64devkit** (GCC 15.x), bukan dari MinGW lain yang versinya berbeda. `CMakeLists.txt` sudah otomatis mendeteksi `C:\raylib\w64devkit`.

---

## Struktur Direktori

```
.
├── assets/          # Gambar & resource GUI
├── bin/             # Output executable (auto-generated)
├── build/           # File build CMake (auto-generated)
├── docs/            # Dokumentasi tambahan
├── include/         # Header files (.hpp)
├── src/             # Source files (.cpp)
│   ├── cli/         # Implementasi mode CLI
│   ├── gui/         # Implementasi mode GUI
│   ├── solver/      # Algoritma pencarian
│   ├── heuristic/   # Fungsi heuristik
│   ├── model/       # Struktur data (GameMap, State, dll.)
│   └── utils/       # Parser, validator, engine
├── CMakeLists.txt   # Konfigurasi build
└── README.md        # Dokumentasi ini
```
