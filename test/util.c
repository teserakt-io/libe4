
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e4/util.h"

int main(int argc, char** argv, char** envp) {
    int returncode = 0;
    char test1_input[] = "DEADBEEF";
    unsigned char test1_bytes_kat[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    unsigned char test1_bytes_output[4] = {0};
    int r = 0;

    r = e4c_hex_decode((char*)test1_bytes_output, sizeof(test1_bytes_output), test1_input, strlen(test1_input));
    if (r == 0) {
        returncode = 1;
        goto exit;
    }

    if (memcmp(test1_bytes_output, test1_bytes_kat, sizeof(test1_bytes_kat)) != 0) {
        printf("Test %s failed.", "Test1");


        returncode = 1;
        goto exit;
    }
exit:
    return returncode;
}
