#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define MAX_BUFFER_SIZE 128

void arch(char*dir, char* out)
{
    char buf[MAX_BUFFER_SIZE]= "\0";
    int in_fd, out_fd;
    ssize_t size;
    DIR* dp;
    struct dirent *entry;
    struct stat statbuf;

    out_fd = open(out, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);

    if ((dp = opendir(dir))==NULL)
    {
        write(2, "We made a fuckie-wuckie, uwu", 28);
        exit(1);
    }

    chdir(dir);
    while ((entry = readdir(dp))!= NULL)
    {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
            continue;

        //открытие архивируемого файла
        in_fd = open(entry->d_name, O_RDONLY);

        //запись в архив имени архивируемого файла
        strcpy(buf, entry->d_name);
        write(out_fd, buf, MAX_BUFFER_SIZE);

        //запись в архив размера архивируемого файла
        size = lseek(in_fd, 0, SEEK_END);
        lseek(in_fd, 0, SEEK_SET);
        write(out_fd, &size, sizeof(size));

        //запись в архив содержимого архивируемого файла
        while ((size = read(in_fd, buf, MAX_BUFFER_SIZE)))
            write(out_fd, buf, size);

        close(in_fd);
    }
    close(out_fd);
}

void unarch()
{
    
}

int main(int argc, char *argv[])
{
    arch(argv[1], argv[2]);
    exit(0);
}