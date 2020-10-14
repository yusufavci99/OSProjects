#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

int main (int argc, char *argv[]) {
    struct dirent *pDirent;
    struct stat stats;
    DIR *pDir;

    // Argument count control
    if (argc != 2) {
        printf ("Wrong number of arguments entered!\n");
        return 1;
    }

    // Opening the directory
    pDir = opendir (argv[1]);
    if (pDir == NULL) {
        printf ("Cannot open directory '%s'\n", argv[1]);
        return 1;
    }

    // Adding '/' to end of the path if there is no '/' in the end of path
    char dir_path[256];
    strcpy(dir_path, argv[1]);
    printf("%s%s\n", "Given directory path: ", dir_path);
    if(dir_path[strlen(dir_path) - 1] != '/') {
        strcat(dir_path, "/");
        printf("%s%s\n", "Added '/' to the end: ", dir_path);
    }
    printf("\n");

    // Iterating every directory entry in the directory
    char cur_file_path[256];
    while ((pDirent = readdir(pDir)) != NULL) {
        
        // Finding paths of each dirent
        strcpy(cur_file_path, dir_path);
        strcat(cur_file_path, pDirent->d_name);
        printf("%s%s\n", "Current file path: ", cur_file_path);

        // Print required properties
        printf ("%s%s\n", "File (or subdirectory) name: ",pDirent->d_name);
        if (stat(cur_file_path, &stats) == 0) {
            printf("%s%ld\n", "Inode number: ", (long) stats.st_ino);
            printf("%s", "File type: ");

            switch (stats.st_mode & S_IFMT) {
                case S_IFBLK:  printf("%s", "Block device\n");        break;
                case S_IFCHR:  printf("%s", "Character device\n");    break;
                case S_IFDIR:  printf("%s", "Directory\n");           break;
                case S_IFIFO:  printf("%s", "FIFO/pipe\n");           break;
                case S_IFLNK:  printf("%s", "Symlink\n");             break;
                case S_IFREG:  printf("%s", "Regular file\n");        break;
                case S_IFSOCK: printf("%s", "Socket\n");              break;
                default:       printf("%s", "Unknown?\n");            break;
            }

            printf("%s%lld\n", "Number of Blocks: ", (long long) stats.st_blocks);
            printf("%s%lld\n", "Size (In Bytes): ",(long long) stats.st_size);
            printf("%s%ld\n\n", "User ID: ", (long) stats.st_uid);
        }
        else {
            printf("Unable to get file properties.\n");
            printf("Please check whether '%s' file exists.\n\n", pDirent->d_name);
        }
    }

    // Close
    closedir (pDir);
    return 0;
}