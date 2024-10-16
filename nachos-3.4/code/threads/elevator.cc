// Implementation of the elevator simulation
#include "elevator.h"
#include "system.h"
#include <cstdio>
// #include <thread>       // std::this_thread::sleep_for
// caused exception on ocelot, not locally

ELEVATOR *e;
int nextPersonID = 1;
Lock *personIDLock = new Lock("PersonIDLock");

ELEVATOR::ELEVATOR(int numFloors) {
    this->numFloors = numFloors;
    this->currentFloor = 0;
    this->maxOccupancy = 5;
    this->occupancy = 0;
    this->personsWaiting = 0;

    this->elevatorLock = new Lock("ElevatorLock");
    this->hailed = new Condition("Hailed");

    this->entering = new Condition*[numFloors];
    this->leaving = new Condition*[numFloors];
    this->waitingPersons = new int[numFloors];

    for (int i = 0; i < numFloors; ++i) {
        this->entering[i] = new Condition("Entering Condition");
        this->leaving[i] = new Condition("Leaving Condition");
        this->waitingPersons[i] = 0;
    }
}

ELEVATOR::~ELEVATOR() {
    delete this->elevatorLock;
    delete this->hailed;

    for (int i = 0; i < this->numFloors; ++i) {
        delete this->entering[i];
        delete this->leaving[i];
    }

    delete[] this->entering;
    delete[] this->leaving;
    delete[] this->waitingPersons;
}

// Function to start the elevator
void ELEVATOR::start() {
    while (true) {
        this->elevatorLock->Acquire();
        while (this->personsWaiting == 0) {
            this->hailed->Wait(this->elevatorLock);
        }

        // elevator goes up
        for (int i = 0; i < this->numFloors; ++i) {
            this->currentFloor = i;
            printf("Elevator arrives on floor %d\n", this->currentFloor + 1);

            while (this->waitingPersons[this->currentFloor] > 0 && this->occupancy < this->maxOccupancy) {
                this->entering[this->currentFloor]->Signal(this->elevatorLock);
                this->waitingPersons[this->currentFloor]--;
                this->personsWaiting--;
                //std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate person entering
            }

            this->leaving[this->currentFloor]->Broadcast(this->elevatorLock);
            //std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate delay
        }

        // elevator goes down   
        for (int i = this->numFloors - 1; i >= 0; --i) {
            this->currentFloor = i;
            printf("Elevator arrives on floor %d\n", this->currentFloor + 1);

            this->leaving[this->currentFloor]->Broadcast(this->elevatorLock);
            //std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate delay
        }

        this->elevatorLock->Release();
    }
}

// Function to hail the elevator
void ELEVATOR::Hail(Person *p) {
    this->elevatorLock->Acquire();

    this->waitingPersons[p->atFloor - 1]++;
    this->personsWaiting++;

    printf("Person %d wants to go to floor %d from floor %d\n", p->id, p->toFloor, p->atFloor);

    this->hailed->Signal(this->elevatorLock);

    while (this->currentFloor != p->atFloor - 1 || this->occupancy >= this->maxOccupancy) {
        this->entering[p->atFloor - 1]->Wait(this->elevatorLock);
    }

    printf("Person %d got into the elevator\n", p->id);
    this->occupancy++;

    while (this->currentFloor != p->toFloor - 1) {
        this->leaving[this->currentFloor]->Wait(this->elevatorLock);
    }

    printf("Person %d got out of the elevator\n", p->id);
    this->occupancy--;

    this->elevatorLock->Release();
}

// Function to create a new elevator
void Elevator(int numFloors) {
    Thread *t = new Thread("ElevatorThread");
    t->Fork((VoidFunctionPtr)ElevatorThread, numFloors);
}

// Function to create a new elevator
void ElevatorThread(int numFloors) {
    e = new ELEVATOR(numFloors);
    e->start();
}

// Function to create a new person and hail the elevator
void ArrivingGoingFromTo(int atFloor, int toFloor) {
    personIDLock->Acquire();
    int personID = nextPersonID++;
    personIDLock->Release();

    Person *p = new Person{personID, atFloor, toFloor};

    Thread *t = new Thread("PersonThread");
    t->Fork((VoidFunctionPtr)PersonThread, (int)p);
}

// Function to simulate a person hailing the elevator
void PersonThread(int person) {
    Person *p = (Person *)person;
    e->Hail(p);
}