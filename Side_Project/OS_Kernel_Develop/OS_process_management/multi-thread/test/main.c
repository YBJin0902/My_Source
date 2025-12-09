#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>   // for usleep
#include <stdint.h>

// ========== 模擬工作用 ==========
static void wait_a_bit(void) {
    // 模擬每秒的 busy-wait，可改成 sleep 讓 CPU 負擔較輕
    usleep(1000 * 200);  // 200ms
}

// ========== 子執行緒函式 ==========
void* thread_func(void* arg) {
    char name = *(char*)arg;
    pthread_t tid = pthread_self();

    for (int i = 0; i < 20; ++i) {
        printf("Thread [%c] (tid=%lu): %2d\n", name, (unsigned long)tid, i);
        fflush(stdout);
        wait_a_bit();
    }

    printf("Thread [%c] finished.\n", name);
    return NULL;
}

// ========== 主程式 ==========
int main(void) {
    const int THREAD_COUNT = 4;
    const char thread_names[4] = {'A', 'B', 'C', 'D'};
    pthread_t threads[4];

    printf("=== Multi-thread demo start ===\n");

    // 建立多個執行緒
    for (int i = 0; i < THREAD_COUNT; ++i) {
        if (pthread_create(&threads[i], NULL, thread_func, (void*)&thread_names[i]) != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    // 主執行緒可同時執行其他任務（例如監控）
    for (int i = 0; i < 5; ++i) {
        printf("Main thread monitoring... (%d)\n", i);
        fflush(stdout);
        usleep(1000 * 500);
    }

    // 等待所有子執行緒結束
    for (int i = 0; i < THREAD_COUNT; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("=== All threads finished. ===\n");
    return EXIT_SUCCESS;
}
