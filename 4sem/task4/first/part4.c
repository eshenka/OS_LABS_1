#include <stdio.h>
#include <stdlib.h>

int main() {
    const char env[] = "OS";
    //const char value[] = "ISNOGOOD";

    //setenv(env, value, 1);

    printf("%s\n", getenv(env));

    const char new_value[] = "ISGOOD";

    setenv(env, new_value, 1);

    printf("%s\n", getenv(env));
}
