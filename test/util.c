
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e4/util.h"

int hextest() {
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
        printf("HEXTEST: Test %s failed.\n", "Test1");


        returncode = 1;
        goto exit;
    }
/* fallthrough ok*/
exit:
    return returncode;
}

int zerochecktest() {
    
    size_t check_inner = 0;
    size_t check_outer = 0;
    int returncode = 0;
    unsigned char testbuffer[6] = {0xA0, 0x00, 0x00, 0x00, 0x00, 0xA0};
    
    check_inner = zerocheck(&testbuffer[1], 4);
    if ( check_inner != 0 ) {
        printf("ZEROCHECK: overflow or underflow in zerocheck.\n");
        returncode = 1; goto exit;
    }

    check_outer = zerocheck(&testbuffer[0], 6);
    if ( check_outer == 0 ) {
        printf("ZEROCHECK: non-zero buffer reported as zero.\n");
        returncode = 1; goto exit;
    }

/* fallthrough ok*/
exit:
    return returncode;
}

int main(int argc, char** argv, char** envp) {

    int returncode = 0;
    returncode += hextest();
    returncode += zerochecktest();
    if ( returncode == 0 ) {
        printf("Util tests: OK\n");
    }
    else {
        printf("Util tests: FAILED\n");
    }
    return returncode;
}
