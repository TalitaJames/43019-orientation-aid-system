#include <location.h>
#include <unity.h>


void setUp(void) {}
void tearDown(void) {}

void test_foo(void) {
    TEST_ASSERT_EQUAL(32, fooFunction(16));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_foo);
    UNITY_END();

    return 0;
}