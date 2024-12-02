#ifndef PCB_H
#define PCB_H

#include "list.h"
#include "thread.h"

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
        int exitStatus;
        bool HasExited();
        void DeleteExitedChildrenSetParentNull();

    private:
        int internal_pid;
        PCB* pcbArray[5];
    
};

#endif // PCB_H