#ifndef _INIT_H_
#define _INIT_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

void create_file(const char* path, const char* content);
void init_repository(const char* dir);
int directory_exists(const char* dir);
int create_directory(const char* dir);
void init_repository(const char* dir);

#endif