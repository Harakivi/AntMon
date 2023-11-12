#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

bool DacInit();

bool DacChannelInit(uint8_t channel);

void DacSetValue(uint8_t channel, uint16_t value);

uint16_t DacGetValue(uint8_t channel);

#ifdef __cplusplus
}
#endif