# MP3 Tag Reader 🎵

The **MP3 Tag Reader** is a command-line project in **C** designed to work with **ID3v2.3 tags** in MP3 files.  
ID3 tags are metadata embedded in audio files that store information such as **Title, Track, Artist, Album, Year, Genre, and Comments**.  
This tool allows you to **read and update these tags**, demonstrating **binary file handling** and **modular programming** in C.

---

## ℹ️ About ID3v2.3 Tags
- **ID3 (Identify an MP3)** is a metadata container for MP3 files.  
- **ID3v2.3** is one of the most widely used versions, storing data in “frames” (e.g., `TIT2` for Title, `TPE1` for Artist).  
- Each frame holds specific information about the song and is located at the **beginning of the MP3 file**.  
- This project is **limited to ID3v2.3** and does **not support** other versions like **ID3v1** or **ID3v2.4**.  

---

## >_  Command-Line Usage

```bash
mp3tagreader [options] <filename>
```

### Options

| Option | Description |
|--------|-------------|
| `--help` | Show help message |
| `-v <filename>` | View all tags from the given MP3 file |
| `-e <arg> <value> <filename>` | Edit/update a tag with a new value |

### Supported Arguments for `-e`

| Argument | Tag | Frame |
|----------|-----|-------|
| `-t` | Title | `TIT2` |
| `-T` | Track | `TRCK` |
| `-a` | Artist | `TPE1` |
| `-A` | Album | `TALB` |
| `-y` | Year | `TYER` |
| `-C` | Comment | `COMM` |
| `-g` | Genre | `TCON` |

---

## Example Commands

### 1. View all tags
```bash
./mp3tagreader -v sample.mp3
```

### 2. Edit Title
```bash
./mp3tagreader -e -t "Perfect" sample.mp3
```

### 3. Edit Artist
```bash
./mp3tagreader -e -a "Ed Sheeran" sample.mp3
```

### 4. Help Menu
```bash
./mp3tagreader --help
```

---

## 📖 Note on Tags
The program only supports **ID3v2.3 tags**.  
These tags are stored in **frames at the start of an MP3 file**.  

### Common Frames:
- `TIT2` → Title  
- `TRCK` → Track number  
- `TPE1` → Artist  
- `TALB` → Album  
- `TYER` → Year  
- `COMM` → Comment  
- `TCON` → Genre  

---

## 🛠️ Compilation
To compile the project, run:
```bash
make
```

This will generate the **`mp3tagreader`** executable.

---

## 👨‍💻 Author
- GitHub: [@LABHESHP2003](https://github.com/LABHESHP2003)  
- Developed as a learning project for **C programming, binary file handling, and metadata manipulation**.
