#include "open_close.h"

int my_open_file(char *filePath, int mode)
{
    int ino = getino(filePath);
    MINODE *mip = iget(dev, ino);

    if ((mip->INODE.i_mode & 0xF000) != 0x8000){
        printf("Cannot open: not a file.\n");
        return -1;
    }

    for (int i = 0; i < NFD; i++){
        if (running->fd[i] != 0 && running->fd[i]->minodePtr == mip && running->fd[i]->mode != 0){
            printf("File is currently open in an incompatible mode.\n");
            return -1;
        }
    }

    //Find unused oft & initialize
    OFT *freeOft = oftTable;
    while(freeOft->refCount != 0){
        freeOft++;
    }

    freeOft->mode = mode;
    freeOft->refCount = 1;
    freeOft->minodePtr = mip;

    switch(mode){
        case 0:
            freeOft->offset = 0;
            break;
        case 1:
            my_truncate(mip);
            freeOft->offset = 0;
            break;
        case 2:
            freeOft->offset = 0;
            break;
        case 3:
            freeOft->offset = mip->INODE.i_size;
            break;
        default:
            printf("Invalid mode.\n");
            return -1;
        ;
    }

    int freeFd = 0;
    while(running->fd[freeFd] != 0) {
        freeFd++;
    }

    running->fd[freeFd] = freeOft;

    if (mode == 0)
    {
        mip->INODE.i_atime = time(0L);
    }
    else 
    {
        mip->INODE.i_atime = mip->INODE.i_mtime = time(0L);
    }

    return freeFd;
}

int my_truncate(MINODE *mip)
{
    //Stub
}