#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "thread.h"
#include "membership.h"
#include "pthread.h"

// class MyThread : public Thread
// {
//   public:
//     void *run() {
//         for (int i = 0; i < 5; i++) {
//             printf("thread %lu running - %d\n",  (long unsigned int)self(), i+1);
//             sleep(2);
//         }
//         printf("thread done %lu\n", (long unsigned int)self());
//         return NULL;
//     }
// };

int main(int argc, char** argv)
{

  //   ofstream myfile;
  // myfile.open ("example.txt");
  // myfile << "Writing this to a file.\n";
  // myfile.close();
  //   cout<<"end"<<endl;


    membership* mb=new membership();
    mb->start();
    delete mb;
    cout<<"finished!!!"<<endl;
    // MyThread* thread1 = new MyThread();
    // MyThread* thread2 = new MyThread();
    // thread1->start();
    // thread2->start();
    // thread1->join();
    // thread2->join();
    // printf("main done\n");
    // exit(0);
}