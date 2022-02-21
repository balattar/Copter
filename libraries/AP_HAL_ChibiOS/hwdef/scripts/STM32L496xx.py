#!/usr/bin/env python
'''
these tables are generated from the STM32 datasheet stm32l496ze.pdf
'''

# additional build information for ChibiOS
build = {
    "CHIBIOS_STARTUP_MK"  : "os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32l4xx.mk",
    "CHIBIOS_PLATFORM_MK" : "os/hal/ports/STM32/STM32L4xx/platform.mk"
    }

pincount = {
    'A': 16,
    'B': 16,
    'C': 16,
    'D': 16,
    'E': 16,
    'F': 16,
    'G': 16,
    'H': 16,
    'I': 0,
    'J': 0,
    'K': 0
}


# MCU parameters
mcu = {
    # ram map, as list of (address, size-kb, flags)
    # flags of 1 means DMA-capable
    # flags of 2 means faster memory for CPU intensive work
    'RAM_MAP' : [
        # we treat SRAM1/SRAM2 as a single region for simplicifty. SRAM2 is parity checked
        # and is mapped at both 0x20040000 and at 0x10000000
        (0x20000000, 320, 1), # SRAM1/SRAM2
    ],

    'EXPECTED_CLOCK' : 80000000,

    'DEFINES' : {
        'STM32L4' : '1',
    }
}

DMA_Map = {
	# format is (DMA_TABLE, StreamNum, Channel)
	# extracted from tabula-STM32L496-DMA.csv
	"ADC1"    	:	[(1,1,0),(2,3,0)],
	"ADC2"    	:	[(1,2,0),(2,4,0)],
	"ADC3"    	:	[(1,3,0),(2,5,0)],
	"AES_IN"  	:	[(2,1,6),(2,5,6)],
	"AES_OUT" 	:	[(2,2,6),(2,3,6)],
	"DAC_CH1" 	:	[(1,3,6),(2,4,3)],
	"DAC_CH2" 	:	[(1,4,5),(2,5,3)],
	"DCMI"    	:	[(2,6,0),(2,5,4)],
	"DFSDM1_FLT0"	:	[(1,4,0)],
	"DFSDM1_FLT1"	:	[(1,5,0)],
	"DFSDM1_FLT2"	:	[(1,6,0)],
	"DFSDM1_FLT3"	:	[(1,7,0)],
	"HASH_IN" 	:	[(2,7,6)],
	"I2C1_RX" 	:	[(1,7,3),(2,6,5)],
	"I2C1_TX" 	:	[(1,6,3),(2,7,5)],
	"I2C2_RX" 	:	[(1,5,3)],
	"I2C2_TX" 	:	[(1,4,3)],
	"I2C3_RX" 	:	[(1,3,3)],
	"I2C3_TX" 	:	[(1,2,3)],
	"I2C4_RX" 	:	[(2,1,0)],
	"I2C4_TX" 	:	[(2,2,0)],
	"LPUART1_RX"	:	[(2,7,4)],
	"LPUART1_TX"	:	[(2,6,4)],
	"QUADSPI" 	:	[(1,5,5),(2,7,3)],
	"SAI1_A"  	:	[(2,1,1),(2,6,1)],
	"SAI1_B"  	:	[(2,2,1),(2,7,1)],
	"SAI2_A"  	:	[(1,6,1),(2,3,1)],
	"SAI2_B"  	:	[(1,7,1),(2,4,1)],
	"SDMMC1"  	:	[(2,4,7),(2,5,7)],
	"SPI1_RX" 	:	[(1,2,1),(2,3,4)],
	"SPI1_TX" 	:	[(1,3,1),(2,4,4)],
	"SPI2_RX" 	:	[(1,4,1)],
	"SPI2_TX" 	:	[(1,5,1)],
	"SPI3_RX" 	:	[(2,1,3)],
	"SPI3_TX" 	:	[(2,2,3)],
	"SWPMI1_RX"	:	[(2,1,4)],
	"SWPMI1_TX"	:	[(2,2,4)],
	"TIM15_CH1"	:	[(1,5,7)],
	"TIM15_COM"	:	[(1,5,7)],
	"TIM15_TRIG"	:	[(1,5,7)],
	"TIM15_UP"	:	[(1,5,7)],
	"TIM16_CH1"	:	[(1,3,4),(1,6,4)],
	"TIM16_UP"	:	[(1,3,4),(1,6,4)],
	"TIM17_CH1"	:	[(1,1,5),(1,7,5)],
	"TIM17_UP"	:	[(1,1,5),(1,7,5)],
	"TIM1_CH1"	:	[(1,2,7)],
	"TIM1_CH2"	:	[(1,3,7)],
	"TIM1_CH3"	:	[(1,7,7)],
	"TIM1_CH4"	:	[(1,4,7)],
	"TIM1_COM"	:	[(1,4,7)],
	"TIM1_TRIG"	:	[(1,4,7)],
	"TIM1_UP" 	:	[(1,6,7)],
	"TIM2_CH1"	:	[(1,5,4)],
	"TIM2_CH2"	:	[(1,7,4)],
	"TIM2_CH3"	:	[(1,1,4)],
	"TIM2_CH4"	:	[(1,7,4)],
	"TIM2_UP" 	:	[(1,2,4)],
	"TIM3_CH1"	:	[(1,6,5)],
	"TIM3_CH3"	:	[(1,2,5)],
	"TIM3_CH4"	:	[(1,3,5)],
	"TIM3_TRIG"	:	[(1,6,5)],
	"TIM3_UP" 	:	[(1,3,5)],
	"TIM4_CH1"	:	[(1,1,6)],
	"TIM4_CH2"	:	[(1,4,6)],
	"TIM4_CH3"	:	[(1,5,6)],
	"TIM4_UP" 	:	[(1,7,6)],
	"TIM5_CH1"	:	[(2,5,5)],
	"TIM5_CH2"	:	[(2,4,5)],
	"TIM5_CH3"	:	[(2,2,5)],
	"TIM5_CH4"	:	[(2,1,5)],
	"TIM5_TRIG"	:	[(2,1,5)],
	"TIM5_UP" 	:	[(2,2,5)],
	"TIM6_UP" 	:	[(1,3,6),(2,4,3)],
	"TIM7_UP" 	:	[(1,4,5),(2,5,3)],
	"TIM8_CH1"	:	[(2,6,7)],
	"TIM8_CH2"	:	[(2,7,7)],
	"TIM8_CH3"	:	[(2,1,7)],
	"TIM8_CH4"	:	[(2,2,7)],
	"TIM8_COM"	:	[(2,2,7)],
	"TIM8_TRIG"	:	[(2,2,7)],
	"TIM8_UP" 	:	[(2,1,7)],
	"UART4_RX"	:	[(2,5,2)],
	"UART4_TX"	:	[(2,3,2)],
	"UART5_RX"	:	[(2,2,2)],
	"UART5_TX"	:	[(2,1,2)],
	"USART1_RX"	:	[(1,5,2),(2,7,2)],
	"USART1_TX"	:	[(1,4,2),(2,6,2)],
	"USART2_RX"	:	[(1,6,2)],
	"USART2_TX"	:	[(1,7,2)],
	"USART3_RX"	:	[(1,3,2)],
	"USART3_TX"	:	[(1,2,2)],
}

AltFunction_map = {
	# format is PIN:FUNCTION : AFNUM
	# extracted from L496.txt
	"PA0:EVENTOUT"      	:	15,
	"PA0:SAI1_EXTCLK"   	:	13,
	"PA0:TIM2_CH1"      	:	1,
	"PA0:TIM2_ETR"      	:	14,
	"PA0:TIM5_CH1"      	:	2,
	"PA0:TIM8_ETR"      	:	3,
	"PA0:UART4_TX"      	:	8,
	"PA0:USART2_CTS"    	:	7,
	"PA1:EVENTOUT"      	:	15,
	"PA1:I2C1_SMBA"     	:	4,
	"PA1:LCD_SEG0"      	:	11,
	"PA1:SPI1_SCK"      	:	5,
	"PA1:TIM15_CH1N"    	:	14,
	"PA1:TIM2_CH2"      	:	1,
	"PA1:TIM5_CH2"      	:	2,
	"PA1:UART4_RX"      	:	8,
	"PA1:USART2_DE"     	:	7,
	"PA1:USART2_RTS"    	:	7,
	"PA2:EVENTOUT"      	:	15,
	"PA2:LCD_SEG1"      	:	11,
	"PA2:LPUART1_TX"    	:	8,
	"PA2:QUADSPI_BK1_NCS"	:	10,
	"PA2:SAI2_EXTCLK"   	:	13,
	"PA2:TIM15_CH1"     	:	14,
	"PA2:TIM2_CH3"      	:	1,
	"PA2:TIM5_CH3"      	:	2,
	"PA2:USART2_TX"     	:	7,
	"PA3:EVENTOUT"      	:	15,
	"PA3:LCD_SEG2"      	:	11,
	"PA3:LPUART1_RX"    	:	8,
	"PA3:QUADSPI_CLK"   	:	10,
	"PA3:SAI1_MCLK_A"   	:	13,
	"PA3:TIM15_CH2"     	:	14,
	"PA3:TIM2_CH4"      	:	1,
	"PA3:TIM5_CH4"      	:	2,
	"PA3:USART2_RX"     	:	7,
	"PA4:DCMI_HSYNC"    	:	10,
	"PA4:EVENTOUT"      	:	15,
	"PA4:LPTIM2_OUT"    	:	14,
	"PA4:SAI1_FS_B"     	:	13,
	"PA4:SPI1_NSS"      	:	5,
	"PA4:SPI3_NSS"      	:	6,
	"PA4:USART2_CK"     	:	7,
	"PA5:EVENTOUT"      	:	15,
	"PA5:LPTIM2_ETR"    	:	14,
	"PA5:SPI1_SCK"      	:	5,
	"PA5:TIM2_CH1"      	:	1,
	"PA5:TIM2_ETR"      	:	2,
	"PA5:TIM8_CH1N"     	:	3,
	"PA6:DCMI_PIXCLK"   	:	4,
	"PA6:EVENTOUT"      	:	15,
	"PA6:LCD_SEG3"      	:	11,
	"PA6:LPUART1_CTS"   	:	8,
	"PA6:QUADSPI_BK1_IO3"	:	10,
	"PA6:SPI1_MISO"     	:	5,
	"PA6:TIM16_CH1"     	:	14,
	"PA6:TIM1_BKIN"     	:	1,
	"PA6:TIM1_BKIN_COMP2"	:	12,
	"PA6:TIM3_CH1"      	:	2,
	"PA6:TIM8_BKIN"     	:	3,
	"PA6:TIM8_BKIN_COMP2"	:	13,
	"PA6:USART3_CTS"    	:	7,
	"PA7:EVENTOUT"      	:	15,
	"PA7:I2C3_SCL"      	:	4,
	"PA7:LCD_SEG4"      	:	11,
	"PA7:QUADSPI_BK1_IO2"	:	10,
	"PA7:SPI1_MOSI"     	:	5,
	"PA7:TIM17_CH1"     	:	14,
	"PA7:TIM1_CH1N"     	:	1,
	"PA7:TIM3_CH2"      	:	2,
	"PA7:TIM8_CH1N"     	:	3,
	"PA8:EVENTOUT"      	:	15,
	"PA8:LCD_COM0"      	:	11,
	"PA8:LPTIM2_OUT"    	:	14,
	"PA8:RCC_MCO"       	:	0,
	"PA8:SAI1_SCK_A"    	:	13,
	"PA8:SWPMI1_IO"     	:	12,
	"PA8:TIM1_CH1"      	:	1,
	"PA8:USART1_CK"     	:	7,
	"PA8:USB_OTG_FS_SOF"	:	10,
	"PA9:DCMI_D0"       	:	5,
	"PA9:EVENTOUT"      	:	15,
	"PA9:LCD_COM1"      	:	11,
	"PA9:SAI1_FS_A"     	:	13,
	"PA9:SPI2_SCK"      	:	3,
	"PA9:TIM15_BKIN"    	:	14,
	"PA9:TIM1_CH2"      	:	1,
	"PA9:USART1_TX"     	:	7,
	"PA10:DCMI_D1"      	:	5,
	"PA10:EVENTOUT"     	:	15,
	"PA10:LCD_COM2"     	:	11,
	"PA10:SAI1_SD_A"    	:	13,
	"PA10:TIM17_BKIN"   	:	14,
	"PA10:TIM1_CH3"     	:	1,
	"PA10:USART1_RX"    	:	7,
	"PA10:USB_OTG_FS_ID"	:	10,
	"PA11:CAN1_RX"      	:	9,
	"PA11:EVENTOUT"     	:	15,
	"PA11:SPI1_MISO"    	:	5,
	"PA11:TIM1_BKIN2"   	:	2,
	"PA11:TIM1_BKIN2_COMP1"	:	12,
	"PA11:TIM1_CH4"     	:	1,
	"PA11:USART1_CTS"   	:	7,
    "PA11:OTG_FS_DM"	:	10,
	"PA12:CAN1_TX"      	:	9,
	"PA12:EVENTOUT"     	:	15,
	"PA12:SPI1_MOSI"    	:	5,
	"PA12:TIM1_ETR"     	:	1,
	"PA12:USART1_DE"    	:	7,
	"PA12:USART1_RTS"   	:	7,
    "PA12:OTG_FS_DP"	:	10,
	"PA13:EVENTOUT"     	:	15,
	"PA13:IR_OUT"       	:	1,
	"PA13:SAI1_SD_B"    	:	13,
	"PA13:SWPMI1_TX"    	:	12,
    "PA13:JTMS-SWDIO"	:	0,
	"PA13:USB_OTG_FS_NOE"	:	10,
	"PA14:EVENTOUT"     	:	15,
	"PA14:I2C1_SMBA"    	:	4,
	"PA14:I2C4_SMBA"    	:	5,
	"PA14:LPTIM1_OUT"   	:	1,
	"PA14:SAI1_FS_B"    	:	13,
	"PA14:SWPMI1_RX"    	:	12,
    "PA14:JTCK-SWCLK"	:	0,
	"PA14:USB_OTG_FS_SOF"	:	10,
	"PA15:EVENTOUT"     	:	15,
	"PA15:LCD_SEG17"    	:	11,
	"PA15:SAI2_FS_B"    	:	13,
	"PA15:SPI1_NSS"     	:	5,
	"PA15:SPI3_NSS"     	:	6,
	"PA15:SWPMI1_SUSPEND"	:	12,
	"PA15:SYS_JTDI"     	:	0,
	"PA15:TIM2_CH1"     	:	1,
	"PA15:TIM2_ETR"     	:	2,
	"PA15:TSC_G3_IO1"   	:	9,
	"PA15:UART4_DE"     	:	8,
	"PA15:UART4_RTS"    	:	8,
	"PA15:USART2_RX"    	:	3,
	"PA15:USART3_DE"    	:	7,
	"PA15:USART3_RTS"   	:	7,
	"PB0:COMP1_OUT"     	:	12,
	"PB0:EVENTOUT"      	:	15,
	"PB0:LCD_SEG5"      	:	11,
	"PB0:QUADSPI_BK1_IO1"	:	10,
	"PB0:SAI1_EXTCLK"   	:	13,
	"PB0:SPI1_NSS"      	:	5,
	"PB0:TIM1_CH2N"     	:	1,
	"PB0:TIM3_CH3"      	:	2,
	"PB0:TIM8_CH2N"     	:	3,
	"PB0:USART3_CK"     	:	7,
	"PB1:DFSDM1_DATIN0" 	:	6,
	"PB1:EVENTOUT"      	:	15,
	"PB1:LCD_SEG6"      	:	11,
	"PB1:LPTIM2_IN1"    	:	14,
	"PB1:LPUART1_DE"    	:	8,
	"PB1:LPUART1_RTS"   	:	8,
	"PB1:QUADSPI_BK1_IO0"	:	10,
	"PB1:TIM1_CH3N"     	:	1,
	"PB1:TIM3_CH4"      	:	2,
	"PB1:TIM8_CH3N"     	:	3,
	"PB1:USART3_DE"     	:	7,
	"PB1:USART3_RTS"    	:	7,
	"PB2:DFSDM1_CKIN0"  	:	6,
	"PB2:EVENTOUT"      	:	15,
	"PB2:I2C3_SMBA"     	:	4,
	"PB2:LCD_VLCD"      	:	11,
	"PB2:LPTIM1_OUT"    	:	1,
	"PB2:RTC_OUT_ALARM" 	:	0,
	"PB2:RTC_OUT_CALIB" 	:	0,
	"PB3:CRS_SYNC"      	:	10,
	"PB3:EVENTOUT"      	:	15,
	"PB3:LCD_SEG7"      	:	11,
	"PB3:SAI1_SCK_B"    	:	13,
	"PB3:SPI1_SCK"      	:	5,
	"PB3:SPI3_SCK"      	:	6,
	"PB3:SYS_JTDO-SWO"  	:	0,
	"PB3:TIM2_CH2"      	:	1,
	"PB3:USART1_DE"     	:	7,
	"PB3:USART1_RTS"    	:	7,
	"PB4:DCMI_D12"      	:	10,
	"PB4:EVENTOUT"      	:	15,
	"PB4:I2C3_SDA"      	:	4,
	"PB4:LCD_SEG8"      	:	11,
	"PB4:SAI1_MCLK_B"   	:	13,
	"PB4:SPI1_MISO"     	:	5,
	"PB4:SPI3_MISO"     	:	6,
	"PB4:SYS_JTRST"     	:	0,
	"PB4:TIM17_BKIN"    	:	14,
	"PB4:TIM3_CH1"      	:	2,
	"PB4:TSC_G2_IO1"    	:	9,
	"PB4:UART5_DE"      	:	8,
	"PB4:UART5_RTS"     	:	8,
	"PB4:USART1_CTS"    	:	7,
	"PB5:CAN2_RX"       	:	3,
	"PB5:COMP2_OUT"     	:	12,
	"PB5:DCMI_D10"      	:	10,
	"PB5:EVENTOUT"      	:	15,
	"PB5:I2C1_SMBA"     	:	4,
	"PB5:LCD_SEG9"      	:	11,
	"PB5:LPTIM1_IN1"    	:	1,
	"PB5:SAI1_SD_B"     	:	13,
	"PB5:SPI1_MOSI"     	:	5,
	"PB5:SPI3_MOSI"     	:	6,
	"PB5:TIM16_BKIN"    	:	14,
	"PB5:TIM3_CH2"      	:	2,
	"PB5:TSC_G2_IO2"    	:	9,
	"PB5:UART5_CTS"     	:	8,
	"PB5:USART1_CK"     	:	7,
	"PB6:CAN2_TX"       	:	8,
	"PB6:DCMI_D5"       	:	10,
	"PB6:DFSDM1_DATIN5" 	:	6,
	"PB6:EVENTOUT"      	:	15,
	"PB6:I2C1_SCL"      	:	4,
	"PB6:I2C4_SCL"      	:	5,
	"PB6:LPTIM1_ETR"    	:	1,
	"PB6:SAI1_FS_B"     	:	13,
	"PB6:TIM16_CH1N"    	:	14,
	"PB6:TIM4_CH1"      	:	2,
	"PB6:TIM8_BKIN2"    	:	3,
	"PB6:TIM8_BKIN2_COMP2"	:	12,
	"PB6:TSC_G2_IO3"    	:	9,
	"PB6:USART1_TX"     	:	7,
	"PB7:DCMI_VSYNC"    	:	10,
	"PB7:DFSDM1_CKIN5"  	:	6,
	"PB7:EVENTOUT"      	:	15,
	"PB7:FMC_NL"        	:	12,
	"PB7:I2C1_SDA"      	:	4,
	"PB7:I2C4_SDA"      	:	5,
	"PB7:LCD_SEG21"     	:	11,
	"PB7:LPTIM1_IN2"    	:	1,
	"PB7:TIM17_CH1N"    	:	14,
	"PB7:TIM4_CH2"      	:	2,
	"PB7:TIM8_BKIN"     	:	3,
	"PB7:TIM8_BKIN_COMP1"	:	13,
	"PB7:TSC_G2_IO4"    	:	9,
	"PB7:UART4_CTS"     	:	8,
	"PB7:USART1_RX"     	:	7,
	"PB8:CAN1_RX"       	:	9,
	"PB8:DCMI_D6"       	:	10,
	"PB8:DFSDM1_DATIN6" 	:	6,
	"PB8:EVENTOUT"      	:	15,
	"PB8:I2C1_SCL"      	:	4,
	"PB8:LCD_SEG16"     	:	11,
	"PB8:SAI1_MCLK_A"   	:	13,
	"PB8:SDMMC1_D4"     	:	12,
	"PB8:TIM16_CH1"     	:	14,
	"PB8:TIM4_CH3"      	:	2,
	"PB9:CAN1_TX"       	:	9,
	"PB9:DCMI_D7"       	:	10,
	"PB9:DFSDM1_CKIN6"  	:	6,
	"PB9:EVENTOUT"      	:	15,
	"PB9:I2C1_SDA"      	:	4,
	"PB9:IR_OUT"        	:	1,
	"PB9:LCD_COM3"      	:	11,
	"PB9:SAI1_FS_A"     	:	13,
	"PB9:SDMMC1_D5"     	:	12,
	"PB9:SPI2_NSS"      	:	5,
	"PB9:TIM17_CH1"     	:	14,
	"PB9:TIM4_CH4"      	:	2,
	"PB10:COMP1_OUT"    	:	12,
	"PB10:DFSDM1_DATIN7"	:	6,
	"PB10:EVENTOUT"     	:	15,
	"PB10:I2C2_SCL"     	:	4,
	"PB10:I2C4_SCL"     	:	3,
	"PB10:LCD_SEG10"    	:	11,
	"PB10:LPUART1_RX"   	:	8,
	"PB10:QUADSPI_CLK"  	:	10,
	"PB10:SAI1_SCK_A"   	:	13,
	"PB10:SPI2_SCK"     	:	5,
	"PB10:TIM2_CH3"     	:	1,
	"PB10:TSC_SYNC"     	:	9,
	"PB10:USART3_TX"    	:	7,
	"PB11:COMP2_OUT"    	:	12,
	"PB11:DFSDM1_CKIN7" 	:	6,
	"PB11:EVENTOUT"     	:	15,
	"PB11:I2C2_SDA"     	:	4,
	"PB11:I2C4_SDA"     	:	3,
	"PB11:LCD_SEG11"    	:	11,
	"PB11:LPUART1_TX"   	:	8,
	"PB11:QUADSPI_BK1_NCS"	:	10,
	"PB11:TIM2_CH4"     	:	1,
	"PB11:USART3_RX"    	:	7,
	"PB12:CAN2_RX"      	:	10,
	"PB12:DFSDM1_DATIN1"	:	6,
	"PB12:EVENTOUT"     	:	15,
	"PB12:I2C2_SMBA"    	:	4,
	"PB12:LCD_SEG12"    	:	11,
	"PB12:LPUART1_DE"   	:	8,
	"PB12:LPUART1_RTS"  	:	8,
	"PB12:SAI2_FS_A"    	:	13,
	"PB12:SPI2_NSS"     	:	5,
	"PB12:SWPMI1_IO"    	:	12,
	"PB12:TIM15_BKIN"   	:	14,
	"PB12:TIM1_BKIN"    	:	1,
	"PB12:TIM1_BKIN_COMP2"	:	3,
	"PB12:TSC_G1_IO1"   	:	9,
	"PB12:USART3_CK"    	:	7,
	"PB13:CAN2_TX"      	:	10,
	"PB13:DFSDM1_CKIN1" 	:	6,
	"PB13:EVENTOUT"     	:	15,
	"PB13:I2C2_SCL"     	:	4,
	"PB13:LCD_SEG13"    	:	11,
	"PB13:LPUART1_CTS"  	:	8,
	"PB13:SAI2_SCK_A"   	:	13,
	"PB13:SPI2_SCK"     	:	5,
	"PB13:SWPMI1_TX"    	:	12,
	"PB13:TIM15_CH1N"   	:	14,
	"PB13:TIM1_CH1N"    	:	1,
	"PB13:TSC_G1_IO2"   	:	9,
	"PB13:USART3_CTS"   	:	7,
	"PB14:DFSDM1_DATIN2"	:	6,
	"PB14:EVENTOUT"     	:	15,
	"PB14:I2C2_SDA"     	:	4,
	"PB14:LCD_SEG14"    	:	11,
	"PB14:SAI2_MCLK_A"  	:	13,
	"PB14:SPI2_MISO"    	:	5,
	"PB14:SWPMI1_RX"    	:	12,
	"PB14:TIM15_CH1"    	:	14,
	"PB14:TIM1_CH2N"    	:	1,
	"PB14:TIM8_CH2N"    	:	3,
	"PB14:TSC_G1_IO3"   	:	9,
	"PB14:USART3_DE"    	:	7,
	"PB14:USART3_RTS"   	:	7,
	"PB15:DFSDM1_CKIN2" 	:	6,
	"PB15:EVENTOUT"     	:	15,
	"PB15:LCD_SEG15"    	:	11,
	"PB15:RTC_REFIN"    	:	0,
	"PB15:SAI2_SD_A"    	:	13,
	"PB15:SPI2_MOSI"    	:	5,
	"PB15:SWPMI1_SUSPEND"	:	12,
	"PB15:TIM15_CH2"    	:	14,
	"PB15:TIM1_CH3N"    	:	1,
	"PB15:TIM8_CH3N"    	:	3,
	"PB15:TSC_G1_IO4"   	:	9,
	"PC0:DFSDM1_DATIN4" 	:	6,
	"PC0:EVENTOUT"      	:	15,
	"PC0:I2C3_SCL"      	:	4,
	"PC0:I2C4_SCL"      	:	2,
	"PC0:LCD_SEG18"     	:	11,
	"PC0:LPTIM1_IN1"    	:	1,
	"PC0:LPTIM2_IN1"    	:	14,
	"PC0:LPUART1_RX"    	:	8,
	"PC1:DFSDM1_CKIN4"  	:	6,
	"PC1:EVENTOUT"      	:	15,
	"PC1:I2C3_SDA"      	:	4,
	"PC1:I2C4_SDA"      	:	2,
	"PC1:LCD_SEG19"     	:	11,
	"PC1:LPTIM1_OUT"    	:	1,
	"PC1:LPUART1_TX"    	:	8,
	"PC1:QUADSPI_BK2_IO0"	:	10,
	"PC1:SAI1_SD_A"     	:	13,
	"PC1:SPI2_MOSI"     	:	3,
	"PC1:SYS_TRACED0"   	:	0,
	"PC2:DFSDM1_CKOUT"  	:	6,
	"PC2:EVENTOUT"      	:	15,
	"PC2:LCD_SEG20"     	:	11,
	"PC2:LPTIM1_IN2"    	:	1,
	"PC2:QUADSPI_BK2_IO1"	:	10,
	"PC2:SPI2_MISO"     	:	5,
	"PC3:EVENTOUT"      	:	15,
	"PC3:LCD_VLCD"      	:	11,
	"PC3:LPTIM1_ETR"    	:	1,
	"PC3:LPTIM2_ETR"    	:	14,
	"PC3:QUADSPI_BK2_IO2"	:	10,
	"PC3:SAI1_SD_A"     	:	13,
	"PC3:SPI2_MOSI"     	:	5,
	"PC4:EVENTOUT"      	:	15,
	"PC4:LCD_SEG22"     	:	11,
	"PC4:QUADSPI_BK2_IO3"	:	10,
	"PC4:USART3_TX"     	:	7,
	"PC5:EVENTOUT"      	:	15,
	"PC5:LCD_SEG23"     	:	11,
	"PC5:USART3_RX"     	:	7,
	"PC6:DCMI_D0"       	:	10,
	"PC6:DFSDM1_CKIN3"  	:	6,
	"PC6:EVENTOUT"      	:	15,
	"PC6:LCD_SEG24"     	:	11,
	"PC6:SAI2_MCLK_A"   	:	13,
	"PC6:SDMMC1_D6"     	:	12,
	"PC6:TIM3_CH1"      	:	2,
	"PC6:TIM8_CH1"      	:	3,
	"PC6:TSC_G4_IO1"    	:	9,
	"PC7:DCMI_D1"       	:	10,
	"PC7:DFSDM1_DATIN3" 	:	6,
	"PC7:EVENTOUT"      	:	15,
	"PC7:LCD_SEG25"     	:	11,
	"PC7:SAI2_MCLK_B"   	:	13,
	"PC7:SDMMC1_D7"     	:	12,
	"PC7:TIM3_CH2"      	:	2,
	"PC7:TIM8_CH2"      	:	3,
	"PC7:TSC_G4_IO2"    	:	9,
	"PC8:DCMI_D2"       	:	10,
	"PC8:EVENTOUT"      	:	15,
	"PC8:LCD_SEG26"     	:	11,
	"PC8:SDMMC1_D0"     	:	12,
	"PC8:TIM3_CH3"      	:	2,
	"PC8:TIM8_CH3"      	:	3,
	"PC8:TSC_G4_IO3"    	:	9,
	"PC9:DCMI_D3"       	:	4,
	"PC9:EVENTOUT"      	:	15,
	"PC9:I2C3_SDA"      	:	6,
	"PC9:LCD_SEG27"     	:	11,
	"PC9:SAI2_EXTCLK"   	:	13,
	"PC9:SDMMC1_D1"     	:	12,
	"PC9:TIM3_CH4"      	:	2,
	"PC9:TIM8_BKIN2"    	:	1,
	"PC9:TIM8_BKIN2_COMP1"	:	14,
	"PC9:TIM8_CH4"      	:	3,
	"PC9:TSC_G4_IO4"    	:	9,
	"PC9:USB_OTG_FS_NOE"	:	10,
	"PC10:DCMI_D8"      	:	10,
	"PC10:EVENTOUT"     	:	15,
	"PC10:LCD_COM4"     	:	11,
	"PC10:LCD_SEG28"    	:	11,
	"PC10:LCD_SEG40"    	:	11,
	"PC10:SAI2_SCK_B"   	:	13,
	"PC10:SDMMC1_D2"    	:	12,
	"PC10:SPI3_SCK"     	:	6,
	"PC10:SYS_TRACED1"  	:	0,
	"PC10:TSC_G3_IO2"   	:	9,
	"PC10:UART4_TX"     	:	8,
	"PC10:USART3_TX"    	:	7,
	"PC11:DCMI_D4"      	:	10,
	"PC11:EVENTOUT"     	:	15,
	"PC11:LCD_COM5"     	:	11,
	"PC11:LCD_SEG29"    	:	11,
	"PC11:LCD_SEG41"    	:	11,
	"PC11:QUADSPI_BK2_NCS"	:	5,
	"PC11:SAI2_MCLK_B"  	:	13,
	"PC11:SDMMC1_D3"    	:	12,
	"PC11:SPI3_MISO"    	:	6,
	"PC11:TSC_G3_IO3"   	:	9,
	"PC11:UART4_RX"     	:	8,
	"PC11:USART3_RX"    	:	7,
	"PC12:DCMI_D9"      	:	10,
	"PC12:EVENTOUT"     	:	15,
	"PC12:LCD_COM6"     	:	11,
	"PC12:LCD_SEG30"    	:	11,
	"PC12:LCD_SEG42"    	:	11,
	"PC12:SAI2_SD_B"    	:	13,
	"PC12:SDMMC1_CK"    	:	12,
	"PC12:SPI3_MOSI"    	:	6,
	"PC12:SYS_TRACED3"  	:	0,
	"PC12:TSC_G3_IO4"   	:	9,
	"PC12:UART5_TX"     	:	8,
	"PC12:USART3_CK"    	:	7,
	"PC13:EVENTOUT"     	:	15,
	"PC14:EVENTOUT"     	:	15,
	"PC15:EVENTOUT"     	:	15,
	"PD0:CAN1_RX"       	:	9,
	"PD0:DFSDM1_DATIN7" 	:	6,
	"PD0:EVENTOUT"      	:	15,
	"PD0:FMC_D2"        	:	12,
	"PD0:FMC_DA2"       	:	12,
	"PD0:SPI2_NSS"      	:	5,
	"PD1:CAN1_TX"       	:	9,
	"PD1:DFSDM1_CKIN7"  	:	6,
	"PD1:EVENTOUT"      	:	15,
	"PD1:FMC_D3"        	:	12,
	"PD1:FMC_DA3"       	:	12,
	"PD1:SPI2_SCK"      	:	5,
	"PD2:DCMI_D11"      	:	10,
	"PD2:EVENTOUT"      	:	15,
	"PD2:LCD_COM7"      	:	11,
	"PD2:LCD_SEG31"     	:	11,
	"PD2:LCD_SEG43"     	:	11,
	"PD2:SDMMC1_CMD"    	:	12,
	"PD2:SYS_TRACED2"   	:	0,
	"PD2:TIM3_ETR"      	:	2,
	"PD2:TSC_SYNC"      	:	9,
	"PD2:UART5_RX"      	:	8,
	"PD2:USART3_DE"     	:	7,
	"PD2:USART3_RTS"    	:	7,
	"PD3:DCMI_D5"       	:	4,
	"PD3:DFSDM1_DATIN0" 	:	6,
	"PD3:EVENTOUT"      	:	15,
	"PD3:FMC_CLK"       	:	12,
	"PD3:QUADSPI_BK2_NCS"	:	10,
	"PD3:SPI2_MISO"     	:	5,
	"PD3:SPI2_SCK"      	:	3,
	"PD3:USART2_CTS"    	:	7,
	"PD4:DFSDM1_CKIN0"  	:	6,
	"PD4:EVENTOUT"      	:	15,
	"PD4:FMC_NOE"       	:	12,
	"PD4:QUADSPI_BK2_IO0"	:	10,
	"PD4:SPI2_MOSI"     	:	5,
	"PD4:USART2_DE"     	:	7,
	"PD4:USART2_RTS"    	:	7,
	"PD5:EVENTOUT"      	:	15,
	"PD5:FMC_NWE"       	:	12,
	"PD5:QUADSPI_BK2_IO1"	:	10,
	"PD5:USART2_TX"     	:	7,
	"PD6:DCMI_D10"      	:	4,
	"PD6:DFSDM1_DATIN1" 	:	6,
	"PD6:EVENTOUT"      	:	15,
	"PD6:FMC_NWAIT"     	:	12,
	"PD6:QUADSPI_BK2_IO1"	:	5,
	"PD6:QUADSPI_BK2_IO2"	:	10,
	"PD6:SAI1_SD_A"     	:	13,
	"PD6:USART2_RX"     	:	7,
	"PD7:DFSDM1_CKIN1"  	:	6,
	"PD7:EVENTOUT"      	:	15,
	"PD7:FMC_NE1"       	:	12,
	"PD7:QUADSPI_BK2_IO3"	:	10,
	"PD7:USART2_CK"     	:	7,
	"PD8:DCMI_HSYNC"    	:	10,
	"PD8:EVENTOUT"      	:	15,
	"PD8:FMC_D13"       	:	12,
	"PD8:FMC_DA13"      	:	12,
	"PD8:LCD_SEG28"     	:	11,
	"PD8:USART3_TX"     	:	7,
	"PD9:DCMI_PIXCLK"   	:	10,
	"PD9:EVENTOUT"      	:	15,
	"PD9:FMC_D14"       	:	12,
	"PD9:FMC_DA14"      	:	12,
	"PD9:LCD_SEG29"     	:	11,
	"PD9:SAI2_MCLK_A"   	:	13,
	"PD9:USART3_RX"     	:	7,
	"PD10:EVENTOUT"     	:	15,
	"PD10:FMC_D15"      	:	12,
	"PD10:FMC_DA15"     	:	12,
	"PD10:LCD_SEG30"    	:	11,
	"PD10:SAI2_SCK_A"   	:	13,
	"PD10:TSC_G6_IO1"   	:	9,
	"PD10:USART3_CK"    	:	7,
	"PD11:EVENTOUT"     	:	15,
	"PD11:FMC_A16"      	:	12,
	"PD11:I2C4_SMBA"    	:	4,
	"PD11:LCD_SEG31"    	:	11,
	"PD11:LPTIM2_ETR"   	:	14,
	"PD11:SAI2_SD_A"    	:	13,
	"PD11:TSC_G6_IO2"   	:	9,
	"PD11:USART3_CTS"   	:	7,
	"PD12:EVENTOUT"     	:	15,
	"PD12:FMC_A17"      	:	12,
	"PD12:I2C4_SCL"     	:	4,
	"PD12:LCD_SEG32"    	:	11,
	"PD12:LPTIM2_IN1"   	:	14,
	"PD12:SAI2_FS_A"    	:	13,
	"PD12:TIM4_CH1"     	:	2,
	"PD12:TSC_G6_IO3"   	:	9,
	"PD12:USART3_DE"    	:	7,
	"PD12:USART3_RTS"   	:	7,
	"PD13:EVENTOUT"     	:	15,
	"PD13:FMC_A18"      	:	12,
	"PD13:I2C4_SDA"     	:	4,
	"PD13:LCD_SEG33"    	:	11,
	"PD13:LPTIM2_OUT"   	:	14,
	"PD13:TIM4_CH2"     	:	2,
	"PD13:TSC_G6_IO4"   	:	9,
	"PD14:EVENTOUT"     	:	15,
	"PD14:FMC_D0"       	:	12,
	"PD14:FMC_DA0"      	:	12,
	"PD14:LCD_SEG34"    	:	11,
	"PD14:TIM4_CH3"     	:	2,
	"PD15:EVENTOUT"     	:	15,
	"PD15:FMC_D1"       	:	12,
	"PD15:FMC_DA1"      	:	12,
	"PD15:LCD_SEG35"    	:	11,
	"PD15:TIM4_CH4"     	:	2,
	"PE0:DCMI_D2"       	:	10,
	"PE0:EVENTOUT"      	:	15,
	"PE0:FMC_NBL0"      	:	12,
	"PE0:LCD_SEG36"     	:	11,
	"PE0:TIM16_CH1"     	:	14,
	"PE0:TIM4_ETR"      	:	2,
	"PE1:DCMI_D3"       	:	10,
	"PE1:EVENTOUT"      	:	15,
	"PE1:FMC_NBL1"      	:	12,
	"PE1:LCD_SEG37"     	:	11,
	"PE1:TIM17_CH1"     	:	14,
	"PE2:EVENTOUT"      	:	15,
	"PE2:FMC_A23"       	:	12,
	"PE2:LCD_SEG38"     	:	11,
	"PE2:SAI1_MCLK_A"   	:	13,
	"PE2:SYS_TRACECLK"  	:	0,
	"PE2:TIM3_ETR"      	:	2,
	"PE2:TSC_G7_IO1"    	:	9,
	"PE3:EVENTOUT"      	:	15,
	"PE3:FMC_A19"       	:	12,
	"PE3:LCD_SEG39"     	:	11,
	"PE3:SAI1_SD_B"     	:	13,
	"PE3:SYS_TRACED0"   	:	0,
	"PE3:TIM3_CH1"      	:	2,
	"PE3:TSC_G7_IO2"    	:	9,
	"PE4:DCMI_D4"       	:	10,
	"PE4:DFSDM1_DATIN3" 	:	6,
	"PE4:EVENTOUT"      	:	15,
	"PE4:FMC_A20"       	:	12,
	"PE4:SAI1_FS_A"     	:	13,
	"PE4:SYS_TRACED1"   	:	0,
	"PE4:TIM3_CH2"      	:	2,
	"PE4:TSC_G7_IO3"    	:	9,
	"PE5:DCMI_D6"       	:	10,
	"PE5:DFSDM1_CKIN3"  	:	6,
	"PE5:EVENTOUT"      	:	15,
	"PE5:FMC_A21"       	:	12,
	"PE5:SAI1_SCK_A"    	:	13,
	"PE5:SYS_TRACED2"   	:	0,
	"PE5:TIM3_CH3"      	:	2,
	"PE5:TSC_G7_IO4"    	:	9,
	"PE6:DCMI_D7"       	:	10,
	"PE6:EVENTOUT"      	:	15,
	"PE6:FMC_A22"       	:	12,
	"PE6:SAI1_SD_A"     	:	13,
	"PE6:SYS_TRACED3"   	:	0,
	"PE6:TIM3_CH4"      	:	2,
	"PE7:DFSDM1_DATIN2" 	:	6,
	"PE7:EVENTOUT"      	:	15,
	"PE7:FMC_D4"        	:	12,
	"PE7:FMC_DA4"       	:	12,
	"PE7:SAI1_SD_B"     	:	13,
	"PE7:TIM1_ETR"      	:	1,
	"PE8:DFSDM1_CKIN2"  	:	6,
	"PE8:EVENTOUT"      	:	15,
	"PE8:FMC_D5"        	:	12,
	"PE8:FMC_DA5"       	:	12,
	"PE8:SAI1_SCK_B"    	:	13,
	"PE8:TIM1_CH1N"     	:	1,
	"PE9:DFSDM1_CKOUT"  	:	6,
	"PE9:EVENTOUT"      	:	15,
	"PE9:FMC_D6"        	:	12,
	"PE9:FMC_DA6"       	:	12,
	"PE9:SAI1_FS_B"     	:	13,
	"PE9:TIM1_CH1"      	:	1,
	"PE10:DFSDM1_DATIN4"	:	6,
	"PE10:EVENTOUT"     	:	15,
	"PE10:FMC_D7"       	:	12,
	"PE10:FMC_DA7"      	:	12,
	"PE10:QUADSPI_CLK"  	:	10,
	"PE10:SAI1_MCLK_B"  	:	13,
	"PE10:TIM1_CH2N"    	:	1,
	"PE10:TSC_G5_IO1"   	:	9,
	"PE11:DFSDM1_CKIN4" 	:	6,
	"PE11:EVENTOUT"     	:	15,
	"PE11:FMC_D8"       	:	12,
	"PE11:FMC_DA8"      	:	12,
	"PE11:QUADSPI_BK1_NCS"	:	10,
	"PE11:TIM1_CH2"     	:	1,
	"PE11:TSC_G5_IO2"   	:	9,
	"PE12:DFSDM1_DATIN5"	:	6,
	"PE12:EVENTOUT"     	:	15,
	"PE12:FMC_D9"       	:	12,
	"PE12:FMC_DA9"      	:	12,
	"PE12:QUADSPI_BK1_IO0"	:	10,
	"PE12:SPI1_NSS"     	:	5,
	"PE12:TIM1_CH3N"    	:	1,
	"PE12:TSC_G5_IO3"   	:	9,
	"PE13:DFSDM1_CKIN5" 	:	6,
	"PE13:EVENTOUT"     	:	15,
	"PE13:FMC_D10"      	:	12,
	"PE13:FMC_DA10"     	:	12,
	"PE13:QUADSPI_BK1_IO1"	:	10,
	"PE13:SPI1_SCK"     	:	5,
	"PE13:TIM1_CH3"     	:	1,
	"PE13:TSC_G5_IO4"   	:	9,
	"PE14:EVENTOUT"     	:	15,
	"PE14:FMC_D11"      	:	12,
	"PE14:FMC_DA11"     	:	12,
	"PE14:QUADSPI_BK1_IO2"	:	10,
	"PE14:SPI1_MISO"    	:	5,
	"PE14:TIM1_BKIN2"   	:	2,
	"PE14:TIM1_BKIN2_COMP2"	:	3,
	"PE14:TIM1_CH4"     	:	1,
	"PE15:EVENTOUT"     	:	15,
	"PE15:FMC_D12"      	:	12,
	"PE15:FMC_DA12"     	:	12,
	"PE15:QUADSPI_BK1_IO3"	:	10,
	"PE15:SPI1_MOSI"    	:	5,
	"PE15:TIM1_BKIN"    	:	1,
	"PE15:TIM1_BKIN_COMP1"	:	3,
	"PF0:EVENTOUT"      	:	15,
	"PF0:FMC_A0"        	:	12,
	"PF0:I2C2_SDA"      	:	4,
	"PF1:EVENTOUT"      	:	15,
	"PF1:FMC_A1"        	:	12,
	"PF1:I2C2_SCL"      	:	4,
	"PF2:EVENTOUT"      	:	15,
	"PF2:FMC_A2"        	:	12,
	"PF2:I2C2_SMBA"     	:	4,
	"PF3:EVENTOUT"      	:	15,
	"PF3:FMC_A3"        	:	12,
	"PF4:EVENTOUT"      	:	15,
	"PF4:FMC_A4"        	:	12,
	"PF5:EVENTOUT"      	:	15,
	"PF5:FMC_A5"        	:	12,
	"PF6:EVENTOUT"      	:	15,
	"PF6:QUADSPI_BK1_IO3"	:	10,
	"PF6:SAI1_SD_B"     	:	13,
	"PF6:TIM5_CH1"      	:	2,
	"PF6:TIM5_ETR"      	:	1,
	"PF7:EVENTOUT"      	:	15,
	"PF7:QUADSPI_BK1_IO2"	:	10,
	"PF7:SAI1_MCLK_B"   	:	13,
	"PF7:TIM5_CH2"      	:	2,
	"PF8:EVENTOUT"      	:	15,
	"PF8:QUADSPI_BK1_IO0"	:	10,
	"PF8:SAI1_SCK_B"    	:	13,
	"PF8:TIM5_CH3"      	:	2,
	"PF9:EVENTOUT"      	:	15,
	"PF9:QUADSPI_BK1_IO1"	:	10,
	"PF9:SAI1_FS_B"     	:	13,
	"PF9:TIM15_CH1"     	:	14,
	"PF9:TIM5_CH4"      	:	2,
	"PF10:DCMI_D11"     	:	10,
	"PF10:EVENTOUT"     	:	15,
	"PF10:QUADSPI_CLK"  	:	3,
	"PF10:TIM15_CH2"    	:	14,
	"PF11:DCMI_D12"     	:	10,
	"PF11:EVENTOUT"     	:	15,
	"PF12:EVENTOUT"     	:	15,
	"PF12:FMC_A6"       	:	12,
	"PF13:DFSDM1_DATIN6"	:	6,
	"PF13:EVENTOUT"     	:	15,
	"PF13:FMC_A7"       	:	12,
	"PF13:I2C4_SMBA"    	:	4,
	"PF14:DFSDM1_CKIN6" 	:	6,
	"PF14:EVENTOUT"     	:	15,
	"PF14:FMC_A8"       	:	12,
	"PF14:I2C4_SCL"     	:	4,
	"PF14:TSC_G8_IO1"   	:	9,
	"PF15:EVENTOUT"     	:	15,
	"PF15:FMC_A9"       	:	12,
	"PF15:I2C4_SDA"     	:	4,
	"PF15:TSC_G8_IO2"   	:	9,
	"PG0:EVENTOUT"      	:	15,
	"PG0:FMC_A10"       	:	12,
	"PG0:TSC_G8_IO3"    	:	9,
	"PG1:EVENTOUT"      	:	15,
	"PG1:FMC_A11"       	:	12,
	"PG1:TSC_G8_IO4"    	:	9,
	"PG2:EVENTOUT"      	:	15,
	"PG2:FMC_A12"       	:	12,
	"PG2:SAI2_SCK_B"    	:	13,
	"PG2:SPI1_SCK"      	:	5,
	"PG3:EVENTOUT"      	:	15,
	"PG3:FMC_A13"       	:	12,
	"PG3:SAI2_FS_B"     	:	13,
	"PG3:SPI1_MISO"     	:	5,
	"PG4:EVENTOUT"      	:	15,
	"PG4:FMC_A14"       	:	12,
	"PG4:SAI2_MCLK_B"   	:	13,
	"PG4:SPI1_MOSI"     	:	5,
	"PG5:EVENTOUT"      	:	15,
	"PG5:FMC_A15"       	:	12,
	"PG5:LPUART1_CTS"   	:	8,
	"PG5:SAI2_SD_B"     	:	13,
	"PG5:SPI1_NSS"      	:	5,
	"PG6:EVENTOUT"      	:	15,
	"PG6:I2C3_SMBA"     	:	4,
	"PG6:LPUART1_DE"    	:	8,
	"PG6:LPUART1_RTS"   	:	8,
	"PG7:EVENTOUT"      	:	15,
	"PG7:FMC_INT"       	:	12,
	"PG7:I2C3_SCL"      	:	4,
	"PG7:LPUART1_TX"    	:	8,
	"PG7:SAI1_MCLK_A"   	:	13,
	"PG8:EVENTOUT"      	:	15,
	"PG8:I2C3_SDA"      	:	4,
	"PG8:LPUART1_RX"    	:	8,
	"PG9:EVENTOUT"      	:	15,
	"PG9:FMC_NCE"       	:	12,
	"PG9:FMC_NE2"       	:	12,
	"PG9:SAI2_SCK_A"    	:	13,
	"PG9:SPI3_SCK"      	:	6,
	"PG9:TIM15_CH1N"    	:	14,
	"PG9:USART1_TX"     	:	7,
	"PG10:EVENTOUT"     	:	15,
	"PG10:FMC_NE3"      	:	12,
	"PG10:LPTIM1_IN1"   	:	1,
	"PG10:SAI2_FS_A"    	:	13,
	"PG10:SPI3_MISO"    	:	6,
	"PG10:TIM15_CH1"    	:	14,
	"PG10:USART1_RX"    	:	7,
	"PG11:EVENTOUT"     	:	15,
	"PG11:LPTIM1_IN2"   	:	1,
	"PG11:SAI2_MCLK_A"  	:	13,
	"PG11:SPI3_MOSI"    	:	6,
	"PG11:TIM15_CH2"    	:	14,
	"PG11:USART1_CTS"   	:	7,
	"PG12:EVENTOUT"     	:	15,
	"PG12:FMC_NE4"      	:	12,
	"PG12:LPTIM1_ETR"   	:	1,
	"PG12:SAI2_SD_A"    	:	13,
	"PG12:SPI3_NSS"     	:	6,
	"PG12:USART1_DE"    	:	7,
	"PG12:USART1_RTS"   	:	7,
	"PG13:EVENTOUT"     	:	15,
	"PG13:FMC_A24"      	:	12,
	"PG13:I2C1_SDA"     	:	4,
	"PG13:USART1_CK"    	:	7,
	"PG14:EVENTOUT"     	:	15,
	"PG14:FMC_A25"      	:	12,
	"PG14:I2C1_SCL"     	:	4,
	"PG15:DCMI_D13"     	:	10,
	"PG15:EVENTOUT"     	:	15,
	"PG15:I2C1_SMBA"    	:	4,
	"PG15:LPTIM1_OUT"   	:	1,
	"PH0:EVENTOUT"      	:	15,
	"PH1:EVENTOUT"      	:	15,
	"PH3:EVENTOUT"      	:	15,
}

ADC1_map = {
	# format is PIN : ADC1_CHAN
    "PA0"	:	5,
    "PA1"	:	6,
    "PA2"	:	7,
    "PA3"	:	8,
    "PA4"	:	9,
    "PA5"	:	10,
    "PA6"	:	11,
    "PA7"	:	12,
    "PC4"   :   13,
    "PC5"   :   14,
    "PB0"   :   15,
    "PB1"   :   16,
    "PC0"   :   1,
    "PC1"   :   2,
    "PC2"   :   3,
    "PC3"   :   4,
}
    
