#include "util.h"
#include "alloc.h"

int tst_bit(char *buf, int bit) 
{
    int i = bit / 8;
    int j = bit % 8;
    
    return (buf[i] & (1 << j));
}

int set_bit(char *buf, int bit) 
{
    int i = bit / 8;
    int j = bit % 8;
    
    (buf[i] |= (1 << j));
    return bit;
}

int clr_bit(char *buf, int bit) 
{
    int i = bit / 8;
    int j = bit % 8;
    
    buf[i] &= ~(1 << j);
    return bit;
}

int decFreeInodes(int dev)
{
  char buf[BLKSIZE];
  // dec free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count--;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count--;
  put_block(dev, 2, buf);
}

int ialloc(int dev)  // allocate an inode number from inode_bitmap
{
  int  i;
  char buf[BLKSIZE];

  // MTABLE *mp = (MTABLE *)get_mtable(dev);
  
  // read inode_bitmap block
  get_block(dev, imap, buf);

  for (i=0; i < ninodes; i++){ // use ninodes from SUPER block
    if (tst_bit(buf, i)==0){
        set_bit(buf, i);
	put_block(dev, imap, buf);

	decFreeInodes(dev);

	printf("allocated ino = %d\n", i+1); // bits count from 0; ino from 1
        return i+1;
    }
  }
  return 0;
}

// WRITE YOUR OWN balloc(dev) function, which allocates a FREE disk block number
int balloc(int dev)
{
    int  i;
    char buf[BLKSIZE];

    // MTABLE *mp = (MTABLE *)get_mtable(dev);
    
    // read inode_bitmap block
    get_block(dev, bmap, buf);

    for (i=0; i < nblocks; i++)
    { // use ninodes from SUPER block
      if (tst_bit(buf, i)==0)
      {
          set_bit(buf, i);
          put_block(dev, bmap, buf);

          //decFreeInodes(dev);

          printf("allocated block = %d\n", i+1); // bits count from 0; ino from 1
          return i+1;
      }
    }
    return 0;   
}

int incFreeInodes(int dev)
{
  char buf[BLKSIZE];

  // inc free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count++;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, buf);
}
		  
int idalloc(int dev, int ino)
{
  int i;  
  char buf[BLKSIZE];

  if (ino > ninodes){
    printf("inumber %d out of range\n", ino);
    return -1;
  }

  // get inode bitmap block
  get_block(dev, imap, buf);
  clr_bit(buf, ino-1);

  // write buf back
  put_block(dev, imap, buf);

  // update free inode count in SUPER and GD
  incFreeInodes(dev);
}

int bdalloc(int dev, int ino)
{
  printf("To Be Implemented\n");
}

// Finish iput(MINODE *mip) code in util.c (in Chapter 11.7.2 3.3)