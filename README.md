# Game 2048 (C++ / SFML)

## Kurze Projekterklärung

Dieses Projekt ist eine **Desktop-Implementierung des Spiels 2048** in **C++17**. Das Spielfeld ist ein **4×4-Raster**; gleichwertige Kacheln verschmelzen bei einer Bewegung, nach jedem gültigen Zug erscheint eine neue Kachel (2 oder 4). Das Spiel endet, wenn kein Zug mehr möglich ist. Die Grafik, das Fenster und die Eingaben werden mit der Bibliothek **SFML 3** umgesetzt; Build und Abhängigkeiten werden über **CMake** verwaltet. Der Code ist in **Spiellogik** (`Game2048`), **Darstellung/Eingabe** (`GameView`) und **Hauptprogramm** (`main`) getrennt.

---

## Drittleistungen

Alle verwendeten Fremdleistungen (Bibliotheken, Schriftarten, Werkzeuge von Dritten):

| Nr. | Drittleistung | Verwendung im Projekt | Quelle / Lizenz |
|-----|----------------|------------------------|-----------------|
| 1 | **SFML** (Simple and Fast Multimedia Library), Version 3.x, Komponenten: Graphics, Window, System | Fenster, Rendering, Text, Tastatur- und Mausereignisse | [https://www.sfml-dev.org/](https://www.sfml-dev.org/) — Lizenz: zlib/png |
| 2 | **DejaVu Sans Bold** (TrueType-Schriftdatei) | Anzeige von Zahlen, Beschriftungen und UI-Text im Spiel | Datei: `assets/fonts/DejaVuSans-Bold.ttf` — [DejaVu Fonts](https://dejavu-fonts.github.io/) (frei nutzbar, siehe Projekt-Lizenzhinweise) |
| 3 | **CMake** (Build-System) | Konfiguration des Builds, Auffinden von SFML, Erzeugen der ausführbaren Datei | [https://cmake.org/](https://cmake.org/) — BSD-Lizenz (Werkzeug, keine eingebundene Laufzeitbibliothek) |
| 4 | **vcpkg** (optional) | Alternative Bereitstellung von Abhängigkeiten über `vcpkg.json` | [https://vcpkg.io/](https://vcpkg.io/) — MIT (Werkzeug) |

*Hinweis:* Compiler, Standardbibliothek und Betriebssystem zählen nicht als „Drittleistungen“ im Sinne von Drittanbieter-Assets; sie sind Voraussetzungen zur Ausführung.

---

## Abgleich mit den Aufgabenanforderungen

- **C++:** Umsetzung in C++17  
- **Desktopanwendung oder Spiel:** 2D-Desktop-Spiel (2048)  
- **Mindestens eine Bibliothek:** SFML über CMake eingebunden  
- **Algorithmus/Mechanik:** Merge- und Bewegungslogik, Zufallsspawn, Erkennung „kein Zug möglich“  

---

## Projektstruktur (Auszug)

- `src/main.cpp` — Einstieg, Fenster, Hauptschleife  
- `src/game2048.*` — Spiellogik (Board, Score, Züge)  
- `src/game_view.*` — Zeichnen und Eingaben  
- `CMakeLists.txt` — Build-Konfiguration  
- `assets/fonts/DejaVuSans-Bold.ttf` — Schriftart  
- `cmake/Package.cmake` — optional: Abgabepaket `dist/`  
- `vcpkg.json` — optional: Abhängigkeits-Manifest  

---

## Voraussetzungen

- CMake 3.16+  
- C++17-kompatibler Compiler  
- SFML 3.0+ (Graphics, Window, System)  

### SFML installieren

**macOS (Homebrew)**

```bash
brew install sfml
```

**Ubuntu/Debian**

```bash
sudo apt update
sudo apt install libsfml-dev
```

**Windows:** SFML von der [offiziellen SFML-Webseite](https://www.sfml-dev.org/download.php) laden und `SFML_DIR` bzw. `CMAKE_PREFIX_PATH` für CMake setzen.

---

## Build und Start

```bash
cd game_2048
cmake -S . -B build
cmake --build build
./build/game_2048
```

### Optional: Build mit vcpkg

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=/pfad/zu/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=x64-osx
cmake --build build
```

### Abgabepaket (`dist`) erzeugen

```bash
cd game_2048
cmake -S . -B build
cmake --build build --target package
```

Ausgabe u. a.: `build/dist/game_2048` (unter Windows: `game_2048.exe`) und `build/dist/assets/fonts/DejaVuSans-Bold.ttf`.

---

## Steuerung

- **Pfeiltasten:** Kacheln bewegen  
- **R:** Neustart  
- **New Game** (Mausklick): Neustart  

---

## Spielregeln (Kurz)

- Gleiche Werte verschmelzen (`2 + 2 → 4`).  
- Nach gültigem Zug: neue Kachel 2 oder 4.  
- **Game Over:** kein legaler Zug mehr.  
- **Ziel:** Kachel **2048** erreichen.  

---

## Hinweise zu Performance und Portabilität

- Bildrate begrenzt auf 60 FPS.  
- Beim Build wird `assets/` neben die ausführbare Datei kopiert.  
- Spielfeld: feste Arrays 4×4.  
