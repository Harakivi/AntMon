#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_CMD 10
#define UINT8 (void *)1
#define UINT16 (void *)2
#define UINT32 (void *)3

    typedef struct
    {
        const char *cmdString;
        void *returns;
        void (*functor)();
        void *takes;
    } cmd_t;

    extern cmd_t cmd_list[];

    void addCmd(cmd_t cmd);

    bool parseCmd(char *string);

#ifdef __cplusplus
}
#endif