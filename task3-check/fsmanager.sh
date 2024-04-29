#!/bin/bash

gcc fsmanager.c -o manager -lm
ln manager make_directory
ln manager list_directory
ln manager remove_directory
ln manager make_file
ln manager print_file
ln manager remove_file
ln manager create_symlink
ln manager read_symlink
ln manager print_symlink_content
ln manager remove_symlink
ln manager create_link
ln manager file_stat
ln manager change_mode
