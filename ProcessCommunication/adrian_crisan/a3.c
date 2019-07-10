#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

/*size_t getFileSize(const char* fileName) {
    struct stat st;
    stat(fileName, &st);
    return st.st_size;
}*/

int main() {
    int fd1 = 0;
    int fd2 = 0;
    char *connect = {"CONNECT"};
    char *success = {"SUCCESS"};
    char *error = {"ERROR"};
    char *pong = {"PONG"};
    int variant = 27635;
    int seven = 7;
    int four = 4;
    int five = 5;
    int len = 0;
    key_t key = 12005;

    if (mkfifo("RESP_PIPE_27635", 0644) < 0) {
        printf("ERROR\n");
        printf("cannot create the response pipe\n");
        exit(1);
    }
    fd1 = open("RESP_PIPE_27635", O_RDWR);
    fd2 = open("REQ_PIPE_27635", O_RDWR);
    if (fd2<0) {
        printf("ERROR\n");
        printf("cannot open the request pipe\n");
        exit(1);
    }
    else {
        printf("SUCCESS\n");
    }

    write(fd1, &seven, 1);
    write(fd1, connect, seven);

    int shmem = 0;
    char* ptr = NULL;
    char* file = NULL;
    int fd = 0;
    unsigned int p1 = 0;
    char* mappedData = NULL;
    off_t fileSize = 0;
    int offset = 0;
    int value = 0;
    int len1 = 0;

    while (1) {

        read(fd2, &len, 1);
        char *buf = (char*)malloc(sizeof(char)*len);
        read(fd2, buf, len);
        if (strcmp(buf, "PING") == 0) {
            write(fd1, &len, 1);
            write(fd1, buf, len);
            write(fd1, &four, 1);
            write(fd1, pong, four);
            write(fd1, &variant, 5);
        }
        else if (strcmp("CREATE_SHM", buf) == 0) {
            read(fd2, &p1, 7);
            shmem = shmget(key, p1, IPC_CREAT|0664);
            ptr = (char*)shmat(shmem, NULL, 0);
            if (shmem == -1) {
                write(fd1, &len, 1);
                write(fd1, buf, len);
                write(fd1, &five, 1);
                write(fd1, error, five);
            }
            else if (ptr < (char*)0) {
                write(fd1, &len, 1);
                write(fd1, buf, len);
                write(fd1, &five, 1);
                write(fd1, error, five);
            }
            else {
                write(fd1, &len, 1);
                write(fd1, buf, len);
                write(fd1, &seven, 1);
                write(fd1, success, seven);
            }
        }
        else if (strcmp("WRITE_TO_SHM", buf) == 0) {
            read(fd2, &offset, sizeof(int));
            if (offset > p1 || offset < 0) {
                write(fd1, &len, 1);
                write(fd1, buf, len);
                write(fd1, &five, 1);
                write(fd1, error, five);
            }
            else {
                read(fd2, &value, sizeof(int));
                memcpy((ptr + offset), &value, sizeof(int));
                write(fd1, &len, 1);
                write(fd1, buf, len);
                write(fd1, &seven, 1);
                write(fd1, success, seven);
            }

        }
        else if (strcmp(buf, "MAP_FILE") == 0) {
            file = (char*)malloc(sizeof(char)*1000);
            read(fd2, &len1, 1);
            read(fd2, file, len1);
            fd = open(file, O_RDWR);
            fileSize = lseek(fd, 0, SEEK_END);
            mappedData = mmap(ptr, fileSize, PROT_READ, MAP_SHARED, fd, 0);
            if ((int*)mappedData == (int*)-1) {
                    write(fd1, &len, 1);
                    write(fd1, buf, len);
                    write(fd1, &five, 1);
                    write(fd1, error, five);
                }

                else {
                        write(fd1, &len, 1);
                        write(fd1, buf, len);
                        write(fd1, &seven, 1);
                        write(fd1, success, seven);
                    }
                }
        else if (strcmp("READ_FROM_FILE_OFFSET", buf) == 0) {
            int offset2 = 0;
            int nob = 0;
            read(fd2, &offset2, sizeof(int));
            read(fd2, &nob, sizeof(nob));
            char* res = (char*)malloc(nob*sizeof(char));
            memcpy(res, (char*)mappedData+offset2, nob);
            memcpy(ptr, res, nob);
            if ((int*)ptr == (int*)-1 || (offset2 + nob) > fileSize || (int*)mappedData == (int*)-1) {
                write(fd1, &len, 1);
                write(fd1, buf, len);
                write(fd1, &five, 1);
                write(fd1, error, five);
            }
            else {
                write(fd1, &len, 1);
                write(fd1, buf, len);
                write(fd1, &seven, 1);
                write(fd1, success, seven);
            }
        }
        else if (strcmp(buf, "EXIT") == 0) {
            close(fd1);
            close(fd2);
            unlink("RESP_PIPE_27635");
            shmdt(ptr);
            shmctl(shmem, IPC_RMID, 0);
            munmap(mappedData, sizeof(mappedData));
            close(fd);
            exit(0);
        }
        else {
            exit(1);
        }
    }
    return 0;
}
