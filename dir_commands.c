#include "util.h"
#include "dir_commands.h"
#include "alloc.h"

int imkdir()
{
    strcpy(child, basename(pathname));
    strcpy(parent, dirname(pathname));

    printf("Dirname: %s Basename: %s\n", parent, child);

    pino = getino(parent);
    pmip= iget(dev, pino);

    printf("pino: %d pmip->pino: %d\n", pino, pmip->ino);

    printf("Checking if %s is a dir...\n", parent);
    if ((pmip->INODE.i_mode & 0xF000) != 0x4000) 
    {
        printf("%s is not a directory\n", parent);
        return 0;
    }

    printf("Checking if %s already exists in %s\n", child, parent);
    if(search(pmip, child) != 0)
    {
        printf("Dir %s already exists in %s\n", child, parent);
        return 0;
    }

    ikmkdir(pmip, child);
}

int ikmkdir(MINODE* pmip, char* basename)
{
    int ino = ialloc(dev);
    int blk = balloc(dev);

    MINODE* mip = iget(dev, ino);

    INODE *ip = &mip->INODE;
    ip->i_mode = 0x41ED; // 040755: DIR type and permissions
    ip->i_uid = running->uid; // owner uid
    ip->i_gid = running->gid; // group Id
    ip->i_size = BLKSIZE; // size in bytes
    ip->i_links_count = 2; // links count=2 because of . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 2; // LINUX: Blocks count in 512 byte chunks
    ip->i_block[0] = ino; // new DIR has one data block

    for(int i = 1; i < 14; i++ )
    {
        ip->i_block[i] = 0;
    }

    mip->dirty = 1; // mark minode dirty

    iput(mip);

    enter_name(mip, ino, ".", 1);
    enter_name(mip, pmip->ino, "..", 1);
    enter_name(pmip, ino, child, 1);
}

int irmdir()
{
    strcpy(child, basename(pathname));
    strcpy(parent, dirname(pathname));

    ino = getino(child);
    mip= iget(dev, ino);

    printf("ino: %d mip->ino: %d\n", ino, mip->ino);

    printf("Checking if %s is a dir...\n", child);
    if ((mip->INODE.i_mode & 0xF000) != 0x4000) 
    {
        printf("%s is not a directory\n", child);
        return 0;
    }

    if (mip->INODE.i_links_count > 2)
    {
        printf("Can not remove a directory that isn't empty\n");
        return 0;
    }

    if ((strcmp(child, ".") == 0) || (strcmp(child, "..") == 0))
    {
        printf("Can not remove %s directory\n", child);
        return 0;
    }

    pino = getino(parent);
    pmip = iget(mip->dev, pino);
    printf("ino: %d mip->ino: %d\n", pino, pmip->ino);

    findmyname(pmip, ino, nodeName);
    printf("name: %s\n", nodeName);

    rm_name(pmip, ino, nodeName);

    pmip->INODE.i_links_count--;
    pmip->dirty = 1;

    iput(mip);

    //bdalloc(mip->dev, mip->INODE.i_block[0]);
    idalloc(mip->dev, mip->ino); //FREE DATA
    iput(mip);
}