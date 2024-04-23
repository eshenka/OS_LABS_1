#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

void make_directory(char* dir_name) {
    int dir = mkdir(dir_name, 0777);
    if (dir == -1) {
        printf("unable to create a directiry with this name\n");
        exit(0);
    }
}

void list_directory(char* dir_name) {
    DIR* dir = opendir(dir_name);

    if (dir == NULL) {
        printf("Unable to open directory\n");
        exit(0);
    }

    struct dirent* dir_elem = readdir(dir);
    while(dir_elem != NULL) {
        printf("%s\n", dir_elem->d_name);
        dir_elem = readdir(dir);
    }

    closedir(dir);
}

void remove_directory(char* dir_name) {
    int removed_dir = rmdir(dir_name);
    if (removed_dir == -1) {
        perror("Unable to remove");
        exit(0);
    }
}

void make_file(char* file_name) {
    int file = creat(file_name, O_CREAT);
    if (file == -1) {
        perror("Unable to create file");
        exit(0);
    }
}

void print_file(char* file_name) {
    FILE* file = fopen(file_name, "r");

    if (file == NULL) {
        perror("Unable to open file");
        exit(0);
    }

    size_t buffer_size = 100;
    char buffer[buffer_size];

    while(!feof(file)) {
        size_t n = fread(buffer, buffer_size - 1, 1, file);
        buffer[n - 1] = '\0';
        printf("%s\n", buffer);
    }
}

void remove_file(char* file_name) {
    if (remove(file_name) == -1) {
        perror("Unable to delete");
        exit(0);
    }
}

void create_sym_link(char* source_file, char* link_file) {
    int sym_link = symlink(source_file, link_file);
    if (sym_link == -1) {
        perror("Unable to create symlink");
        exit(0);
    }
}

void read_symlink(char* filename) {
    size_t buffer_size = 1024;
    char buffer[buffer_size];

    int link = readlink(filename, buffer, buffer_size);

    if (link == -1) {
        perror("Unable to read symlink");
        exit(0);
    }

    buffer[link] = '\0';
    printf("%s\n", buffer);
}

void print_symlink_content(char* linkname) {
    size_t buffer_size = 1024;
    char buffer[buffer_size];

    int link = readlink(linkname, buffer, buffer_size);

    if (link == -1) {
        perror("Unable to read symlink");
    }

    buffer[link] = '\0';

    print_file(buffer);
}

void remove_symlink(char* linkname) {
    if (remove(linkname) == -1) {
        perror("Unable to remove symlink");
    }
}

void create_link(char* source_file, char* link_file) {
    int hard_link = link(source_file, link_file);
    if (hard_link == -1) {
        perror("Unable to create link\n");
        exit(0);
    }
}

void convert_to_str(mode_t mode, char* mode_str) {
    for (int i = 8; i >= 0; i--) {
        if ((mode >> i) % 2 == 1) {
            if (i % 3 == 0) {
                mode_str[8 - i] = 'x';
            }
            else if (i % 3 == 1) {
                mode_str[8 - i] = 'w';
            } else {
                mode_str[8 - i] = 'r';
            }
        } else {
            mode_str[8 - i] = '-';
        }
    }
}

void file_stat(char* filename) {
    struct stat info[1];

    if (stat(filename, info) == -1) {
        perror("Unable to stat file");
        exit(0);
    }

    mode_t mode = info->st_mode;
    char mode_str[9];
    convert_to_str(info->st_mode, mode_str);
    nlink_t links = info->st_nlink;

    printf("mode: %s links: %ld\n", mode_str, links);
}

int hex_mode(int mode) {
    int hex_mode = 0;   
    int i = 0;
    while (mode) {
        hex_mode += (mode % 10) * pow(8, i);
        mode /= 10;
        i++;
    }
    return hex_mode;
}

void change_mode(char* name, int mode) {
    mode = hex_mode(mode);
    if (chmod(name, mode) == -1) {
        perror("Unable to change mode");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        perror("Invalid arguments\n");
        exit(0);
    }

    if (!strcmp(argv[0], "./make_directory")) {
        printf("im making the directory...\n");
        make_directory(argv[1]);
    }
    else if (!strcmp(argv[0], "./list_directory")) {
        printf("Reading the directory...\n");
        list_directory(argv[1]);
    }
    else if (!strcmp(argv[0], "./remove_directory")) {
        printf("Deleting directory...\n");
        remove_directory(argv[1]);
    }
    else if (!strcmp(argv[0], "./make_file")) {
        printf("Making file...\n");
        make_file(argv[1]);
    }
    else if (!strcmp(argv[0], "./print_file")) {
        printf("Printing file...\n");
        print_file(argv[1]);
    }
    else if (!strcmp(argv[0], "./remove_file")) {
        printf("Deleting file...\n");
        remove_file(argv[1]);
    }
    else if (!strcmp(argv[0], "./create_sym_link")) {
        printf("Creating symlink...\n");
        create_sym_link(argv[1], argv[2]);
    }
    else if (!strcmp(argv[0], "./read_symlink")) {
        printf("Reading symlink...\n");
        read_symlink(argv[1]);
    }
    else if (!strcmp(argv[0], "./print_symlink")) {
        printf("Printing symlink content...\n");
        print_symlink_content(argv[1]);
    }
    else if (!strcmp(argv[0], "./remove_symlink")) {
        printf("Deleting symlink...\n");
        remove_file(argv[1]);
    }
    else if (!strcmp(argv[0], "./create_link")) {
        printf("Creating link...\n");
        create_link(argv[1], argv[2]);
    }
    else if (!strcmp(argv[0], "./remove_link")) {
        printf("Removing link...");
        remove_file(argv[1]);
    }
    else if (!strcmp(argv[0], "./file_stat")) {
        file_stat(argv[1]);
    }
    else if (!strcmp(argv[0], "./change_mode")) {
        change_mode(argv[1], atoi(argv[2]));
    }
    else {
        printf("i dont know how to do this yet\n");
    }
    return 0;
}
