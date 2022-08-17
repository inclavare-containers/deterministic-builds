#include <stdio.h>

int main() {
    /* Standard Predefined Macros */
    printf("File: %s\n", __FILE__);
    printf("Line: %d\n", __LINE__);
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("STDC: %d\n", __STDC__);
    printf("STDC_VERSION: %ld\n", __STDC_VERSION__);
    // printf("c++: %s\n", __cplusplus);
    // printf("ObjC: %s\n", __OBJC__);
    // printf("Assembler: %s\n", __ASSEMBLER__);

    /* Common Predefined Macros */
    printf("Base File: %s\n", __BASE_FILE__);
    // printf("File Name: %s\n", __FILE_NAME__);
    printf("Include Level: %d\n", __INCLUDE_LEVEL__);
    printf("Timestamp: %s\n", __TIMESTAMP__);
    printf("GNUC: %d %d %d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
}
