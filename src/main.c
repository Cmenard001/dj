#include "dj/dj.h"
#include "dj_tester/dj_tester.h"

int main(int argc, char *argv[])
{
    // Initialize the DJ module and run the tests
    dj_init();
    dj_tester_launch_tests();
    return 0;
}
