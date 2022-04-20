#include "read_cat.h"

int myread(int fd, char *buf, int nbytes)
{
    int count = 0;
    int avil = fileSize - OFT; //'s offset // number of bytes still available in file.
    char *cq = buf;                // cq points at buf[ ]

    while (nbytes && avil){

       //Compute LOGICAL BLOCK number lbk and startByte in that block from offset;

        lbk = oftp->offset / BLKSIZE;
        startByte = oftp->offset % BLKSIZE;
     
       // I only show how to read DIRECT BLOCKS. YOU do INDIRECT and D_INDIRECT
 
       if (lbk < 12){                     // lbk is a direct block
           blk = mip->INODE.i_block[lbk]; // map LOGICAL lbk to PHYSICAL blk
       }
       else if (lbk >= 12 && lbk < 256 + 12) { 
            //  indirect blocks 
       }
       else{ 
            //  double indirect blocks
       } 

       /* get the data block into readbuf[BLKSIZE] */
       get_block(mip->dev, blk, readbuf);

       /* copy from startByte to buf[ ], at most remain bytes in this block */
       char *cp = readbuf + startByte;   
       int remain = BLKSIZE - startByte;   // number of bytes remain in readbuf[]

       while (remain > 0){
            *cq++ = *cp++;             // copy byte from readbuf[] into buf[]
             oftp->offset++;           // advance offset 
             count++;                  // inc count as number of bytes read
             avil--; nbytes--;  remain--;
             if (nbytes <= 0 || avil <= 0) 
                 break;
       }
 
       // if one data block is not enough, loop back to OUTER while for more ...

   }
   printf("myread: read %d char from file descriptor %d\n", count, fd);  
   return count;   // count is the actual number of bytes read
}