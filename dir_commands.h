#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>

char parent[128], child[64];

int pino;
MINODE* pmip;

int my_mkdir();
int kmkdir(MINODE* pmip, char* basename);
int imkdir();
int ikmkdir(MINODE* pmip, char* basename);