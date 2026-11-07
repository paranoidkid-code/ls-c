
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#define reset_color "\033[0m"
#define dir_color   "\033[1;34m"
#define exe_color   "\033[1;32m"
#define link_color  "\033[1;36m"
#define comp_color  "\033[1;31m"
#define img_color   "\033[0;35m"
#define dev_color   "\033[40;33m"
#define broken_link "\033[1;40;31m"
#define sticky_dir  "\033[44;37m"

typedef struct {
    char *name;
    struct stat info;
    int stat_ok;
} fileData;

int get_terminal_width() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns = 80;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return columns;
}

int ends_with(const char *filename, const char *ext) {
    size_t file_len = strlen(filename);
    size_t ext_len = strlen(ext);
    if (ext_len > file_len) {
        return 0;
    }
    const char *file_end = filename + (file_len - ext_len);
    return strcmp(file_end, ext) == 0;
}

const char* get_color(fileData *f) {
    if (!f->stat_ok) return broken_link;
    if (S_ISDIR(f->info.st_mode)) return dir_color;
    if (f->info.st_mode & S_IXUSR) return exe_color;
    if (ends_with(f->name, ".lnk")) return link_color;
    
    if (ends_with(f->name, ".zip") || ends_with(f->name, ".rar") || ends_with(f->name, ".7z") ||
        ends_with(f->name, ".tar") || ends_with(f->name, ".gz") || ends_with(f->name, ".tar.gz") ||
        ends_with(f->name, ".tgz") || ends_with(f->name, ".bz2") || ends_with(f->name, ".xz") || 
        ends_with(f->name, ".iso")) return comp_color;
        
    if (ends_with(f->name, ".png") || ends_with(f->name, ".jpg") || ends_with(f->name, ".jpeg") || 
        ends_with(f->name, ".gif") || ends_with(f->name, ".bmp") || ends_with(f->name, ".svg") || 
        ends_with(f->name, ".webp") || ends_with(f->name, ".ico") || ends_with(f->name, ".mp4") || 
        ends_with(f->name, ".mkv") || ends_with(f->name, ".avi") || ends_with(f->name, ".mov") || 
        ends_with(f->name, ".webm") || ends_with(f->name, ".mp3") || ends_with(f->name, ".wav") || 
        ends_with(f->name, ".flac") || ends_with(f->name, ".ogg")) return img_color;
        
    if (ends_with(f->name, ".c") || ends_with(f->name, ".h") || ends_with(f->name, ".cpp") || 
        ends_with(f->name, ".hpp") || ends_with(f->name, ".html") || ends_with(f->name, ".css") || 
        ends_with(f->name, ".js") || ends_with(f->name, ".ts") || ends_with(f->name, ".json") || 
        ends_with(f->name, ".py") || ends_with(f->name, ".sh") || ends_with(f->name, ".ps1") || 
        ends_with(f->name, ".bat") || ends_with(f->name, ".ini") || ends_with(f->name, ".yaml") || 
        ends_with(f->name, ".yml") || ends_with(f->name, ".xml") || ends_with(f->name, ".toml")) return dev_color;

    return reset_color;
}

const char* get_extension(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return "";
    }
    return dot;
}

int compare_name(const void *a, const void *b) {
    fileData *fa = *(fileData **)a;
    fileData *fb = *(fileData **)b;
    return strcmp(fa->name, fb->name);
}

int compare_time(const void *a, const void *b) {
    fileData *fa = *(fileData **)a;
    fileData *fb = *(fileData **)b;
    if (fb->info.st_mtime > fa->info.st_mtime) return 1;
    if (fb->info.st_mtime < fa->info.st_mtime) return -1;
    return strcmp(fa->name, fb->name);
}

int compare_size(const void *a, const void *b) {
    fileData *fa = *(fileData **)a;
    fileData *fb = *(fileData **)b;
    if (fb->info.st_size > fa->info.st_size) return 1;
    if (fb->info.st_size < fa->info.st_size) return -1;
    return strcmp(fa->name, fb->name);
}

int compare_ext(const void *a, const void *b) {
    fileData *fa = *(fileData **)a;
    fileData *fb = *(fileData **)b;
    const char *ext_a = get_extension(fa->name);
    const char *ext_b = get_extension(fb->name);
    int ext_cmp = strcmp(ext_a, ext_b);
    if (ext_cmp != 0) {
        return ext_cmp;
    }
    return strcmp(fa->name, fb->name);
}

void format_size(long bytes, char *buffer) {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double size = bytes;
    while (size >= 1024 && i < 4) {
        size /= 1024;
        i++;
    }
    if (i == 0) {
        sprintf(buffer, "%ld%s", (long)size, units[i]);
    } else {
        sprintf(buffer, "%.1f%s", size, units[i]);
    }
}

int main(int argc, char *argv[]) {
    const char *target_path = ".";
    int show_hidden = 0, show_long = 0, sort_time = 0, sort_size = 0, sort_ext, size_readable = 0;
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) show_hidden = 1;
        else if (strcmp(argv[i], "-l") == 0) show_long = 1;
        else if (strcmp(argv[i], "-t") == 0) sort_time = 1;
        else if (strcmp(argv[i], "-s") == 0) sort_size = 1;
        else if (strcmp(argv[i], "-e") == 0) sort_ext = 1;
        else if (strcmp(argv[i], "-r") == 0) size_readable = 1;
        else if (argv[i][0] == '-') {
            printf("Error: Unknown flag '%s'\n", argv[i]);
            return 1;
        } else {
            target_path = argv[i];
        }
    }
    DIR *dir = opendir(target_path);
    if (dir == NULL) {
        printf("Error: Cannot open directory '%s'\n", target_path);
        return 1;
    }
    struct dirent *entry;
    int capacity = 256;
    fileData **files = malloc(capacity * sizeof(fileData *));
    int file_count = 0, max_len = 0;
    char full_path[1024];
    while ((entry = readdir(dir)) != NULL) {
        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }
        if (file_count >= capacity) {
            capacity *= 2;
            files = realloc(files, capacity * sizeof(fileData *));
            if (files == NULL) {
                printf("Error: Out of memory\n");
                return 1;
            }
        }
        files[file_count] = malloc(sizeof(fileData));
        files[file_count]->name = malloc(strlen(entry->d_name) + 1);
        strcpy(files[file_count]->name, entry->d_name);
        snprintf(full_path, sizeof(full_path), "%s/%s", target_path, entry->d_name);
        if (stat(full_path, &files[file_count]->info) != -1) {
            files[file_count]->stat_ok = 1;
        } else {
            files[file_count]->stat_ok = 0;
        }
        int current_len = strlen(entry->d_name);
        if (current_len > max_len) {
            max_len = current_len;
        }
        file_count++;
    }
    closedir(dir);
    if (sort_size) {
        qsort(files, file_count, sizeof(fileData *), compare_size);
    } else if (sort_time) {
        qsort(files, file_count, sizeof(fileData *), compare_time);
    } else if (sort_ext) {
        qsort(files, file_count, sizeof(fileData *), compare_ext);
    } else {
        qsort(files, file_count, sizeof(fileData *), compare_name);
    }
    int term_width = get_terminal_width();
    int col_width = max_len + 2;
    int cols_per_row = term_width / col_width;
    if (cols_per_row == 0) cols_per_row = 1;

    if (show_long) {
        printf("\033[4m%8s %12s %s\033[0m\n", "Size", "Date  Time ", "Name    ");
    }

    for (int i = 0; i < file_count; i++) {
        fileData *f = files[i];
        const char *color = get_color(f);
        if (show_long) {
            char time_str[64] = "Unknown Time";
            char size_str[32] = "0B";
            if (f->stat_ok) {
                struct tm *tm_info = localtime(&f->info.st_mtime);
                strftime(time_str, sizeof(time_str), "%b %d %H:%M", tm_info);
                if (size_readable) {
                    format_size((long)f->info.st_size, size_str);
                } else {
                    sprintf(size_str, "%ld", (long)f->info.st_size);
                }
            }
            printf("%8s %12s %s%s%s\n", size_str, time_str, color, f->name, reset_color);
        } else {
            printf("%s%s%s", color, f->name, reset_color);
            int spaces_needed = (max_len - strlen(f->name)) + 2;
            for (int s = 0; s < spaces_needed; s++) printf(" ");
            if ((i + 1) % cols_per_row == 0) printf("\n");
        }
        free(f->name);
        free(f);
    }
    if (!show_long && file_count % cols_per_row != 0) {
        printf("\n");
    }
    free(files);
    return 0;
}