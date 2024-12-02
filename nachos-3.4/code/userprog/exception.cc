// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "system.h"
#include "addrspace.h"
#include "pcbmanager.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void doExit(int status, int pid) {
    
    printf("Process [%d] exits with [%d]\n", pid, status);
    delete currentThread->space; 
    currentThread->Finish();

}


void childFunction(int pid) {

    // 1. Restore the state of registers
    currentThread->RestoreUserState();

    // 2. Restore the page table for child
    currentThread->space->RestoreState();

    // print message for child creation (pid,  pcreg, currentThread->space->GetNumPages())
    printf("Process [%d] Fork: start at address [%p] and [%d] pages memory\n", pid, machine->ReadRegister(PCReg), currentThread->space->GetNumPages());

    machine->Run();

}


// ---------------------------------------------------------------------
// doFork
// 	Entry point into the Nachos kernel.  Called when a user program
//  requests to fork a new process.
// ----------------------------------------------------------------------
int doFork(int funcAddr) {

    // 1. Check if sufficient memory exists to create new process
    // currentThread->space->GetNumPages() <= mm->GetFreePageCount()
    // if check fails, return -1
    if (currentThread->space->GetNumPages() > mm->GetFreePageCount()) {
        return -1;
    }

    // 2. SaveUserState for the parent thread
    currentThread->space->SaveState();

    // 3. Create a new address space for child by copying parent address space
    AddrSpace* childAddrSpace = new AddrSpace(currentThread->space);

    // 4. Create a new thread for the child and set its addrSpace
    Thread *childThread = new Thread("childThread");
    childThread->space = childAddrSpace;

    // 5. Create a PCB and associate the new Address and new Thread with PCB
    PCB* childPCB = pcbManager->AllocatePCB();
    childAddrSpace->pcb = childPCB;
    childPCB->thread = childThread;
    childPCB->parent = currentThread->space->pcb;

    //currentThread->space->pcb->AddChild(childPCB);
    childAddrSpace->pcb = childPCB;

    // 6. Copy old register values to new register. Set pc reg value to value in r4. save new
    //    register values to new AddrSpace.
    machine->WriteRegister(PCReg, funcAddr);
    machine->WriteRegister(PrevPCReg, funcAddr - 4);
    machine->WriteRegister(NextPCReg, funcAddr + 4);
    childThread->SaveUserState();

    // 7. Use Thread::Fork(func, arg) to set new thread behavior: restore registers, restore
    //    memory and put machine to run.
    childThread->Fork(childFunction, childPCB->pid());

    // 8. Restore register state of parent user-level process
    currentThread->RestoreUserState();
    

    return childPCB->pid();

}

void incrementPC() {
    int oldPCReg = machine->ReadRegister(PCReg);

    machine->WriteRegister(PrevPCReg, oldPCReg);
    machine->WriteRegister(PCReg, oldPCReg + 4);
    machine->WriteRegister(NextPCReg, oldPCReg + 8);
}


void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    int pid = currentThread->GetPid();
    int status = 0;

    const char *exceptionMsg = "System Call: [%d] invoked %s.\n";

    if (which == SyscallException) {
        switch (type) {
            case SC_Halt:
                printf(exceptionMsg, pid, "Halt");
                DEBUG('a', "Shutdown, initiated by user program.\n");
                interrupt->Halt();
                break;

            case SC_Exit:
                printf(exceptionMsg, pid, "Exit");
                DEBUG('a', "Exit, initiated by user program.\n");
                status = machine->ReadRegister(4);
                doExit(status, pid);
                break;

            case SC_Fork: { // must be enclosed in a block bc of variable declarations
                printf(exceptionMsg, pid, "Fork");
                DEBUG('a', "Fork, initiated by user program.\n");
                int funcAddr = machine->ReadRegister(4);
                int result = doFork(funcAddr);
                
                // 9. Write new process pid to r2
                machine->WriteRegister(2, result);
                
                // 10. Update counter of old process and return
                incrementPC();
                break;
            }

            default:
                printf("Unexpected user mode exception %d %d\n", which, type);
                ASSERT(FALSE);
        }
    }
}
