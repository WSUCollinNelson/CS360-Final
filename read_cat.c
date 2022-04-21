#include "read_cat.h"

int read_file()
{

}

int myread(int fd, char *buf, int nbytes)
{
    OFT *oftp = proc[0].fd[fd];
    int avil = oftp->minodePtr->INODE.i_size - oftp->offset; //'s offset // number of bytes still available in file.
    int lblk = oftp->offset / BLKSIZE;               // cq points at buf[ ]
    int start = oftp->offset % BLKSIZE;
    int remaining = nbytes < avil ? nbytes : avil;
    if (DEBUG) printf("avil: %d reamining: %d\n", avil, remaining);

    int bytesRead = remaining;
    int blk = -1;

    char bbuf[BLKSIZE];

    blk = get_logical_block(oftp->minodePtr, lblk);
    if (DEBUG) printf("\n -------New Block------- \n");
    get_block(dev, blk, bbuf);

    if(BLKSIZE - start >= remaining)
    {
        memcpy(buf, bbuf, remaining);
        remaining = 0;
    }
    else
    {
        memcpy(buf, bbuf + start, BLKSIZE - start);
        remaining -= (BLKSIZE - start);
        buf += (BLKSIZE - start);
    }

    //I believe in you
    while(remaining > 0)
    {
        lblk++;
        blk = get_logical_block(oftp->minodePtr, lblk);
        if (DEBUG) printf("\n -------New Block------- \n");
        get_block(dev, blk, bbuf);

        if(remaining >= BLKSIZE)
        {
            if (DEBUG) printf("in 1st case\n");
            memcpy(buf, bbuf, BLKSIZE);
            buf += BLKSIZE;
            remaining -= BLKSIZE;
        }
        else 
        {
            memcpy(buf, bbuf, remaining);
            remaining = 0;
        }
    }

    oftp->offset += bytesRead;
    return bytesRead;
    // return index;   // count is the actual number of bytes read
}