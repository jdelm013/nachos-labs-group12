#ifndef PCB_H
#define PCB_H

#include "list.h"

class Thread;

class PCB {
    public:
        PCB(int pid);
        ~PCB();

        int pid();
        PCB *parent;
        List *children;
        Thread* thread;
        void AddChild(PCB *pcb);
        int RemoveChild(PCB *pcb);

    private:
        int internal_pid;

};

#endif // PCB_H