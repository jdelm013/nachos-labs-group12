#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "synch.h"

struct Person {
    int id;
    int atFloor;
    int toFloor;
};

class ELEVATOR {
public:
    ELEVATOR(int numFloors);
    ~ELEVATOR();
    void start();
    void Hail(Person *p);

private:
    int numFloors;
    int currentFloor;
    int maxOccupancy;
    int occupancy;
    int personsWaiting;
    int *waitingPersons;

    Lock *elevatorLock;
    Condition **entering;
    Condition **leaving;
    Condition *hailed;
};

void Elevator(int numFloors);
void ArrivingGoingFromTo(int atFloor, int toFloor);
void ElevatorThread(int numFloors);
void PersonThread(int person);

#endif // ELEVATOR_H