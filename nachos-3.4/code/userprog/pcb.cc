#include "pcb.h"

PCB::PCB(int pid) {
    this->internal_pid = pid;
    this->parent = NULL;
    this->children = new List();
    this->thread = NULL;
}

PCB::~PCB() {
  //  if (children != NULL) {
   //     delete children;
  //  }
    if (this->thread != NULL) {
        delete this->thread;
    }       
}

int PCB::pid() {
    return this->internal_pid;
}   

void PCB::AddChild(PCB *pcb) {
 
    pcbArray[pcb->pid()] = pcb;
    this->children->Append((void *)pcb);
}   

int PCB::RemoveChild(PCB *pcb) {
    return this->children->RemoveItem(pcb);
}
