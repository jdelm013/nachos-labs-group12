#include "pcb.h"

PCB::PCB(int pid) {
    this->internal_pid = pid;
    parent = NULL;
    children = new List();
    thread = NULL;
}

PCB::~PCB() {
    delete thread;
}

int PCB::pid() {
    return this->internal_pid;
}   

void PCB::AddChild(PCB *pcb) {
    children->Append(pcb);
}   

int PCB::RemoveChild(PCB *pcb) {
    return children->RemoveItem(pcb);
}
