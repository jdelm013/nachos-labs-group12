// Alternate two threads, one that prints "ping" and another that
// prints "pong".

#include "copyright.h"
#include "system.h"
#include "synch.h"

Lock * pingLock;
Condition *cv;
bool pingTurn = true;

void
PingPong(int which)
{
    char *msg = (char *) which;
    int i;

    pingLock->Acquire();
    for (i = 0; i < 5; i++) {
		printf("%s\n", msg);
		cv->Signal(pingLock);
		cv->Wait(pingLock);
    }
    pingLock->Release();
}

// void PingPong(int which)
// {
//     char *msg = (char *) which;
//     for (int i = 0; i < 5; i++) {
//         pingLock->Acquire();
//         if (strcmp(msg, "ping") == 0) {
//             while (!pingTurn) {
//                 cv->Wait(pingLock);
//             }
//             printf("%s\n", msg);
//             pingTurn = false;
//         } else {
//             while (pingTurn) {
//                 cv->Wait(pingLock);
//             }
//             printf("%s\n", msg);
//             pingTurn = true;
//         }
//         cv->Signal(pingLock);
//         pingLock->Release();
//     }
// }

void Ping()
{
    pingLock = new Lock("cv");
    cv = new Condition("ping pong");

    char *ping = "ping";
    char *pong = "pong";

    Thread *t = new Thread(ping);
    t->Fork(PingPong, (int) ping);

    PingPong((int) pong);

 }