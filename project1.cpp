/*
 *  Created by Darius Moomivand @ 11 April 2018
 *  This program takes user input to create producer and
 *  consumer pthreads that act on a queue.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

class Queue {
private:
    int front;
    int rear;
    int *arr;
    int size;
public:
    Queue(int max) {
	size = max;
        front = -1;
        rear = -1;
        arr = new int[size];
    };

    void insert_item(int rand) {
        if (front == -1) {
            front = 0;	
	    rear = 0;
            arr[rear] = rand;
        } 
	else if (rear == size - 1 && front != 0) {
            rear = 0;
            arr[rear] = rand;
        } 
	else {
            rear++;
            arr[rear] = rand;
        }
    };
    int remove_item() {
        int temp = arr[front];
        arr[front] = -1;
        if (front == rear) {
            front = -1;
            rear = -1;
        } 
	else if (front == size - 1) {
            front = 0;
        }
	 else
            front++;
        return temp;
    };
    bool is_full(){
        if((front == 0 && rear == size-1) || (rear == size-1)){
            return true;
        }
	 else
            return false;
    };
    bool is_empty(){
        if(front == -1)
            return true;
        else
            return false;
    };
    ~Queue(){
        delete arr;
    }
};
Queue queue(11);

typedef struct prod_thread{
     pthread_t tid;
     pthread_attr_t attribute;
     int producerId;
     int itemCount;
} Produce;

#define producerPointer Produce*
typedef struct consum_thread{
     pthread_t tid;
     pthread_attr_t attribute;
     int consumerId;
     int itemCount;
} Consume;
#define consumerPointer Consume*

bool RUN;
sem_t sem[3];

void *producerFunc(void *);
void *consumerFunc(void *);

int main(int argc, char **argv){
    RUN = true;
    srand(time(NULL));

    if(argc != 4){
        fprintf(stderr, "Please enter: <sleep>, <producers>, <consumers>\n");
        return -1;
    }

    producerPointer producers = new Produce[atoi(argv[2])];
    consumerPointer consumers = new Consume[atoi(argv[3])];

    sem_init(&sem[0],0,1);
    sem_init(&sem[1],0,10);
    sem_init(&sem[2],0,0);

    printf("Creating threads.\n");
    
    for(int i = 0; i < atoi(argv[2]); i++){
        producers[i].producerId = i + 1;
	pthread_attr_init(&producers[i].attribute);
	producers[i].itemCount = 0;
        pthread_create(&producers[i].tid, &producers[i].attribute, producerFunc, (void *)&producers[i]);
    }

    for(int i = 0; i < atoi(argv[3]); i++){
        consumers[i].consumerId = i + 1;
        pthread_attr_init(&consumers[i].attribute);
	consumers[i].itemCount = 0;
	pthread_create(&consumers[i].tid, &consumers[i].attribute, consumerFunc,(void *)&consumers[i]);
    }

    sleep(atoi(argv[0]));
    RUN = false;
    delete producers;
    delete consumers;
    sem_destroy(&sem[0]);
    sem_destroy(&sem[1]);
    sem_destroy(&sem[2]);
    return 0;
}

void *producerFunc(void *param){
    producerPointer ptemp = (producerPointer)param;
    while(RUN) {
        if(!queue.is_full()){
            sem_wait(&sem[1]);
            sem_wait(&sem[0]);
            queue.insert_item(1000);
            sem_post(&sem[0]);
            sem_post(&sem[2]);
            printf("Producer %d produced %d\n", ptemp->producerId, ptemp->itemCount);
	    ptemp->itemCount++;
        }
    }
    pthread_exit(0);
}

void *consumerFunc(void *param){
    consumerPointer ctemp = (consumerPointer)param;
    while (RUN) {
        if(!queue.is_empty()){
            sem_wait(&sem[2]);
            sem_wait(&sem[0]);
            queue.remove_item();
            printf("     Consumer %d consumed %d\n", ctemp->consumerId, ctemp->itemCount);
	    ctemp->itemCount++;
            sem_post(&sem[0]);
            sem_post(&sem[1]);

        }
    }
    pthread_exit(0);
}

