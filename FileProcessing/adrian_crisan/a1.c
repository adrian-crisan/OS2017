#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>

struct section {
    char name[15];
    int offset, size;
    short type;
};
//simple listing
void simpleListing(char *dirName) {
    DIR* dir;
    struct dirent *dirEntry;
    dir = opendir(dirName);
    printf("SUCCESS\n");
    if (dir == NULL) {
        printf("invalid directory path\n");
        exit(0);
    }
    while ((dirEntry = readdir(dir)) != NULL) {
        if (strcmp(".",dirEntry -> d_name) && strcmp("..",dirEntry -> d_name))
            printf("%s/%s\n", dirName, dirEntry -> d_name);
    }
    closedir(dir);
}
//recursive listing
int isDir(const char *parent, char *name) {
    struct stat st_buf;
    if (!strcmp(".", name) || !strcmp("..", name))
        return 0;

    char *path = alloca(strlen(name) + strlen(parent) + 2);
    sprintf(path, "%s/%s", parent, name);
    stat(path, &st_buf);
    return S_ISDIR(st_buf.st_mode);
}

void recursiveListing(const char* dirName) {
    DIR *dir;
    char name[1000];
    struct dirent *dirEntry;
    dir = opendir(dirName);
    if (dir == NULL) {
        printf("invalid directory path\n");
        exit(0);
    }
    while ((dirEntry = readdir(dir)) != NULL) {
        char *entry = dirEntry -> d_name;
         sprintf(name, "%s/%s", dirName, dirEntry -> d_name);
         if (strcmp(".",dirEntry -> d_name) && strcmp("..",dirEntry -> d_name) && strcmp("test_root", dirEntry -> d_name)){
                printf("%s\n", name);
         }
        if (isDir(dirName, entry)) {
            char *next = malloc(strlen(dirName) + strlen(entry) + 2);

            sprintf(next, "%s/%s", dirName, entry);
            recursiveListing(next);
            free(next);
        }
    }
    closedir(dir);
}
//recursive listing name_starts_with
void recursiveListingName(const char* dirName, char* start) {
    DIR *dir;
    char name[1000];
    struct dirent *dirEntry;
    dir = opendir(dirName);
    if (dir == NULL) {
        printf("invalid directory path\n");
        exit(0);
    }
    int len = (int)strlen(start);
    while ((dirEntry = readdir(dir)) != NULL) {
        char *entry = dirEntry -> d_name;
         sprintf(name, "%s/%s", dirName, dirEntry -> d_name);
         if (strcmp(".",dirEntry -> d_name) && strcmp("..",dirEntry -> d_name) && strcmp("test_root", dirEntry -> d_name)){
                if (!(strncmp(dirEntry -> d_name, start, len))) {
                    printf("%s\n", name);
                    break;
                }
         }
        if (isDir(dirName, entry)) {
            char *next = malloc(strlen(dirName) + strlen(entry) + 2);

            sprintf(next, "%s/%s", dirName, entry);
            recursiveListing(next);
            free(next);
        }
    }
    closedir(dir);
}
//simple listing name_starts_with
void simpleListingName(char* dirName, char* start) {
    DIR* dir;
    struct dirent *dirEntry;
    char name[10000];
    int len = (int)strlen(start);
    dir = opendir(dirName);
    if (dir == NULL) {
        printf("invalid directory path\n");
        exit(0);
    }
    while ((dirEntry = readdir(dir)) != NULL) {
        sprintf(name, "%s/%s", dirName, dirEntry -> d_name);
        if (strcmp(".",dirEntry -> d_name) && strcmp("..",dirEntry -> d_name))
            if (!(strncmp(dirEntry -> d_name, start, len)))
                printf(" %s\n", name);
    }
    closedir(dir);
}
//simple listing has_perm_write
void simpleListingPerm(char *dirName) {
    DIR* dir;
    struct dirent *dirEntry;
    struct stat fileMetadata;
    char name[10000];
    dir = opendir(dirName);
   if (dir == NULL) {
        printf("invalid directory path\n");
        exit(0);
    }
    while ((dirEntry = readdir(dir)) != NULL) {
        sprintf(name, "%s/%s", dirName, dirEntry -> d_name);
        stat(name, &fileMetadata);
        if (strcmp(".",dirEntry -> d_name) && strcmp("..",dirEntry -> d_name)) {
            if (fileMetadata.st_mode & S_IWUSR) {
                printf(" %s\n", name);
            }
        }
    }
    closedir(dir);
}
//recursive listing has_perm_write
void recursiveListingPerm(const char* dirName) {
    DIR *dir;
    char name[1000];
    struct dirent *dirEntry;
    struct stat fileMetadata;
    dir = opendir(dirName);
    if (dir == NULL) {
        printf("invalid directory path\n");
        exit(0);
    }
    while ((dirEntry = readdir(dir)) != NULL) {
        char *entry = dirEntry -> d_name;
         sprintf(name, "%s/%s", dirName, dirEntry -> d_name);
         stat(name, &fileMetadata);
         if (strcmp(".",dirEntry -> d_name) && strcmp("..",dirEntry -> d_name) && strcmp("test_root", dirEntry -> d_name)){
                if (fileMetadata.st_mode & S_IWUSR) {
                    printf("%s\n", name);
                }
         }
        if (isDir(dirName, entry)) {
            char *next = malloc(strlen(dirName) + strlen(entry) + 2);

            sprintf(next, "%s/%s", dirName, entry);
            recursiveListing(next);
            free(next);
        }
    }
    closedir(dir);
}
//parse and corrupted
void parse(char* fileName) {
    int file;
    if ((file = open(fileName, O_RDONLY)) == -1) {
        printf("Could not open file!\n");
        exit(0);
    }
    lseek(file, -4, SEEK_END);
    int nrread;
    char buf[4];
    if ((nrread = read(file, buf, sizeof(buf))) == 0) {
        printf("could not read magic\n");
        exit(0);
    }
    if (!(strstr(buf, "S5x2"))) {
        printf("ERROR\n");
        printf("wrong magic\n");
        exit(1);
    }
    lseek(file, -6, SEEK_END);
    int hsize = 0;
    nrread = read(file, &hsize, 2);
    lseek(file, -hsize, SEEK_END);
    char version[2];
    int no_sect;
    if ((nrread = read(file, version, 2)) == 0) {
        printf("ERROR\n");
        exit(0);
    }
    int vs = ((version[1] *0x100)+version[0]);

    if (vs < 22 || vs > 125) {
        printf("ERROR\n");
        printf("wrong version\n");
        exit(0);
    }
    char sct[1];
    if ((nrread = read(file, sct, 1)) == 0) {
        printf("could not read no_sect\n");
        exit(0);
    }
    no_sect = sct[0];
    if (no_sect < 6 || no_sect > 17) {
        printf("ERROR\n");
        printf("wrong sect_nr\n");
        exit(0);
    }
    char num[15];
    struct section sect[no_sect];
    for (int i = 0; i < no_sect; i++) {
        char tip[1];
        nrread = read(file, num, sizeof (num));
        nrread = read(file, tip, 1);
        sect[i].type = tip[0];
        if (sect[i].type != 13 && sect[i].type != 67 && sect[i].type != 72 && sect[i].type != 59 && sect[i].type != 56 && sect[i].type != 10 && sect[i].type != 25 ) {
            printf("ERROR\n");
            printf("wrong sect_types\n");
            exit(0);
        }
        nrread = read(file, &sect[i].offset, 4);
        nrread = read(file, &sect[i].size, 4);
        strcpy (sect[i].name, num);
    }
    printf("SUCCESS\n");
    printf("version=%d\n", vs);
    printf("nr_sections=%d\n", no_sect);
    for (int i = 0; i < no_sect; i++) {
        printf("section%d: %s %d %d\n", i+1, sect[i].name, sect[i].type, sect[i].size);
    }

}
//findall
void findall(char* dirName) {
    DIR *dir;
    char name[1000];
    struct dirent *dirEntry;
    struct stat inode;
    dir = opendir(dirName);
    if (dir == NULL) {
        printf("invalid directory path\n");
        exit(0);
    }
    while ((dirEntry = readdir(dir)) != NULL) {
         char *entry = dirEntry -> d_name;
         sprintf(name, "%s/%s", dirName, dirEntry -> d_name);
         lstat(name, &inode);
         int file = open(name, O_RDONLY);
         if (S_ISREG(inode.st_mode)) {
            if ((file = open(name, O_RDONLY)) == -1) {
                printf("Could not open file!\n");
                exit(0);
            }
         }
         lseek(file, -4, SEEK_END);
         int nrread;
         char buf[4];
         if ((nrread = read(file, buf, sizeof(buf))) == 0) {
            printf("could not read magic\n");
            exit(0);
         }
         lseek(file, -6, SEEK_END);
         int hsize = 0;
         nrread = read(file, &hsize, 2);
         lseek(file, -hsize, SEEK_END);
         char version[2];
         int no_sect;
         if ((nrread = read(file, version, 2)) == 0) {
            printf("ERROR\n");
            exit(0);
         }
         char sct[1];
         if ((nrread = read(file, sct, 1)) == 0) {
            printf("could not read no_sect\n");
            exit(0);
         }
         no_sect = sct[0];
         char num[15];
         struct section sect[no_sect];
         for (int i = 0; i < no_sect; i++) {
            char tip[1];
            nrread = read(file, num, sizeof (num));
            nrread = read(file, tip, 1);
            sect[i].type = tip[0];
            nrread = read(file, &sect[i].offset, 4);
            nrread = read(file, &sect[i].size, 4);
            strcpy (sect[i].name, num);
         }
         int check = 0;
         for (int i = 0; i < no_sect; i++) {
             if (strcmp(".",dirEntry -> d_name) && strcmp("..",dirEntry -> d_name) && strcmp("test_root", dirEntry -> d_name)){
                    if (sect[i].type == 10) {
                        check++;
                    }
            }
         }
         if (S_ISREG(inode.st_mode)) {
            if (check > 0) {
                printf("%s\n", name);
            }
         }
        if (isDir(dirName, entry)) {
            char *next = malloc(strlen(dirName) + strlen(entry) + 2);
            sprintf(next, "%s/%s", dirName, entry);
            findall(next);
            free(next);
        }
    }
    closedir(dir);
}

int main(int argc, char **argv){

    if(argc >= 2){
        if(strstr(argv[1], "variant")){
            printf("27635\n");
        }
        if (strstr(argv[1], "list")) {
            if (strncmp(argv[2], "path=", 5) == 0) {
                char substr[100];
                sscanf (argv[2], "path=%s", substr);
                simpleListing(substr);
            }
            if (strstr(argv[2], "recursive")) {
                if (strncmp(argv[3], "path=", 5) == 0) {
                    printf("SUCCESS\n");
                    char substr[100];
                    sscanf(argv[3], "path=%s", substr);
                    recursiveListing(substr);
                }
            }
            if (strncmp(argv[2], "name_starts_with=", 17) == 0) {
                if (strncmp(argv[3], "path=", 5) == 0) {
                    printf("SUCCESS\n");
                    char substr[100];
                    char start[100];
                    sscanf(argv[2], "name_starts_with=%s", start);
                    sscanf(argv[3], "path=%s", substr);
                    simpleListingName(substr, start);
                }
            }
            if (strstr(argv[2], "recursive")) {
                if (strncmp(argv[3], "name_starts_with=", 17) == 0) {
                    if (strncmp(argv[4], "path=", 5) == 0) {
                        printf("SUCCESS\n");
                        char substr[100];
                        char start[100];
                        sscanf(argv[2], "name_starts_with=%s", start);
                        sscanf(argv[3], "path=%s", substr);
                        recursiveListingName(substr, start);
                    }
                }
            }
            if (strncmp(argv[2], "has_perm_write", 14) == 0) {
                if (strncmp(argv[3], "path=", 5) == 0) {
                    printf("SUCCESS\n");
                    char substr[100];
                    sscanf(argv[3], "path=%s", substr);
                    simpleListingPerm(substr);
                }
            }
            if (strstr(argv[2], "recursive")) {
                if (strncmp(argv[3], "has_perm_write", 14) == 0) {
                    if (strncmp(argv[4], "path=", 5) == 0) {
                        printf("SUCCESS");
                        char substr[100];
                        sscanf(argv[4], "path=%s", substr);
                        recursiveListingPerm(substr);
                    }
                }
            }
        }
        if (strstr(argv[1], "parse")) {
            if (strncmp(argv[2], "path=", 5) == 0) {
                char substr[100];
                sscanf(argv[2], "path=%s", substr);
                parse(substr);
            }
        }
        if (strstr(argv[1], "findall")) {
            if (strncmp(argv[2], "path=", 5) == 0) {
                printf("SUCCESS\n");
                char substr[1000];
                sscanf(argv[2], "path=%s", substr);
                findall(substr);
            }
        }
    }

    return 0;
}
