#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <dirent.h>
#include <fstream>


std::string reverse_str(std::string str) {
    std::string reversed;
    for(int i = str.size() - 1; i >= 0; i--) {
        reversed += str[i];
       // std::cout<<reversed<<std::endl;

    }
    return reversed;
} 

void backword(std::string folder) {
    std::string res_folder = folder;
    std::string back_folder;
    for (int i = folder.size() - 1; folder[i] != '/'; i--) {
        back_folder += folder[i];
        folder.pop_back();
    }
    back_folder = folder + back_folder;
    const char* char_folder = back_folder.c_str();

    mkdir(back_folder.c_str(), 0777);

    DIR* directory = opendir(res_folder.c_str());
    DIR* back_dir = opendir(back_folder.c_str());

    if (directory != NULL) {
        struct dirent* d;
        struct stat dst;
        for (d = readdir(directory); d != NULL; d = readdir(directory)) {
            std::string short_name = d->d_name;
            std::string name = res_folder + "/" + short_name;
            if (stat(name.c_str(), &dst) == 0) {
                if (dst.st_mode & S_IFREG) {
                    std::cout << "i've found a reg file" << std::endl;

                    std::string back_file_name = reverse_str(short_name);
                    //std::cout << "1" << back_file_name << std::endl;
                    back_file_name = back_folder + "/" + back_file_name;
                    //std::cout << "2" << back_file_name << std::endl;
                    std::ofstream back_file(back_file_name);
                    std::ifstream file(name);
                    //std::cout<<"i was able to open successfuly"<<std::endl;

                    std::string content;
                    while(!file.eof()) {
                        char tmp = file.get();
                        if (!file.eof()) {
                            content.insert(content.begin(), tmp);
                        }
                    }
                    back_file << content;

                    back_file.close();
                    file.close();
                }
            }
        }
    }

    std::cout << back_folder << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Invalid input" << std::endl;
        exit(0);
    }

    backword(argv[1]);

    return 0;
}
