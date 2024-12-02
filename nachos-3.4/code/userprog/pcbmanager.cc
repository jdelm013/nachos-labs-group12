#include "pcbmanager.h"

PCBManager::PCBManager(int maxProcesses) {

    bitmap = new BitMap(maxProcesses);
    pcbs = new PCB*[maxProcesses];
    pcbManagerLock = new Lock("PCB Manager Lock");

}

PCBManager::~PCBManager() {
    delete bitmap;
    delete pcbs;
    delete pcbManagerLock;
}

PCB* PCBManager::AllocatePCB() {

    pcbManagerLock->Acquire();

    int pid = bitmap->Find();

    //printf("Allocating PCB for pid %d\n", pid);

    pcbManagerLock->Release();

    ASSERT(pid >= 0);

    pcbs[pid] = new PCB(pid);
   // printf("Allocated PCB for pid %d\n", pcbs[pid]->pid());
    return pcbs[pid];
}

int PCBManager::DeallocatePCB(PCB *pcb) {
    int pid = pcb->pid();
    if (pid == -1) {
        return -1;
    }

    pcbManagerLock->Acquire();

    bitmap->Clear(pid);

    pcbManagerLock->Release();

    delete pcbs[pid];

    return 0;
}