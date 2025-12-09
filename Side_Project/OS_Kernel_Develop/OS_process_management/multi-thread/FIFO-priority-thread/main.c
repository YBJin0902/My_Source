#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void* worker(void* arg) {
    const char* name = (const char*)arg;

    struct sched_param sp;
    int policy;
    pthread_getschedparam(pthread_self(), &policy, &sp);

    for (int i = 0; i < 20; ++i) {
        printf("[%s] i=%02d (policy=%s, prio=%d)\n",
               name, i,
               policy == SCHED_FIFO ? "FIFO" :
               policy == SCHED_RR   ? "RR"   :
               policy == SCHED_OTHER? "OTHER": "??",
               sp.sched_priority);
        // 模擬一些工作，再讓出 CPU
        usleep(50 * 1000);
    }
    return NULL;
}

static void set_attr_rt(pthread_attr_t* attr, int policy, int prio) {
    pthread_attr_init(attr);
    pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED); // 用我們設定的
    pthread_attr_setschedpolicy(attr, policy);

    struct sched_param sp = {0};
    sp.sched_priority = prio;
    if (pthread_attr_setschedparam(attr, &sp) != 0) {
        perror("pthread_attr_setschedparam");
        exit(1);
    }
}

int main(void) {
    pthread_t th_hi, th_lo;
    pthread_attr_t attr_hi, attr_lo;

    int pmin = sched_get_priority_min(SCHED_FIFO);
    int pmax = sched_get_priority_max(SCHED_FIFO);
    // 取兩個合適的優先權
    int hi = pmax - 0;           // 最高
    int lo = pmin + (pmax-pmin)/4; // 比較低

    set_attr_rt(&attr_hi, SCHED_FIFO, hi);
    set_attr_rt(&attr_lo, SCHED_FIFO, lo);

    if (pthread_create(&th_hi, &attr_hi, worker, "HIGH") != 0) {
        perror("pthread_create hi");
        return 1;
    }
    if (pthread_create(&th_lo, &attr_lo, worker, "LOW") != 0) {
        perror("pthread_create lo");
        return 1;
    }

    pthread_join(th_hi, NULL);
    pthread_join(th_lo, NULL);

    return 0;
}
