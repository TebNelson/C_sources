#include <stdlib.h>
#include <stdio.h>

int main() {
    // Run the first set of commands
    int result1 = system("apt-get update -y && apt-get upgrade -y 2>/dev/null");

    // Check the result of the first set of commands
    if (result1 == 0) {
        printf("First set of commands executed successfully.\n");
        printf("\n");
    } else {
        printf("Error executing first set of commands.\n");
    }

    // Run the second set of commands
    int result2 = system("apt-get autoclean -y && apt-get -y autoremove && apt-get autopurge -y 2>/dev/null");

    // Check the result of the second set of commands
    if (result2 == 0) {
        printf("Second set of commands executed successfully.\n");
        printf("\n");
    } else {
        printf("Error executing second set of commands.\n");
    }

    return 0;
}
