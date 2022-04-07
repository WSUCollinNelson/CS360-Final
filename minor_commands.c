#include "minor_commands.h"
#include "cd_ls_pwd.h"

int my_stat()
{
    struct stat myst;
    int ino = getino(pathname);
    char ftime[29];

    if (ino == 0)
    {
        printf("Pathname is incorrect\n");
        return 0;
    }

    MINODE* mip = iget(dev, ino);
    printf("ino: %d mip->ino: %d\n", ino, mip->ino);

    myst.st_dev = dev;
    myst.st_ino = ino;
    myst.st_mode = mip->INODE.i_mode;
    myst.st_nlink = mip->INODE.i_links_count;
    myst.st_uid = mip->INODE.i_uid;

    printf("File: %s\n", pathname);
    printf("Size: %d \t Blocks: %d \n", mip->INODE.i_size, mip->INODE.i_blocks);
    printf("Device: %d  \t Inode: %d \t Links: %d\n", dev, ino, mip->INODE.i_links_count);

    printf("Access: (0644/");
    if ((mip->INODE.i_mode & 0xF000) == 0x8000) // if (S ISREG()) 
        printf("%c", '-'); 
    if ((mip->INODE.i_mode & 0xF000) == 0x4000) // if (S ISDIR()) 
        printf("%c",'d'); 
    if ((mip->INODE.i_mode & 0xF000) == 0xA000) // if (S ISLNK()) 
        printf("%c",'l'); 
    for (int i=8; i >= 0; i-- )
    { 
        if (mip->INODE.i_mode & (1 << i)) // print r|w|x 
            printf( "%c", t1[i]); 
        else 
            printf( "%c", t2[i]); // or print 
    } 

    printf(") \t Uid: %d \t Gid: %d\n", mip->INODE.i_uid, mip->INODE.i_gid);

    time_t dateValueA = mip->INODE.i_atime;
    ctime_r(&dateValueA, ftime);
    ftime[strlen(ftime) - 1] = 0; // kill \n at end  

    printf("Access: %28s\n",ftime);

    time_t dateValueM = mip->INODE.i_mtime;
    ctime_r(&dateValueM, ftime);
    ftime[strlen(ftime) - 1] = 0; // kill \n at end  

    printf("Modify: %28s\n", ftime);
    
    time_t dateValueC = mip->INODE.i_ctime;
    ctime_r(&dateValueC, ftime);
    ftime[strlen(ftime) - 1] = 0; // kill \n at end  

    printf("Change: %28s\n", ftime);
}

int my_chmod()
{

}

int my_utime()
{

}