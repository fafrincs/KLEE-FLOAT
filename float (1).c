#include "klee/klee.h"
#include <assert.h>
#include <fenv.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

unsigned int snd_cwnd;      
unsigned int acked;         
unsigned int num_packets;   
double delta;

unsigned int tcp_original_linux(unsigned int snd_cwnd,  unsigned int acked, unsigned int num_packets) {
    unsigned int cwnd_cnt = 0;
    while (num_packets > 0) {
        num_packets--;
        cwnd_cnt += acked;
        while (cwnd_cnt >= snd_cwnd) {
            cwnd_cnt -= snd_cwnd;
            snd_cwnd += 1;
        }
    }

    return snd_cwnd;
}

unsigned int tcp_aggregated_model(unsigned int snd_cwnd, unsigned int acked, unsigned int num_packets) {
   
  
    delta = snd_cwnd * snd_cwnd - snd_cwnd + 1.0/4.0 + 2 * num_packets * acked;
    snd_cwnd = sqrt(delta) + 1.0/2.0;

    return snd_cwnd;
}

int testcase1(){
    for(snd_cwnd=1; snd_cwnd<100; snd_cwnd++)
        for(acked=1; acked<snd_cwnd; acked++)
            for (num_packets=1; num_packets<100; num_packets++){
                unsigned int result = tcp_aggregated_model(snd_cwnd, acked, num_packets);
                if (result < snd_cwnd) 
                  return 1;
            }

    return 0;
}

int testcase2(){
  	   
    for(snd_cwnd=1; snd_cwnd<100; snd_cwnd++)
        for(acked=1; acked<snd_cwnd; acked++){
            
                num_packets=1; 
                unsigned int result1 = tcp_original_linux(snd_cwnd, acked, num_packets);
                unsigned int result2 = tcp_aggregated_model(snd_cwnd, acked, num_packets);
                if (result1!=result2)
                  return 1;
                for (num_packets=2; num_packets<100; num_packets++){
                    unsigned int first_n_1 = tcp_aggregated_model(snd_cwnd, acked, num_packets-1);

                    unsigned int result1 = tcp_original_linux(first_n_1, acked, 1);
                    unsigned int result2 = tcp_aggregated_model(first_n_1, acked, 1);              
                    if (result1!=result2)
                    return 1;
                }
            }
    return 0;
}

int main() {
    klee_make_symbolic(&delta, sizeof(delta), "delta");
    klee_make_symbolic(&snd_cwnd, sizeof(snd_cwnd), "snd_cwnd");
    klee_assume((1<=snd_cwnd));
    klee_assume((snd_cwnd<=1000));
  	klee_make_symbolic(&acked, sizeof(acked), "acked");
    klee_assume((1<=acked));
    klee_assume((acked<=snd_cwnd));
  	klee_make_symbolic(&num_packets, sizeof(num_packets), "num_packets");
    klee_assume((1<=num_packets));
    klee_assume((num_packets<=1000000));
    	
	
  
    if (testcase1())
    {
      printf("Failed");
    }
    else
    {
      printf("Passed");
    }
   
    if (testcase2())
    {
      printf("Failed");
    }
      
    else
    {
      printf("Passed");
    }
 
  return 0;
}

