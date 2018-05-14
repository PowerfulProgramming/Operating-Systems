/*
 *  Created by Darius Moomivand @ 15 April 2018
 *  
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

bool RUN;

pthread_mutex_t mutex;
sem_t studSem;
sem_t taSem;
int seats[3];
int waitList = 0;
int tutor = 0;
int current = 0;

typedef struct Thread {
    int studentId;
    pthread_t tid;
} Persons;

void *students(void *);
void *tutoring(void *);

int main(int argc, char *argv[]){

    if(argc != 3){
        printf("Input format: <Sleeptime> <# of Students>");
        return -1;
    }

    sem_init(&taSem, 0, 1);
    sem_init(&studSem, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    srand(time(NULL));
    RUN = true;

    Persons ta;
    ta.studentId = 0;
    int total = atoi(argv[2]);
    Persons *people = new Persons[total];

    pthread_create(&ta.tid, NULL, tutoring, (void *) &ta);
    for(int i = 0; i < total; i++){
        people[i].studentId = i;
        pthread_create(&people[i].tid, NULL, students, (void *)&people[i]);
    }

    sleep(atoi(argv[1]));
    RUN = false;

    return 0;
}

void *tutoring(void *param){
    Persons *temp = (Persons *)param;
    int id = temp->studentId;
    while(RUN){
	if(waitList > 0){
             sem_wait(&studSem);
             pthread_mutex_lock(&mutex);
  	     printf("TA is helping student %d\n", seats[tutor]);
             seats[tutor] = 0;
             waitList--;
             tutor = (tutor + 1)%3;
             sleep(1);
             printf("	TA is finished tutoring the student.\n");
             pthread_mutex_unlock(&mutex);
             sem_post(&taSem);
	}
	else
	     printf("TA goes to sleep.\n");
	     sleep(1);
    }
    pthread_exit(0);
}

void *students(void *param){
    Persons *temp = (Persons*)param;
    int id = temp->studentId;
    while(RUN){
        sleep(1);
        pthread_mutex_lock(&mutex);
        if(waitList < 3){
            seats[current] = id;
            waitList++;
            printf("          Student %d sits in an open chair.\n", id);
            current = (current + 1)%3;
            pthread_mutex_unlock(&mutex);
            sem_post(&studSem);
            sem_wait(&taSem);
        }
        else{
            pthread_mutex_unlock(&mutex);
            printf("       No chairs available, Student %d goes back to studying.\n", id);
        }
        pthread_exit(0);
    }
}
