/*********** util.c file ****************/

#include "type.h"
#include "util.h"

#define DEBUG 1

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;

extern char gpath[128];
extern char *name[64];
extern int n;

extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, iblk;

extern char line[128], cmd[32], pathname[128];

int get_block(int dev, int blk, char *buf)
{
   lseek(dev, (long)blk*BLKSIZE, 0);
   read(dev, buf, BLKSIZE);
}   

int put_block(int dev, int blk, char *buf)
{
   lseek(dev, (long)blk*BLKSIZE, 0);
   write(dev, buf, BLKSIZE);
}   

int tokenize(char *pathname)
{
  int i;
  char *s;
  if(DEBUG){printf("tokenize %s\n", pathname);}

  strcpy(gpath, pathname);   // tokens are in global gpath[ ]
  n = 0;

  s = strtok(gpath, "/");
  while(s){
    name[n] = s;
    n++;
    s = strtok(0, "/");
  }
  name[n] = 0;
  
  if(DEBUG)
  {
     for (i= 0; i<n; i++)
      printf("%s  ", name[i]);
     printf("\n");
  }
}

// return minode pointer to loaded INODE
MINODE *iget(int dev, int ino)
{
  int i;
  MINODE *mip;
  char buf[BLKSIZE];
  int blk, offset;
  INODE *ip;

  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount && mip->dev == dev && mip->ino == ino){
       mip->refCount++;
       if(DEBUG) printf("found [%d %d] as minode[%d] in core\n", dev, ino, i);
       return mip;
    }
  }
    
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount == 0){
       //printf("allocating NEW minode[%d] for [%d %d]\n", i, dev, ino);
       mip->refCount = 1;
       mip->dev = dev;
       mip->ino = ino;

       // get INODE of ino to buf    
       blk    = (ino-1)/8 + iblk;
       offset = (ino-1) % 8;

       //printf("iget: ino=%d blk=%d offset=%d\n", ino, blk, offset);

       get_block(dev, blk, buf);
       ip = (INODE *)buf + offset;
       // copy INODE to mp->INODE
       mip->INODE = *ip;
       return mip;
    }
  }   
  printf("PANIC: no more free minodes\n");
  return 0;
}

void iput(MINODE *mip)
{
   int i, block, offset;
   char buf[BLKSIZE];
   INODE *ip;

   if (mip==0) 
      return;

   mip->refCount--;
   
   if (mip->refCount > 0) return;
   if (!mip->dirty)       return;
   
   /* write INODE back to disk */
   /**************** NOTE ******************************
    For mountroot, we never MODIFY any loaded INODE
                  so no need to write it back
   FOR LATER WROK: MUST write INODE back to disk if refCount==0 && DIRTY

   Write YOUR code here to write INODE back to disk
   *****************************************************/
   block = (mip->ino-1)/8 +iblk;
   offset = (mip->ino-1)% 8;

   get_block(dev, block, buf);
   ip = (INODE *)buf + offset;
   *ip = mip->INODE;
   put_block(dev, block, buf);
} 

int search(MINODE *mip, char *name)
{
   int i; 
   char *cp, c, sbuf[BLKSIZE], temp[256];
   DIR *dp;
   INODE *ip;

   if(DEBUG) printf("search for %s in MINODE = [%d, %d]\n", name,mip->dev,mip->ino);
   ip = &(mip->INODE);

   /*** search for name in mip's data blocks: ASSUME i_block[0] ONLY ***/

   get_block(dev, ip->i_block[0], sbuf);
   dp = (DIR *)sbuf;
   cp = sbuf;
   if(DEBUG){ printf("  ino   rlen  nlen  name\n");}

   while (cp < sbuf + BLKSIZE){
     strncpy(temp, dp->name, dp->name_len);
     temp[dp->name_len] = 0;
     if(DEBUG){
      if(DEBUG) printf("%4d  %4d  %4d    %s\n", 
            dp->inode, dp->rec_len, dp->name_len, dp->name);
     }
     if (strcmp(temp, name)==0){
        if(DEBUG){ printf("found %s : ino = %d\n", temp, dp->inode);}
        return dp->inode;
     }
     cp += dp->rec_len;
     dp = (DIR *)cp;
   }
   return 0;
}

int getino(char *pathname)
{
  int i, ino, blk, offset;
  char buf[BLKSIZE];
  INODE *ip;
  MINODE *mip;

  if(DEBUG){printf("getino: pathname=%s\n", pathname);}
  if (strcmp(pathname, "/")==0)
      return 2;
  
  // starting mip = root OR CWD
  if (pathname[0]=='/')
     mip = root;
  else
     mip = running->cwd;

  mip->refCount++;         // because we iput(mip) later
  
  tokenize(pathname);

  for (i=0; i<n; i++){
     if(DEBUG){
      printf("===========================================\n");
      printf("getino: i=%d name[%d]=%s\n", i, i, name[i]);
      }

      ino = search(mip, name[i]);

      if (ino==0){
         iput(mip);
         if(DEBUG){
         printf("name %s does not exist\n", name[i]);}
         return 0;
      }
      iput(mip);
      mip = iget(dev, ino);
   }

   iput(mip);
   return ino;
}

// These 2 functions are needed for pwd()
int findmyname(MINODE *parent, u32 myino, char myname[ ]) 
{
  // WRITE YOUR code here
  // search parent's data block for myino; SAME as search() but by myino
  // copy its name STRING to myname[ ]

   int i; 
   char *cp, c, sbuf[BLKSIZE], temp[256];
   DIR *dp;
   INODE *ip;

   ip = &(parent->INODE);

   /*** search for name in mip's data blocks: ASSUME i_block[0] ONLY ***/

   get_block(dev, ip->i_block[0], sbuf);
   dp = (DIR *)sbuf;
   cp = sbuf;

   while (cp < sbuf + BLKSIZE){
     if (dp->inode == myino){
        strncpy(myname, dp->name, dp->name_len);
     }
     cp += dp->rec_len;
     dp = (DIR *)cp;
   }
   return 0;

}

int traverse(MINODE* source, char *pathname)
{
   tokenize(pathname);
   MINODE *currentNode = source;
   int nextNode;
   for(int i = 0; i < n; i++)
   {
      if((currentNode->INODE.i_mode & 0xF000) == 0x4000)
      {
         nextNode = search(currentNode, name[i]);
         if(nextNode != 0)
         {
            currentNode = iget(dev, nextNode);
         }
         else
         {
            return 0;
         }
      }
      else
      {
         return 0;
      }
   }
   return nextNode;
}

int findino(MINODE *mip, u32 *myino) // myino = i# of . return i# of ..
{
  // mip points at a DIR minode
  // WRITE your code here: myino = ino of .  return ino of ..
  // all in i_block[0] of this DIR INODE.

   int i; 
   char *cp, c, sbuf[BLKSIZE], temp[256];
   DIR *dp;
   INODE *ip;

   ip = &(mip->INODE);

   /*** search for name in mip's data blocks: ASSUME i_block[0] ONLY ***/

   get_block(dev, ip->i_block[0], sbuf);
   dp = (DIR *)sbuf;
   cp = sbuf;

   while (cp < sbuf + BLKSIZE){
     strncpy(temp, dp->name, dp->name_len);
     temp[dp->name_len] = 0;
     if (strcmp(temp, ".")==0){
        *myino = dp->inode;
     }
     cp += dp->rec_len;
     dp = (DIR *)cp;
   }
   return 0;
}

int enter_name(MINODE *mip, int ino, char *name, int isDir)
{
   get_block(dev, mip->INODE.i_block[0], buf);
   DIR *dp = (DIR *)buf;
   char *cp = buf;

   if(DEBUG) printf("Fetched dir block...\n");

   if(*cp == 0)
   {
      printf("In new block\n");
      dp->inode = ino;
      strcpy(dp->name, name);
      dp->name[strlen(name)] = 0;
      dp->name_len = strlen(name);
      dp->file_type = isDir? 'd' : 'r';
      dp->rec_len = BLKSIZE;

      put_block(dev, mip->INODE.i_block[0], buf);

      return 1;
   }

   while (cp + dp->rec_len < buf + BLKSIZE){
      if(DEBUG) printf("scanning ino %d, reclen = %d\n", dp->inode, dp->rec_len);
      cp += dp->rec_len;
      dp = (DIR *)cp;
   }

   if(DEBUG) printf("Found empty entry\n");

   int oldRecLength = dp->rec_len;
   dp->rec_len = sizeof(DIR) + dp->name_len - 1;
   int newRecLength = dp->rec_len;
   cp += dp->rec_len;
   dp = (DIR *)cp;

   dp->inode = ino;
   strcpy(dp->name, name);
   dp->name[strlen(name)] = 0;
   dp->name_len = strlen(name);
   dp->file_type = isDir? 'd' : 'r';
   dp->rec_len = oldRecLength - newRecLength;

   put_block(dev, mip->INODE.i_block[0], buf);

   return 1;
}

int rm_name(MINODE *mip, int ino, char *name)
{
   get_block(dev, mip->INODE.i_block[0], buf);
   DIR *dp = (DIR *)buf;
   char *cp = buf;
   DIR *lastdp;
   int deletedLength;

   char newBuf[BLKSIZE];
   char *newcp = newBuf;

   if(DEBUG) printf("Fetched dir block...\n");

   int foundTarget = 0;
   if (DEBUG) printf("Looking for inode = %d name = %s\n", ino, name);
   while (cp + dp->rec_len < buf + BLKSIZE){
      if(DEBUG) printf("scanning ino %d, reclen = %d\n", dp->inode, dp->rec_len);

      if(dp->inode == ino && strncmp(dp->name, name, dp->name_len) == 0){
         deletedLength = dp->rec_len;
         if (DEBUG) printf("Deleted %d bytes partway.\n", deletedLength);
      }
      else
      {
         lastdp = (DIR*)newcp;
         memcpy(newcp, cp, dp->rec_len);
         newcp += dp->rec_len;
      }

      cp += dp->rec_len;
      dp = (DIR *)cp;
   }

   if(strncmp(dp->name, name, dp->name_len) == 0)
   {
      lastdp->rec_len += dp->rec_len+1;
      if (DEBUG) printf("Deleted node at end\n");
   }
   else
   {
      memcpy(newcp, cp, dp->rec_len);
      ((DIR*)newcp)->rec_len += deletedLength;
   }

   put_block(dev, mip->INODE.i_block[0], newBuf);
}
