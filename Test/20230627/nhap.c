#include <stdio.h>
#include <dirent.h>

int main()
{
    const char *path = "./btd.txt";

    DIR *dir = opendir(path);
    if (dir != NULL)
    {
        printf("%s is a directory.\n", path);
        closedir(dir);
    }
    else
    {
        printf("%s is a file.\n", path);
    }

    return 0;
}