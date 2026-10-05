#include "bits.h"
#include "status.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int total = 0, failed = 0;

static void check(const char *name, int correct)
{
    printf("%s: %s\n", correct ? "PASS" : "FAIL", name);
    total++;
    failed += !correct;
}

static void checkPrint(const char *name, uint32_t value, int width, const char *expected)
{
    char output[64] = {0};
    int fd[2];
    if (pipe(fd) < 0) exit(1);
    int saved = dup(STDOUT_FILENO);
    fflush(stdout);
    if (saved < 0 || dup2(fd[1], STDOUT_FILENO) < 0) exit(1);
    printBinary(value, width);
    fflush(stdout);
    if (dup2(saved, STDOUT_FILENO) < 0) exit(1);
    close(saved);
    close(fd[1]);
    ssize_t length = read(fd[0], output, sizeof output - 1);
    close(fd[0]);
    check(name, length == (ssize_t)strlen(expected) && strcmp(output, expected) == 0);
}

int main(void)
{
    checkPrint("print width 1", 1, 1, "1");
    checkPrint("print width 32", UINT32_MAX, 32,
                "1111 1111 1111 1111 1111 1111 1111 1111");

    check("get width 1", getField(3, 0, 1) == 1);
    check("get width 32", getField(0x12345678, 0, 32) == 0x12345678);
    check("get position 31", getField(0x80000000, 31, 1) == 1);

    check("set width 1", setField(UINT32_MAX, 0, 1, 0) == 0xFFFFFFFE);
    check("set width 32", setField(0, 0, 32, 0x12345678) == 0x12345678);
    check("set position 31", setField(0, 31, 1, 1) == 0x80000000);
    check("set value too wide", setField(0xD6, 2, 3, 0xB) == 0xCE);

    check("sign width 1 negative", signExtend(1, 1) == -1);
    check("sign width 32 minimum", signExtend(0x80000000, 32) == INT32_MIN);

    status_t s = status_unpack(0x1631);
    check("status 0x1631", s.heat && !s.cool && !s.fan && !s.fault && !s.res
          && s.mode == 3 && s.modeValidity && s.setpnt == 22);
    s = status_unpack(0x8000);
    check("status 0x8000", !s.heat && !s.cool && !s.fan && !s.fault && !s.res
          && s.mode == 0 && s.modeValidity && s.setpnt == -128);
    s = status_unpack(0xFFFF);
    check("status 0xFFFF", s.heat && s.cool && s.fan && s.fault && s.res
          && s.mode == 7 && !s.modeValidity && s.setpnt == -1);

    printf("%d passed, %d failed\n", total - failed, failed);
    return failed != 0;
}
