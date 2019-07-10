#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>


int semid7 = 0;
int semid8 = 0;
int semid2 = 0;
int semid22 = 0;

void P(int semId, int semNr)
{
    struct sembuf op = {semNr, -1, 0};

    semop(semId, &op, 1);
}

void V(int semId, int semNr)
{
    struct sembuf op = {semNr, +1, 0};

    semop(semId, &op, 1);
}

void *thread_function7(void *thread_no) {
    int t;
    t = *(int*)thread_no;
    if (t == 5) {
        info(BEGIN, 7, t);
         V(semid7, 0);
         P(semid7, 0);
         info(END, 7, t);
         V(semid7, 0);
    }
    else {
        if (t == 1) {
            P(semid7, 0);
            info(BEGIN, 7, t);
            info(END, 7, t);
            V(semid7, 0);
        }
        else {
            info(BEGIN, 7, t);
            /*if (t == 4) {
                info(BEGIN, 7, t);
                P(semid2, 0);
                info(END, 7, t);
                V(semid22, 0);
            }*/
            info(END, 7, t);
        }
    }
    return 0;
}

void *thread_function2(void *thread_no) {
    int t;
    t = *(int*)thread_no;
    /*if (t == 4) {
        info(BEGIN, 2, t);
        info(END, 2, t);
        V(semid2, 0);
    }
    else {
        if (t == 2) {
            P(semid22, 0);
            info(BEGIN, 2, t);
            info(END, 2, t);
        }
        else {
            info(BEGIN, 2, t);
            info(END, 2 ,t);
        }
    }*/
    info(BEGIN, 2, t);
    info(END, 2 ,t);
    return 0;
}

void *thread_function8(void *thread_no) {
    int t;
    t = *(int*)thread_no;
    P(semid8, 0);
    info(BEGIN, 8, t);
    info(END, 8 ,t);
    V(semid8, 0);
    return 0;
}

int main(){
    pid_t p2,p3,p4,p5,p6,p7,p8;
    int pid2 = -1;
    int pid3 = -1;
    int pid4 = -1;
    int pid5 = -1;
    int pid6 = -1;
    int pid7 = -1;
    int pid8 = -1;
    int i = 0;
    semid2 = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if (semid2 < 0) {
        perror("Error creating the semaphore set");
        exit(2);
    }
    semctl(semid2, 0, SETVAL, 0);
    semid22 = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if (semid22 < 0) {
        perror("Error creating the semaphore set");
        exit(2);
    }
    semctl(semid22, 0, SETVAL, 0);
    init();
    info(BEGIN, 1, 0);
    p3 = fork();
    if (p3 == 0) {
        info(BEGIN, 3, 0);
        pid3 = getpid();
        info(END, 3, 0);
    }
    else {
        p2 = fork();
        if (p2 == 0) {
             info(BEGIN, 2, 0 );
             pid2 = getpid();
             pthread_t* th = (pthread_t*)malloc(5*sizeof(pthread_t));

            int one = 1, two = 2, three = 3, four = 4, five = 5;

            if (pthread_create(&th[0], NULL, thread_function2, &one) != 0) {
                printf("Error!");
                exit(1);
            }
            if (pthread_create(&th[1], NULL, thread_function2, &two) != 0) {
                printf("Error!");
                exit(1);
            }
            if (pthread_create(&th[2], NULL, thread_function2, &three) != 0) {
                printf("Error!");
                exit(1);
            }
            if (pthread_create(&th[3], NULL, thread_function2, &four) != 0) {
                printf("Error!");
                exit(1);
            }
            if (pthread_create(&th[4], NULL, thread_function2, &five) != 0) {
                printf("Error!");
                exit(1);
            }
            pthread_join(th[0], NULL);
            pthread_join(th[1], NULL);
            pthread_join(th[2], NULL);
            pthread_join(th[3], NULL);
            pthread_join(th[4], NULL);
             p5 = fork();
             if (p5 == 0) {
                info(BEGIN, 5, 0);
                pid5 = getpid();
                p6 = fork();
                if (p6 == 0) {
                    info(BEGIN, 6, 0);
                    pid6 = getpid();
                    p7 = fork();
                    if (p7 == 0) {
                        info(BEGIN, 7, 0);
                        pid7 = getpid();
                            pthread_t* th = (pthread_t*)malloc(5*sizeof(pthread_t));
                            semid7 = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
                            if (semid7 < 0) {
                                perror("Error creating the semaphore set");
                                exit(2);
                            }
                            semctl(semid7, 0, SETVAL, 0);
                            int one = 1, two = 2, three = 3, four = 4, five = 5;

                            if (pthread_create(&th[0], NULL, thread_function7, &one) != 0) {
                                printf("Error!");
                                exit(1);
                            }
                            if (pthread_create(&th[1], NULL, thread_function7, &two) != 0) {
                                printf("Error!");
                                exit(1);
                            }
                            if (pthread_create(&th[2], NULL, thread_function7, &three) != 0) {
                                printf("Error!");
                                exit(1);
                            }
                            if (pthread_create(&th[3], NULL, thread_function7, &four) != 0) {
                                printf("Error!");
                                exit(1);
                            }
                            if (pthread_create(&th[4], NULL, thread_function7, &five) != 0) {
                                printf("Error!");
                                exit(1);
                            }
                            pthread_join(th[0], NULL);
                            pthread_join(th[1], NULL);
                            pthread_join(th[2], NULL);
                            pthread_join(th[3], NULL);
                            pthread_join(th[4], NULL);
                        p8 = fork();
                        if (p8 == 0) {
                            info(BEGIN, 8, 0);
                            pid8 = getpid();
                            pthread_t* th = (pthread_t*)malloc(46*sizeof(pthread_t));
                            int a[46];
                            semid8 = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
                            if (semid8 < 0) {
                                perror("Error creating the semaphore set");
                                exit(2);
                            }
                            semctl(semid8, 0, SETVAL, 6);
                            for (i = 0; i<46; i++) {
                                 a[i] = i+1;
                                 if (pthread_create(&th[i], NULL, thread_function8, &a[i]) != 0) {
                                    printf("Error!");
                                    exit(1);
                                }
                            }
                            for (i = 0; i<46; i++) {
                                pthread_join(th[i], NULL);
                            }
                            info(END, 8, 0);
                        }
                        else {
                            waitpid(pid8, NULL, 0);

                            info(END, 7, 0);

                        }
                    }
                    else {
                        waitpid(pid7, NULL, 0);

                        info(END, 6, 0);
                    }
                }
                else {
                    waitpid(pid6, NULL, 0);
                    info(END, 5, 0);
                }
             }
             else {
                   p4 = fork();
                   if (p4 == 0) {
                        info(BEGIN, 4, 0);
                        pid4 = getpid();
                        info(END, 4, 0);
                   }
                   else {
                        waitpid(pid4, NULL, 0);
                        waitpid(pid5, NULL, 0);
                        info(END, 2, 0);
                   }
             }
        }
        else {
            waitpid(pid3, NULL, 0);
            waitpid(pid2, NULL, 0);
            info(END, 1, 0);
        }
    }
    return 0;
}

