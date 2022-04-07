#include "minor_commands.h"

int my_stat()
{
    struct stat myst;
    int ino = getino(pathname);
    MINODE* mip = iget(dev, ino);
    printf("ino: %d mip->ino: %d\n", ino, mip->ino);

    myst.st_dev = dev;
    myst.st_ino = ino;
    myst.st_mode = mip->INODE.i_mode;
    myst.st_nlink = mip->INODE.i_links_count;
    myst.st_uid = mip->INODE.i_uid;

    printf("ino: %d mode: %d links: %d uid: %d \n", ino, myst.st_mode, myst.st_nlink, myst.st_uid);

}

int my_chmod()
{

}

int my_utime()
{

}