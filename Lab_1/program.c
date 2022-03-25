#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include "lib.h"

#define MAX_BUFFER_SIZE 128
#define TRUE 1

struct Metadata
{
    char Filename[MAX_BUFFER_SIZE]; //имя файла
    mode_t Filemode;                //тип файла и доступ
    uid_t File_uid;                 //id владельца
    gid_t File_gid;                 //id группы
    int Filesize;                   //размер файла в блоках по 128 байт
    
    struct timespec File_atim;      //время последнего доступа
    struct timespec File_mtim;      //время последней модификации
    //stackoverflow сказал что просто так не поменять
    //struct timespec File_ctim;       //время последней смены прав
};

int main(int argc, char *argv[])
{
    struct stat statbuf;
    lstat(argv[1], &statbuf);
    if (S_ISDIR(statbuf.st_mode))
    {
        Archive(argv[1], argv[2]);
    }
    else
    {
        Unarchive(argv[1], argv[2]);
    }
    exit(0);
}
