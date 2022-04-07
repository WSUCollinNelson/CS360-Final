#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "util.h"
#include "header.h"

char parent[128], child[64];

int my_link();
int my_unlink();
int my_readlink();