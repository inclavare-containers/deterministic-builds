#include <time.h>
#include <stdio.h>

int main() {
    time_t t = time(NULL);
    printf("time: %ld\n", t);
}
