/*
 * pizzaproblem.c :: Program that simulates students programming 
 * while being fueled by pizza and having the option to code
 * and eat pizza or goof off.
 *
 */

#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <semaphore.h>  /* Semaphore */

/* function prototypes */
void student(void *ptr);
void pizzaplace();
void eatandcode(int,int);
void goofoff(int);
sem_t semaphore;
sem_t mutex;

int slices = 0; // number of pizza slices aavailable.
int ordered = 0; // indicates whether a pizza has been ordered.

/*
 * main :: Function that initializes 4 threads for each student
 * and one thread for a pizzaplace. Function also initializes
 * two semaphores.
 */
int main()
{
    pthread_t thread_s1;
    pthread_t thread_s2;
    pthread_t thread_s3;
    pthread_t thread_s4;
    pthread_t thread_p;
    int threadargs[4] = {0,1,2,3};

    sem_init(&mutex, 0, 1);
    sem_init(&semaphore, 0, 0);

    pthread_create(&thread_s1,NULL,(void *) &student, (void *) &threadargs[0]);
    pthread_create(&thread_s2,NULL,(void *) &student, (void *) &threadargs[1]);
    pthread_create(&thread_s3,NULL,(void *) &student, (void *) &threadargs[2]);
    pthread_create(&thread_s4,NULL,(void *) &student, (void *) &threadargs[3]);
    pthread_create(&thread_p,NULL,(void *) &pizzaplace, (void *) NULL);

    pthread_join(thread_s1, NULL);
    pthread_join(thread_s2, NULL);
    pthread_join(thread_s3, NULL);
    pthread_join(thread_s4, NULL);

    sem_destroy(&semaphore);
    sem_destroy(&mutex);
    exit(0);
}

/*
 * eatandcode :: Function that displays that a student ate pizza.
 */
void eatandcode(int student,int slice)
{
    printf("student%d ate slice %d\n",student,slice);
}

/*
 * goofoff :: Function that displays that a student is goofing off.
 */
void goofoff(int student)
{
    printf("student%d is goofing off\n",student);
    sleep(1);
}

/*
 * student :: Function in which the student infinitely looks to see if a
 * slice of pizza is available. If so, the student function takes the
 * pizza slice and codes. If not, the first student that determines there is no pizza,
 * orders a new one. If there is no pizza available, the student goofs off.
 */
void student(void *ptr)
{
    while(1)
    {
        int id = *((int *) ptr);
        sem_wait(&mutex); // up
        if( slices > 0 ){
            slices--;
            sem_post(&mutex); // down
            eatandcode(id,slices+1);
        }
        else{
            if(ordered == 0)
            {
                ordered++;
                printf("student%d is ordering a pizza.\n",id);
                sem_post(&semaphore);
            }
            sem_post(&mutex);
            goofoff(id);
        }
    }
}

/*
 * pizzaplace :: Function that infinitely waits to be woken up when a pizza
 * order is made. After being awoken, it updates the number of slices available
 * for eating and goes back to sleep.
 */
void pizzaplace(){
    while(1)
    {
        sem_wait(&semaphore);
        sem_wait(&mutex);
        printf("pizza place received an order for 8 slices.\n");
        slices += 8; // 8 slices per pizza.
        ordered = 0; // reset the order.
        sem_post(&mutex);
    }
}
