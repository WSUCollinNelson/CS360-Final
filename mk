rm a.out 2> /dev/null

gcc main.c util.c linkcommands.c alloc.c dir_commands.c minor_commands.c cd_ls_pwd.c

./mkdisk
