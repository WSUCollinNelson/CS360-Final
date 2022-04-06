#include "util.h"
#include "dir_commands.h"
#include "alloc.h"

int my_mkdir()
{
    // tokenize
    printf("before tokenize\n");
    tokenize(pathname);
    printf("after tokenize\n");
    // verify dirname is a dir and exists

    MINODE* pmip;
    int pino;

    printf("before checking each directory\n");
    for (int i = 0; i < n -1; i++)
    {
        pino = getino(name[i]);
        pmip = iget(dev, pino);

        printf("checking %s: %d for being a dir \n", name[i], pino);

        if ((pmip->INODE.i_mode & 0xF000) != 0x4000) 
        {
            printf("%s is not a directory\n", name[i]);
            return 0;
        }
    }
    printf("after checking each directory\n");

    // verify base name isn't in parent Dir 
    // search (pmip, bsename) == 0

    printf("before searching for %s in parent dir\n", name[n-1]);
    if (getino(pathname) != 0) 
    {
        printf("%s already exists in %s\n", name[n -1], name[n-2]);
        return 0;
    }
    printf("after searching for %s in parent dir\n", name[n-1]);
    // create a dir, call kmkdir
    printf("before kmkdir\n");
    kmkdir(pmip, pino, name[n-1]);
}

int kmkdir(MINODE* pmip, int pino, char* basename)
{
    // Allocate an INODE and disk block
    printf("before ialloc\n");
    int ino = ialloc(dev);
    printf("before balloc\n");
    int blk = balloc(dev);
    // load inode into minode
    MINODE* mip = iget(dev, ino);
    printf("after iget\n");

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

    
    iput(mip); // write INODE to disk

    (mip->INODE.i_mode &= 0x0FFF);
    (mip->INODE.i_mode |= 0x4000);

    printf("after bit math\n");

    mip->INODE.i_block[0] = blk;
    printf("after setting 1st i block to blk\n");

    printf("before setting other blocks to 0\n");
    for(int i = 1; i < 12; i++)
    {
        mip->INODE.i_block[i] = 0;
    }
    printf("after setting other blocks to 0\n");
    mip->dirty = 1;

    iput(mip);
    // make data block 0 of INODE contain . and ..

    printf("before enter_name for ino\n");
    enter_name(mip, ino, ".");
    printf("before enter_name for pino\n");
    enter_name(mip, pmip->ino, "..");
    // sets parent's child to this inode
    printf("before enter_name for new node\n");
    enter_name(pmip, ino, name[n-1]);
}