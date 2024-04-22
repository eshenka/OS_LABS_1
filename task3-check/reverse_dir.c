#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

void reverse_str(char* to_reverse) {
    int len = strlen(to_reverse);

    char tmp[len];
    memcpy(tmp, to_reverse, len);
    
    for(int i = len - 1; i >= 0; i--) {
        to_reverse[len - 1 - i] = tmp[i]; 
    }
}

void write_reversed_file(int file, int rev_file) {
    int buffer_size = 1024;
    char buffer[buffer_size];
    
    int file_size = lseek(file, 0, SEEK_END);
    lseek(file, 0, SEEK_SET);
    
    for (int pos = 0; pos < file_size / buffer_size; pos++) {
        int r = read(file, buffer, buffer_size);
        if (r == -1) {
            perror("Unable to read the file");
            exit(-1);
        }

        reverse_str(buffer);

        int offset = file_size - (pos + 1) * buffer_size;

        int w = pwrite(rev_file, buffer, buffer_size, offset);
        if (w == -1) {
            perror("Unable to write to the file");
            exit(-1);
        }
    }

    int left = read(file, buffer, buffer_size);
    if (left > 0) {
        reverse_str(buffer);

        pwrite(rev_file, buffer, left, 0);
    }
}

void reverse_directory(char* dir_name) {
    char rev_name[256];

    int j = 0;
    for (int i = strlen(dir_name) - 1; dir_name[i] != '/'; i--) {
        rev_name[j] = dir_name[i];
        j++;
    }
        
    char parent_dir_name[256];
    int fucking_len = strlen(dir_name) - j;
    memcpy(parent_dir_name, dir_name, fucking_len);
    parent_dir_name[fucking_len] = '\0';

    char rev_dir_name[256];
    memcpy(rev_dir_name, parent_dir_name, strlen(parent_dir_name));
    strncat(rev_dir_name, rev_name, strlen(rev_name));
    rev_dir_name[strlen(parent_dir_name) + strlen(rev_name)] = '\0';

    int rev_dir_creat = mkdir(rev_dir_name, 0777);
    if (rev_dir_creat == -1) {
        perror("Unable to create directory");
        exit(-1);
    }
    
    DIR* dir = opendir(dir_name);
    int dir_fd = open(dir_name, O_DIRECTORY);
    int rev_dir_fd = open(rev_dir_name, O_DIRECTORY);

    if (dir_fd == -1 || rev_dir_fd == -1) {
        perror("Unable to open directory");
        exit(-1);
    }

    struct dirent* file_entry = readdir(dir);
    while (file_entry != NULL) {
        if (file_entry->d_type == DT_REG) {
            int file_fd = openat(dir_fd, file_entry->d_name, O_RDONLY);
            printf("opening readonly file %s\n", file_entry->d_name);

            if (file_fd == -1) {
                perror("Unable to open file");
                exit(-1);
            }

            struct stat* fl_stat;
            fstat(file_fd, fl_stat);

            char rev_file_name[strlen(file_entry->d_name) + 1];
            memcpy(rev_file_name, file_entry->d_name, strlen(file_entry->d_name));
            rev_file_name[strlen(file_entry->d_name)] = '\0';
            reverse_str(rev_file_name);

            int rev_file_fd = openat(rev_dir_fd, rev_file_name, O_WRONLY | O_CREAT, fl_stat->st_mode);
            printf("creating write only file %s\n", rev_file_name);
            if (rev_file_fd == -1) {
                perror("Unable to create file");
                exit(-1);
            }

            write_reversed_file(file_fd, rev_file_fd);

            close(file_fd);
            close(rev_file_fd);
        }
        file_entry = readdir(dir);
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Incorrect directory name\n");
        exit(-1);
    }

    reverse_directory(argv[1]);

    return 0;
}

