// Name: Alexander Lauderdale
// Assignment: Project 4
// Class: CS 307 1001

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

// Struct used to obtain the data from command line arguments.
struct {
    int passengers;
    int seats;
    int iters;
} args;
// 2 binary semaphone done and seating, vacancy is for number of seats in car.
sem_t vacancy;
sem_t done;
sem_t seating;
// inPark keeps track of passengers still trying to ride.
int inPark;
// thrDone helps to hold id of thread finished and trying to join early.
int thrDone = -1;
// boarding bool value allows passengers to board car before riding.
int boarding;
// rideOver bool value lets passengers know when to exit car after riding.
int rideOver = 0;
// inSeats counter keeps track of number of passengers that boarded car.
int inSeats = 0;
// function prototypes
int cmdArgs(int argc, char** argv);
void * passThr();
void * carThr();
void boardPassengers();
void boardCar(int id, int r, int m);
char * ordinal(int n);

int main(int argc, char** argv) {
// If the command line arguments function fails, program ends.
    if (cmdArgs(argc, argv) == 0) {
        return EXIT_SUCCESS;
    }
// Initialize the semaphores
    sem_init(&vacancy, 0, args.seats);
    sem_init(&done, 0, 1);
    sem_init(&seating, 0, 1);
// Create car and passenger threads, passengers are in thread pool.
    pthread_t car;
    pthread_t * passPool;
    passPool = malloc(sizeof(pthread_t[args.passengers]));
// Create an id array to assign id numbers to threads.
    int passId[args.passengers];
// Fill array with id numbers starting from 0 to args.passengers - 1.
    for (long int i = 0; i < args.passengers; i++) {
		passId[i] = i;
	}
// All passengers enter the park, starting at inPark.
    inPark = args.passengers;
// Car thread is created.
    pthread_create(&car, NULL, &carThr, NULL);
// Passenger threads are created, while being passed their id number.
    for (int i = 0; i < args.passengers; i++) {
		pthread_create(&passPool[i], NULL, &passThr, (void *) &passId[i]);
	}
// While there are still passengers, if thread is done, join it back and inPark--.
    while (inPark > 0) {
        if (thrDone != -1) {
// Threads that finish early are joined back first.
// thrDone stops the threads from joining back simultaneously and causing race condition.
            pthread_join(passPool[thrDone], NULL);
            thrDone = -1;
            inPark--;
        }
    }
// Deallocate the memory allocated for the thread pool.
    free(passPool);
// After all the passenger threads are joined, join the car thread.
    pthread_join(car, NULL);
// Destroy the semaphores.
    sem_destroy(&vacancy);
    sem_destroy(&seating);
    sem_destroy(&done);
// Program successfully returns.
        return EXIT_SUCCESS;
}
// Function for parsing command line arguments.
int cmdArgs(int argc, char** argv) {

    char options;
    int n;
    int c;
    int i;
// If no parameters were passed, give instructions for how to run program.
    if (argc == 1) {
        printf("Usage: -n <count> -c <count> -i <count>");
        return 0;
    }
// Switch statement looks for valid options and obtains their values.
    while ((options = getopt(argc, argv, ":n:c:i:")) != -1) {
        switch (options) {
            case 'n':
                n = atol(optarg);
                if (n <= 0) {
                    printf("./roller: invalid n value - %s\n", optarg);
                    return 0;
                }
                args.passengers = n;
                break;
            case 'c':
                c = atoi(optarg);
                if (c <= 0) {
                    printf("./roller: invalid c value - %s\n", optarg);
                    return 0;
                }
                args.seats = c;
                break;
            case 'i':
                i = atoi(optarg);
                if (i <= 0) {
                    printf("./roller: invalid i value - %s\n", optarg);
                    return 0;
                }
                args.iters = i;
                break;
// If unexpected option found anywhere, error occurs and program ends.
            default:
                printf("./roller: invalid option - %c\n", optopt);
                return 0;
        }
    }
// If any argument is out of bounds of the restrictions, error also occurs.
    if ((c > n) || (n > 100) || (i > 20) || c == 0 || n == 0 || i == 0) {
        printf("n (>c ) and n (<= 100) and i (<= 20) arguments required\n");
        return 0;
    }
// If command line arguments parsing was successful, function returns 1.
    return 1;
}
// Car thread serves as the vehicle in which the passenger threads are tied to.
void * carThr() {
// rideCnt keeps track of how many times car has ran.
    int rideCnt = 1;
// As long as there are still passengers in the park, the car keeps riding.
    while (inPark > 0) {
// rideOver stops passengers from leaving the ride before it is over.
        rideOver = 0;
        boardPassengers();
// If only one passenger in car, make the sentence singular, otherwise plural.
        if (inSeats == 1) {
        	printf(" Car: 1 passenger is riding the roller coaster. Off we go on the %i ride! \n", rideCnt);      
        } else {
        	printf(" Car: %i passengers are riding the roller coaster. Off we go on the %i ride! \n", inSeats, rideCnt);      
        }
// Sleep call serves as the car "riding".
        sleep(5);
// When car ride is over, reset variables.
        printf(" Car: ride %i completed.\n", rideCnt);
        inSeats = 0;
// rideOver = 1 allows the passenger threads to exit the car.
        rideOver = 1;
// This sleep call lets the passengers have enough time to get off the ride.
        sleep(1);
        rideCnt++;

    }
// Once all passengers have left, the roller coaster can shut down.
    printf(" Car: Roller coaster shutting down.\n");
    return NULL;
}
// Function lets passengers board the car until two seconds have passed.
void boardPassengers() {
    boarding = 1;
    sleep(2);
    boarding = 0;
    return;
}
// Passenger threads try to ride the car until satisfied.
void * passThr(void * idNum) {
// *id is the threads id number, sleepNum is how long it waits before attempting ride.
    int * id = (int *) idNum;
    int sleepNum;
// Random number for how many times passenger will ride the car.
    int max_iter = rand() % (args.iters + 1);
// For loop keeps looping until passenger has finished riding max_iter times.
    for (int j = 0; j < max_iter; j++) {
    // The passenger will wait some random time 0s-10s till trying to board car.
    	sleepNum = rand() % 11;
        sleep(sleepNum);
        boardCar(*id, j, max_iter);
    }
// After done riding, the passengers exit the park.
    printf(" thread %i: Completed %i iterations on the roller coaster. Exiting.\n", *id, max_iter);
// Binary semaphore prevents race condition when passengers are leaving park / joining in main.
    sem_wait(&done);
    while (thrDone != -1) {} 
    thrDone = *id;
    sem_post(&done);
    return NULL;
}
// Function is for passengers to board the care within the conditions.
void boardCar(int id, int r, int m) {
// numEnd is so the appropriate ordinal suffix is associated in the print statement.
    char * numEnd;
// The maximum amount of passengers that can enter the semaphore is # of seats in car.
    sem_wait(&vacancy);
// Potential riders must wait until car is boarding to get on the ride.
    while (!boarding) {}
// After obtaining proper ordinal suffix, the passenger signals that it is going to ride.
    numEnd = ordinal(r+1);
    printf(" Thread %i: Wooh! I'm about to ride the roller coaster for the %i%s time! I have %i iterations left. \n", id, (r+1), numEnd, (m-r));
// Binary semaphore prevents race condition for inSeats so # of passengers riding is correctly displayed.
    sem_wait(&seating);
    inSeats++;
    sem_post(&seating);
// Passengers riding will stay on the ride until the ride is over.
    while (!rideOver) {}
// The riders leave the semaphore and will try to ride the car again.  
    sem_post(&vacancy);
    return;
}
// ordinal function gets the right ordinal ending for a number.
char * ordinal(int n) {
// Any number ending in 1 besideds 11 ends in "st", like 1st.
	if (n % 10 == 1 && n != 11) {
		return "st";
	}
// Any number ending in 2 besides 12 ends in "nd", like 2nd.
	if (n % 10 == 2 && n != 12) {
		return "nd";
	}
// Any number ending in 3 besides 13 ends in "rd", like 3rd.
	if (n % 10 == 3 && n != 13) {
		return "rd";
	}
// Every other number will end in "th", like 4th.
	return "th";
}
