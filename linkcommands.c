#include "linkcommands.h"

int my_link()
{
    int oino = getino(pathname);
    if (DEBUG) printf("INO = %d\n", oino);
    MINODE *omip = iget(dev, oino);

    if((omip->INODE.i_mode & 0xF000) == 0x4000)
    {
        printf("Cannot link from a DIR\n");
        return 0;
    }

    if(getino(pathname2) != 0)
    {
        printf("File '%s' already exists.\n", pathname2);
        return 0;
    }

    strcpy(parent, dirname(pathname2));
    strcpy(child, basename(pathname2));
    int pino = getino(parent);
    MINODE *pmip = iget(dev, pino);
    if(DEBUG) printf("Entering name\n");

    enter_name(pmip, oino, child);
    if(DEBUG) printf("Entered new DIR\n");

    omip->INODE.i_links_count++;
    omip->dirty = 1;
    iput(omip);
    iput(pmip);

    return 1;
}