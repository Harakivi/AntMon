#include "stm32f1xx_hal.h"
#include "stm32f100xb.h"
#include "STM32Bootloader.h"

#define BOOT_ADDR	0x1FFFF000	// my MCU boot code base address
#define	MCU_IRQS	8u	// no. of NVIC IRQ inputs

struct boot_vectable_ {
    uint32_t Initial_SP;
    void (*Reset_Handler)(void);
};

#define BOOTVTAB	((struct boot_vectable_ *)BOOT_ADDR)

void EnterBootloader(void)
{
	/* Disable all interrupts */
	__disable_irq();

	/* Disable Systick timer */
	SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    HAL_DeInit();

	/* Clear Interrupt Enable Register & Interrupt Pending Register */
	for (uint8_t i = 0; i < MCU_IRQS / 32; i++)
	{
		NVIC->ICER[i]=0xFFFFFFFF;
		NVIC->ICPR[i]=0xFFFFFFFF;
    }

	/* Re-enable all interrupts */
	__enable_irq();

	// Set the MSP
	__set_MSP(BOOTVTAB->Initial_SP);

	// Jump to app firmware
	BOOTVTAB->Reset_Handler();
}