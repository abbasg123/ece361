#include <stdint.h>
#include <stdio.h>

#include "iom361_r4.h"

int main(void)
{
    int rc;
    uint32_t *base = iom361_initialize(2, 3, &rc);

    if (base == NULL || rc != 0) {
        fprintf(stderr, "iom361_initialize failed: %d\n", rc);
        return 1;
    }

    puts("Automatic door");
    return 0;
}
