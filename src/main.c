#include "api/printf/printf.h"
#include "api/time/time.h"
#include "dj/dj.h"
#include "dj_tester/dj_tester.h"

int main(void) __attribute__((used));
int main(void)
{
    printfInit(); // Initialize printf functionality
    time_init();  // Initialize time management

    // Initialize the DJ module and run the tests
    dj_init();
    dj_tester_launch_tests();
    return 0;
}
