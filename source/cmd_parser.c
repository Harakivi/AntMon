#include "cmd_parser.h"
#include <stdint.h>

cmd_t cmd_list[10] = {0};
uint8_t cmd_cnt = 0;

uint32_t parseNumber(const char* string)
{
    uint32_t res = 0;
    while(*string)
    {
        if(*string >= 0x30)
        {
            res *= 10;
            res += *string - 0x30;
        }
        string++;
    }
    return res;
}

void addCmd(cmd_t cmd)
{
    cmd_list[cmd_cnt++] = cmd;
}

bool parseCmd(char* string)
{
    for(int i = 0; i < cmd_cnt; i++)
    {
        uint8_t pos = 0;
        while(cmd_list[i].cmdString[pos] == *(string + pos))
        {
            if(cmd_list[i].takes != 0 && *(string + pos + 1) == '=')
            {
                uint32_t number = parseNumber(string + pos + 2);
                if(cmd_list[i].takes == UINT8)
                {
                    ((void(*)(uint8_t))cmd_list[i].functor)((uint8_t)number);
                }
                else if (cmd_list[i].takes == UINT16)
                {
                    ((void(*)(uint16_t))cmd_list[i].functor)((uint16_t)number);
                }
                else if (cmd_list[i].takes == UINT32)
                {
                    ((void (*)(uint32_t))cmd_list[i].functor)((uint32_t)number);
                }
                return true;
            }
            else if(cmd_list[i].cmdString[pos] == 0 && *(string + pos) == 0)
            {
                ((void(*)(void))cmd_list[i].functor)();
                return true;
            }
            else if(*(string + pos) == 0)
            {
                break;
            }
            pos++;
        }
    }
    return false;
}