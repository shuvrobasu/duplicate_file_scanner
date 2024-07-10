#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define MAX_FILES 1000
#define MAX_PATH 1024

void print_banner() {
    printf("\n\n") ;
    printf("\033[1;37m ########::'##::::'##:'########::::'########::'########:'##::::'##::'#######::'##::::'##:'########:'########:'########::\n");
    printf("\033[1;37m ##.... ##: ##:::: ##: ##.... ##::: ##.... ##: ##.....:: ###::'###:'##.... ##: ##:::: ##: ##.....:: ##.....:: ##.... ##:\n");
    printf("\033[1;37m ##:::: ##: ##:::: ##: ##:::: ##::: ##:::: ##: ##::::::: ####'####: ##:::: ##: ##:::: ##: ##::::::: ##::::::: ##:::: ##:\n");
    printf("\033[1;37m ##:::: ##: ##:::: ##: ########:::: ########:: ######::: ## ### ##: ##:::: ##: ##:::: ##: ######::: ######::: ########::\n");
    printf("\033[1;37m ##:::: ##: ##:::: ##: ##.....::::: ##.. ##::: ##...:::: ##. #: ##: ##:::: ##:. ##:: ##:: ##...:::: ##...:::: ##.. ##::\n");
    printf("\033[1;37m ##:::: ##: ##:::: ##: ##:::::::::: ##::. ##:: ##::::::: ##:.:: ##: ##:::: ##::. ## ##::: ##::::::: ##::::::: ##::. ##::\n");
    printf("\033[1;37m ########::. #######:: ##:::::::::: ##:::. ##: ########: ##:::: ##:. #######::::. ###:::: ########: ########: ##:::. ##:\n");
    printf("........::::.......:::..:::::::::::..:::::..::........::..:::::..:::.......::::::...:::::........::........::..:::::..::\n\n");
    printf("\033[0m");
    printf("\033[1;33mWelcome to DupRemover!\n\n");
    printf("This program will help you find duplicate files in a directory.\n");
    printf("You can choose to find duplicates by [N]ame or [S]ize.\n\n");
    printf("Usage: ./dup <directory> <N|S>\033[0m\n\n\033[131mCtrl+C to exit.\n\n");
    printf("Please note that this program will prompt you to delete duplicate files.\n");
    printf("\033[0m");

    printf("\033[1;32m(c) Shuvro Basu, 2024.\n\n");
    printf("\033[0m");
    printf("\n");
}

typedef struct {
    char path[MAX_PATH];
    char name[MAX_PATH];
    size_t size;
    time_t mtime;
} FileEntry;

void find_files(const char *base_path, FileEntry *files, int *count) {
    DIR *dir;
    struct dirent *entry;
    char path[MAX_PATH];

    if (!(dir = opendir(base_path))) {
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);
        struct stat st;
        if (stat(path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                find_files(path, files, count);
            } else {
                strncpy(files[*count].path, path, MAX_PATH);
                strncpy(files[*count].name, entry->d_name, MAX_PATH);
                files[*count].size = st.st_size;
                files[*count].mtime = st.st_mtime;
                (*count)++;
            }
        }
    }
    closedir(dir);
}

void clear_screen() {
    printf("\033[H\033[2J");
    fflush(stdout);
}

void print_file_info(const FileEntry *file) {
    char timebuf[80];
    struct tm *tm_info;
    tm_info = localtime(&file->mtime);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Name: %s (Path: %s)\nSize: %lu bytes\nDate: %s\n\n", file->name, file->path, file->size, timebuf);
}

void prompt_and_delete(const char *file1, const char *file2) {
    char choice;
    printf("\033[1;31mDo you want to delete:\n1. %s\n2. %s\nn. None\nChoice: ", file1, file2);
    printf("\033[0m");
    scanf(" %c", &choice);
    if (choice == '1') {
        if (remove(file1) == 0) {
            printf("Deleted %s\n", file1);
            clear_screen();
        } else {
            perror("\033[1;31mError deleting file");
            printf("\033[0m");
        }
    } else if (choice == '2') {
        if (remove(file2) == 0) {
            printf("Deleted %s\n", file2);
            clear_screen();
        } else {
            perror("\033[1;31mError deleting file");
            printf("\033[0m");
        }
    } else {
        printf("\033[1;31mNo files deleted\n");
        printf("\033[0m");

    }
}

void find_duplicates_by_name(FileEntry *files, int count) {
    int duplicates_found = 0;
    int progress = 0;
    int total = count * (count - 1) / 2; // Total number of comparisons

    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            progress++;
            if (strcmp(files[i].name, files[j].name) == 0 && strcmp(files[i].path, files[j].path) != 0) {
                duplicates_found = 1;
                printf("Duplicate by name:\n");
                printf("File 1:\n");
                print_file_info(&files[i]);
                printf("File 2:\n");
                print_file_info(&files[j]);
                prompt_and_delete(files[i].path, files[j].path);
            }
            // Display progress bar
            printf("Scanning folder : [%d/%d] %d%%\r", progress, total, (int)((double)progress / total * 100));
            fflush(stdout); // Flush output buffer to ensure the progress bar is updated immediately
        }
    }

    if (!duplicates_found) {
        printf("\033[1;31m\nNo duplicates found.\n");
        printf("\033[0m");
    }
    printf("\n");
}

void find_duplicates_by_size(FileEntry *files, int count) {
    int duplicates_found = 0;
    int progress = 0;
    int total = count * (count - 1) / 2; // Total number of comparisons

    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            progress++;
            if (files[i].size == files[j].size && strcmp(files[i].name, files[j].name) == 0 && strcmp(files[i].path, files[j].path) != 0) {
                duplicates_found = 1;
                printf("Duplicate by size:\n");
                printf("File 1:\n");
                print_file_info(&files[i]);
                printf("File 2:\n");
                print_file_info(&files[j]);
                prompt_and_delete(files[i].path, files[j].path);
            }
            // Display Scanning folder :Scanning folder : bar
            printf("Scanning Folder : [%d/%d] %d%%\r", progress, total, (int)((double)progress / total * 100));
            fflush(stdout); // Flush output buffer to ensure the progress bar is updated immediately
        }
    }

    if (!duplicates_found) {
        printf("\n\033[1;31mNo duplicates found.\n");
        printf("\033[0m");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
        if (argc < 3) {
        fprintf(stderr, "Usage: %s <directory> <N|S>\n", argv[0]);
        return 1;
    }

    char *directory = argv[1];
    char option = argv[2][0];

    if (option != 'N' && option != 'S' && option != 'n' && option != 's') {
        fprintf(stderr, "\033[1;31mInvalid option. Use 'N' for Name or 'S' for Size.\n");
        printf("\033[0m");
        return 1;
    }

    clear_screen();
    print_banner();

    FileEntry files[MAX_FILES];
    int count = 0;
    find_files(directory, files, &count);

    if (count == 0) {
        printf("\033[1;31mNo files found in the directory.\n");
        printf("\033[0m");
        return 0;
    }

    switch (option) {
        case 'N' :
        case 'n' :
            printf("\033[1;34mSearching by Name\n");
            printf("\033[0m");
            find_duplicates_by_name(files, count);
            printf("\n");
            
            break;
        case 'S' : 
        case 's':
            printf("\033[1;34mSearching by Size\n");
            printf("\033[0m");
            find_duplicates_by_size(files, count);
            printf("\n");
            break;
    }

    return 0;
}
