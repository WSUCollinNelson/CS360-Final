#include "read_cat.h"

int read_file()
{

}

int myread(int fd, char *buf, int nbytes)
{
    OFT *oftp = proc[0].fd[fd];
    int avil = oftp->minodePtr->INODE.i_size - oftp->offset; //'s offset // number of bytes still available in file.
    char *cq = buf;                // cq points at buf[ ]

    int index = 0;
    int blk = -1;

    char bbuf[BLKSIZE];

    //I believe in you
    while(blk != 0 && avil != 0)
    {
        if (DEBUG)
            printf("Before getting lblk\n");
        int blk = get_logical_block(oftp->minodePtr, index);
        if (DEBUG)
            printf("Before getting blk: avail- %d\n", avil);
        get_block(dev, blk, bbuf);

        if(avil >= BLKSIZE)
        {
            if (DEBUG)
                printf("in 1st case\n");
            memcpy(buf, bbuf, BLKSIZE);
            buf += BLKSIZE;
            avil -= BLKSIZE;
            index++;
        }
        else 
        {
            memcpy(buf, bbuf, avil);
            avil = 0;

            if (DEBUG)
                printf("in second case: bbuf - %s blk- %d avil- %d buf- %s\n", bbuf, blk, avil, buf);
        }
    }
    //
   printf("myread: read %d char from file descriptor %d\n", index, fd);  
   return index;   // count is the actual number of bytes read
}