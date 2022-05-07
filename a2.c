#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "a2_helper.h"

pthread_cond_t cond_p3 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_p3 = PTHREAD_MUTEX_INITIALIZER;
int p3t15_arrived = 0;
sem_t semaphore;
pthread_barrier_t barrier; 
void* slave_p3(void* arg) {
    int t_id = *(int*) arg;

    // pthread_mutex_lock(&lock_p3);
    if (t_id != 15) {
        if (p3t15_arrived == 0) {
            // pthread_barrier_wait(&barrier);
            pthread_mutex_lock(&lock_p3);
            pthread_cond_wait(&cond_p3, &lock_p3);
            pthread_mutex_unlock(&lock_p3);    
        }
        
        sem_wait(&semaphore);
        info(BEGIN, 3, t_id);
    }
    else {
        
        sem_wait(&semaphore);
        p3t15_arrived = 1;
        info(BEGIN, 3, t_id);
        pthread_mutex_lock(&lock_p3);
        pthread_cond_broadcast(&cond_p3);
        pthread_mutex_unlock(&lock_p3);
    }
    
    info(END, 3, t_id);
    sem_post(&semaphore);
    // pthread_mutex_unlock(&lock_p3);

    return NULL;
}

void run_p3() {
    sem_init(&semaphore, 0, 4);
    pthread_barrier_init(&barrier, NULL, 4);
    pthread_t threads[44];
    int* ids = (int*) malloc(44 * sizeof(int));
    for (int i = 0; i < 44; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, slave_p3, (void*) &ids[i]);
    }
    for (int i = 0; i < 44; i++) {
        pthread_join(threads[i], NULL);
    }
    free(ids);
}

void run_p2() {
    if (fork() == 0) {
        info(BEGIN, 3, 0);
        run_p3();
        info(END, 3, 0);
        exit(0);
    }
    if (fork() == 0) {
        info(BEGIN, 6, 0);
        // run_p3();
        info(END, 6, 0);
        exit(0);
    }
    if (fork() == 0) {
        info(BEGIN, 7, 0);
        // run_p3();
        info(END, 7, 0);
        exit(0);
    }
    wait(0);
    wait(0);
    wait(0);
}

pthread_cond_t cond_p4 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_p4 = PTHREAD_MUTEX_INITIALIZER;
int p4t2_started = 0;
int p4t4_ended = 0;
void solve_t2_t4_problem(int t_id) {
    // Make sure t2 starts before t4
    pthread_mutex_lock(&lock_p4);
    if (t_id == 4) {
        if (p4t2_started == 0) {
            pthread_cond_wait(&cond_p4, &lock_p4);
        }
        info(BEGIN, 4, t_id);
    }
    else {
        p4t2_started = 1;
        pthread_cond_signal(&cond_p4);
        info(BEGIN, 4, t_id);
    }
    pthread_mutex_unlock(&lock_p4);

    // Make sure t4 ends before t2
    pthread_mutex_lock(&lock_p4);
    if (t_id == 2) {
        if (p4t4_ended == 0) {
            pthread_cond_wait(&cond_p4, &lock_p4);
        }
        info(END, 4, t_id);
    }
    else {
        p4t4_ended = 1;
        pthread_cond_signal(&cond_p4);
        info(END, 4, t_id);
    }
    pthread_mutex_unlock(&lock_p4);
}

void* slave_p4(void* arg) {
    int t_id = *(int*) arg;
    if (t_id == 2 || t_id == 4) {
        solve_t2_t4_problem(t_id);
        return NULL;
    }
    info(BEGIN, 4, t_id);
    info(END, 4, t_id);
    return NULL;
}

void run_p4() {
    if (fork() == 0) {
        info(BEGIN, 5, 0);
        info(END, 5, 0);
        exit(0);
    }
    wait(0);

    pthread_t threads[4];
    int* ids = (int*) malloc(4 * sizeof(int));
    for (int i = 0; i < 4; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, slave_p4, (void*) &ids[i]);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    free(ids);
}

int main(){
    init();

    info(BEGIN, 1, 0);

    if (fork() == 0) {
        info(BEGIN, 2, 0);
        run_p2();
        info(END, 2, 0);
        exit(0);
    }
    if (fork() == 0) {
        info(BEGIN, 4, 0);
        run_p4();
        info(END, 4, 0);
        exit(0);
    }
    wait(0);
    wait(0);
    
    info(END, 1, 0);
    return 0;
}
