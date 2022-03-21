#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
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
    struct timespec File_ctim;       //время последней смены прав
};

void RecursiveArchive(char *dir, int outFD);

void Archive(char *dir, char *out)
{
    int out_fd;
    out_fd = open(out, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    //просто поверь, это работает
    chdir(dir);
    strcpy(dir, strrchr(getcwd(dir, MAX_BUFFER_SIZE),'/') + 1);
    chdir("..");
    write (out_fd, dir, MAX_BUFFER_SIZE);
    RecursiveArchive(dir, out_fd);
    close(out_fd);
}

void RecursiveArchive(char *dir, int outfd)
{
    char buf[MAX_BUFFER_SIZE] = "\0";
    int infd;
    DIR *dirp;
    struct dirent *entry;
    struct stat statbuf;
    struct Metadata mdbuf;

    if ((dirp = opendir(dir)) == NULL)
    {
        write(2, "We made a fuckie-wuckie, uwu", 28);
        exit(1);
    }

    while ((entry = readdir(dirp)) != NULL)
    {
        strcat(dir, "/");
        strcat(dir, entry->d_name);
        lstat(dir, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            if (strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0)
            {

            //заносим метаданные о папке
            strcpy(mdbuf.Filename, dir);
            mdbuf.Filemode = statbuf.st_mode;
            mdbuf.File_uid = statbuf.st_uid;
            mdbuf.File_gid = statbuf.st_gid;
            mdbuf.File_atim = statbuf.st_atim;
            mdbuf.File_mtim = statbuf.st_mtim;
            mdbuf.File_ctim = statbuf.st_ctim;
            mdbuf.Filesize = 0;
            write(outfd, &mdbuf, sizeof(mdbuf));
            RecursiveArchive(dir, outfd);
            }
        }
        else
        {
            strcpy(mdbuf.Filename, dir);
            mdbuf.Filemode = statbuf.st_mode;
            mdbuf.File_uid = statbuf.st_uid;
            mdbuf.File_gid = statbuf.st_gid;
            mdbuf.File_atim = statbuf.st_atim;
            mdbuf.File_mtim = statbuf.st_mtim;
            mdbuf.File_ctim = statbuf.st_ctim;
            mdbuf.Filesize = statbuf.st_size / MAX_BUFFER_SIZE + (statbuf.st_size%MAX_BUFFER_SIZE != 0);
            write(outfd, &mdbuf, sizeof(mdbuf));

            infd = open(dir, O_RDONLY);
            while (read(infd, buf, MAX_BUFFER_SIZE) != 0)
                write(outfd, buf, MAX_BUFFER_SIZE);
        }
        
        dir[(int)(strrchr(dir, '/') - dir)]='\0';
    }

}
//старая версия (моооожет потом ее)
/* void RecursiveArchiveOLD(char *dir, int outFD)
{
    char buf[MAX_BUFFER_SIZE] = "\0";
    int in_fd, out_fd;
    ssize_t size;
    DIR *dirp;
    struct dirent *entry;
    struct stat statbuf;
    struct Metadata mdbuf;

    if ((dirp = opendir(dir)) == NULL)
    {
        write(2, "We made a fuckie-wuckie, uwu", 28);
        exit(1);
    }

    chdir(dir);

    while ((entry = readdir(dirp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;

            //заносим метаданные о папке
            strcpy(mdbuf.Filename, strcat(strcat(dir, "/"), entry->d_name));
            strcpy(mdbuf.FileType, "Folder");
            size = 0;
            write(outFD, &mdbuf, sizeof(mdbuf));
            //переходим внутрь папки и вызываем функцию рекурсивно
            RecursiveArchive(entry->d_name, outFD);
        }
        else
        {
            //открытие архивируемого файла
            in_fd = open(entry->d_name, O_RDONLY);

            //запись метаданных
            strcpy(mdbuf.Filename, entry->d_name);
            strcpy(mdbuf.FileType, "Binary");
            mdbuf.Filesize = lseek(in_fd, SEEK_END, 0)/MAX_BUFFER_SIZE + (lseek(in_fd, SEEK_END, 0)%MAX_BUFFER_SIZE != 0);
            write(outFD, &mdbuf, sizeof(mdbuf));

            //запись в архив содержимого архивируемого файла
            lseek(in_fd, SEEK_SET, 0);
            while (read(in_fd, buf, MAX_BUFFER_SIZE) != 0)
                write(outFD, buf, MAX_BUFFER_SIZE);

            //закрытие архивируемого файла
            close(in_fd);
        }
    }
    //закончили с этой папкой, возвращаемся на уровень выше
    chdir("..");
    close(out_fd);
} */

void Unarchive(char *in, char *dir)
{
    char buf[MAX_BUFFER_SIZE] = "\0";
    int infd, out_fd;
    ssize_t size;
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    struct Metadata mdbuf;

    //открытие архива
    infd = open(in, O_RDONLY);

    chdir(dir);
    read(infd, buf, MAX_BUFFER_SIZE);
    mkdir(buf, S_IRWXU);

    while (read(infd, buf, MAX_BUFFER_SIZE) == MAX_BUFFER_SIZE)
    {
        if (1)
        {

        }

        mkdir(buf, S_IRWXU);
    }
}

int main(int argc, char *argv[])
{
    Archive(argv[1], argv[2]);
    //Unarchive("out", "../../");
    exit(0);
}