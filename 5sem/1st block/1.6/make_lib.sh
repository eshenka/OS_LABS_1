#!/bin/bash

gcc -c mythread.c 
ar r libmythread.a mythread.o
gcc test_join.c -g -L. -lmythread -o join
gcc test_detach.c -g -L. -lmythread -o detach
gcc test_cancel.c -g -L. -lmythread -o cancel
