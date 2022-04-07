#include "util.h"
#include "dir_commands.h"
#include "alloc.h"

#define DEBUG 1

int my_mkdir()
{
    char pname[128];

    // tokenize
    printf("before tokenize\n");
    tokenize(pathname);
    printf("after tokenize\n");
    // verify dirname is a dir and exists

    MINODE* pmip;
    int pino;

    if (n == 1)
    {
        strcpy(parent, ".");
        strcpy(child, name[0]);
        strcpy(pname, "./"); 
        strcat(pname, pathname);
        n++;

        pino = getino(parent);
        pmip = iget(dev, pino);

        printf("checking %s: %d for being a dir \n", parent, pino);

        if ((pmip->INODE.i_mode & 0xF000) != 0x4000) 
        {
            printf("%s is not a directory\n", parent);
            return 0;
        }

        printf("after checking each directory\n");
    }
    else
    {
        strcpy(parent, name[n-2]);
        strcpy(child, name[n-1]);

        for (int i = 0; i < n -1; i++)
        {
            strcat(pname, name[i]);
            strcat(pname, "/");
            pino = getino(pname);
            pmip = iget(dev, pino);

            printf("checking %s: %d for being a dir \n", name[i], pino);
            

            if ((pmip->INODE.i_mode & 0xF000) != 0x4000) 
            {
                printf("%s is not a directory\n", name[i]);
                return 0;
            }
        }
    }

    printf("parent: %s child: %s\n", parent, child);

    printf("before checking each directory\n");
    //for (int i = 0; i < n -2; i++)
    //{
    //    pino = getino(name[i]);
    //    pmip = iget(dev, pino);

    //    printf("checking %s: %d for being a dir \n", name[i], pino);

    //    if ((pmip->INODE.i_mode & 0xF000) != 0x4000) 
    //    {
    //        printf("%s is not a directory\n", name[i]);
    //        return 0;
    //    }
    //}

    

    // verify base name isn't in parent Dir 
    // search (pmip, bsename) == 0

    printf("before searching for %s in %s\n", child, pname);
    if (getino(pname) != 0) 
    {
        printf("%s already exists in %s\n", child, parent);
        return 0;
    }
    printf("after searching for %s in parent dir\n", child);
    // create a dir, call kmkdir
    printf("before kmkdir\n");
    kmkdir(pmip, child);
}

int kmkdir(MINODE* pmip, char* basename)
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
    enter_name(mip, ino, ".", 1);
    printf("before enter_name for pino: %d\n", pmip->ino);
    enter_name(mip, pmip->ino, "..", 1);
    // sets parent's child to this inode
    printf("before enter_name for new node\n");
    enter_name(pmip, ino, child, 1);
}

int imkdir()
{
    strcpy(parent, dirname(pathname));
    strcpy(child, basename(pathname));

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