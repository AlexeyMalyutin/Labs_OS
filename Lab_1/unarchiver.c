#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

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


void Unarchive(char *in, char *dir)
{
    char buf[MAX_BUFFER_SIZE] = "\0";
    int infd, outfd;
    struct Metadata mdbuf;

    //открытие архива
    if ((infd = open(in, O_RDONLY)) == -1)
    {
        write(2, "ERROR\n", 6);
        exit(1);
    }

    write(2, "govno otkril\n", sizeof("govno otkril\n"));


    chdir(dir);
    read(infd, buf, MAX_BUFFER_SIZE);
    mkdir(buf, S_IRWXU);
    
    while (read(infd, &mdbuf, sizeof(mdbuf)) == sizeof(mdbuf))
    {
        write(2, "govnodir\n", sizeof("govnodir\n"));
        if (S_ISDIR(mdbuf.Filemode))
        {
            mkdir(mdbuf.Filename, mdbuf.Filemode);
            chown(mdbuf.Filename, mdbuf.File_uid, mdbuf.File_gid);
            struct utimbuf FileTimes;
            FileTimes.actime = mdbuf.File_atim.tv_sec;
            FileTimes.modtime = mdbuf.File_mtim.tv_sec;
            utime(mdbuf.Filename, &FileTimes);
        }
        else
        {
            write(2, "govnofile\n", sizeof("govnofile\n"));
            outfd = open(mdbuf.Filename, O_WRONLY | O_CREAT, mdbuf.Filemode);

            for (int i = 0; i< mdbuf.Filesize; i++)
            {
                int size = read(infd, buf, MAX_BUFFER_SIZE);
                write(outfd, buf, strlen(buf));
            }
            chown(mdbuf.Filename, mdbuf.File_uid, mdbuf.File_gid);
            struct utimbuf FileTimes;
            FileTimes.actime = mdbuf.File_atim.tv_sec;
            FileTimes.modtime = mdbuf.File_mtim.tv_sec;
            utime(mdbuf.Filename, &FileTimes);
            close(outfd);    
        }
    }
    close(infd);
}
