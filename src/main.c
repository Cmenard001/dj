#include "dj/dj.h"
#include "dj_tester/dj_tester.h"
#include "api/printf/printf.h"

// Add this at the top to ensure C linkage if using C++
#ifdef __cplusplus
extern "C" {
#endif

int main(void) __attribute__((used));
int main(void)
{
    printfInit(); // Initialize printf functionality

    // Initialize the DJ module and run the tests
    dj_init();
    dj_tester_launch_tests();
    return 0;
}

#ifdef __cplusplus
}
#endif
