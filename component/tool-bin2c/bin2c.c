#include <stdio.h>

int main() {
    int c;
    printf("\"");
    while((c = fgetc(stdin)) != EOF) {
        printf("\\x%X", (unsigned)c);
    }
    /* printf("'\\0'"); // put terminating zero */
    printf("\""); // put terminating zero
}

