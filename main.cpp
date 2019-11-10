#include <stdio.h>
#include <sys/time.h>
#include <assert.h>
#include "types.h"

int main (int argc, const char * argv[]) {
    Timer timer;
    
    timer.start();
    timer.stop();
    printf("%d fadd %d %s\n", iter, n, timer().c_str());
    return 0;
}