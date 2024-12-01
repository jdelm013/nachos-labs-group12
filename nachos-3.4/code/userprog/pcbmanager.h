#ifndef PCBMANAGER_H
#define PCBMANAGER_H

#include "pcb.h"
#include "bitmap.h"
#include "synch.h"

class PCB;
class Lock;

class PCBManager {
    public:
        PCBManager(int maxProcesses);
        ~PCBManager();

        PCB* AllocatePCB();
        int DeallocatePCB(PCB *pcb);

    private:
        BitMap *bitmap;
        PCB** pcbs; // List of PCBs
        Lock* pcbManagerLock;

};

#endif // PCBMANAGER_H