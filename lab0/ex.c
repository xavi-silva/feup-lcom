#include <stdio.h>
int set_bit(int input, int pos) {
    int x = 1 << pos;
    input = input | x;
    return input;
}

int unset_bit(int input, int pos) {
    int x = ~(1 << pos);
    input = input & x;
    return input;
}



int main(int argc, char *argv[]) {
    int result = set_bit(1000, 1);
    int result2 = unset_bit(11000, 3);
    printf("%d\n", result);
    printf("\n");
    printf("%d\n", result2);
    return 0;
}
