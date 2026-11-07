# Custom C `ls` Utility (Windows)

A colorized, feature-rich directory listing utility implemented in C for Windows console environments, mimicking the Unix `ls` command with modern terminal enhancements.

---

## Features

* **Terminal-Aware Column Formatting:** Automatically detects console window width via the Windows API (`GetConsoleScreenBufferInfo`) to wrap entries neatly into multiple columns.
* **Syntax & File-Type Color Highlighting:**
  * **Directories:** Blue
  * **Executables:** Green
  * **Shortcuts (`.lnk`):** Cyan
  * **Archives (`.zip`, `.tar.gz`, `.iso`, etc.):** Red
  * **Media (`.png`, `.jpg`, `.mp4`, `.mp3`, etc.):** Magenta
  * **Source Code & Configs (`.c`, `.py`, `.js`, `.json`, etc.):** Yellow
* **Long Listing Format (`-l`):** Displays file size, last modified date/time, and filename.
* **Human-Readable Sizes (`-r`):** Formats raw byte counts into units (`B`, `KB`, `MB`, `GB`, `TB`).
* **Flexible Sorting Options:** Sort by filename, modification timestamp, file size, or file extension.

---

## Getting Started

### Prerequisites

* Windows environment
* A C compiler supporting Windows APIs (e.g., `gcc` via MinGW/MSYS2, Clang, or MSVC)

### Compilation

Compile using `gcc` (MinGW):

```cmd
gcc -Wall -Wextra -O2 ls.c -o myls.exe
```

---

## Usage

```cmd
myls.exe [flags] [directory]
```

If no directory is specified, it defaults to the current working directory (`.`).

### Command-Line Flags

| Flag | Description |
| :--- | :--- |
| `-a` | Include hidden files and directories (names starting with `.`) |
| `-l` | Use long listing format (Size, Date, Time, Name) |
| `-r` | Display file sizes in human-readable units (`KB`, `MB`, etc.) in long listing |
| `-t` | Sort entries by last modification time (newest first) |
| `-s` | Sort entries by file size (largest first) |
| `-e` | Sort entries alphabetically by file extension |

---

## Examples

* **Default directory list:**
  ```cmd
  myls
  ```

* **Show hidden files in long format with human-readable sizes:**
  ```cmd
  myls -a -l -r
  ```

* **Inspect a specific directory sorted by file size:**
  ```cmd
  myls -l -s C:\path\to\folder
  ```

* **Sort by extension:**
  ```cmd
  myls -e
  ```

---

## License

This project is open-source and available under the [MIT License](LICENSE).
