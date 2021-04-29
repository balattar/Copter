#!/usr/bin/env python
'''
these tables are generated from the STM32 datasheet DM00273119.pdf for the
STM32F765xx, STM32F767xx, STM32F768aX and STM32F769xx
'''

# additional build information for ChibiOS
build = {
    "CHIBIOS_STARTUP_MK"  : "os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f7xx.mk",
    "CHIBIOS_PLATFORM_MK" : "os/hal/ports/STM32/STM32F7xx/platform.mk"
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
    'I': 16,
    'J': 0,
    'K': 0
}


# MCU parameters
mcu = {
    # ram map, as list of (address, size-kb, flags)
    # flags of 1 means DMA-capable
    # flags of 2 means faster memory for CPU intensive work
    'RAM_MAP' : [
        (0x20020000, 384, 0), # SRAM1/SRAM2
        # split DTCM in two to allow for fast checking of IS_DMA_SAFE in bouncebuffer code
        (0x20000000,  64, 1), # DTCM, DMA safe
        (0x20010000,  64, 2), # DTCM, 2nd half, used as fast memory. This lowers memory contention in the EKF code
    ],

    'EXPECTED_CLOCK' : 216000000,

    # this MCU has M7 instructions and hardware double precision
    'CORTEX'    : 'cortex-m7',
    'CPU_FLAGS' : '-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard',
}

DMA_Map = {
	# format is (DMA_TABLE, StreamNum, Channel)
	"ADC1"    	:	[(2,0,0),(2,4,0)],
	"ADC2"    	:	[(2,2,1),(2,3,1)],
	"ADC3"    	:	[(2,0,2),(2,1,2)],
	"CRYP_IN" 	:	[(2,6,2)],
	"CRYP_OUT"	:	[(2,5,2)],
	"DAC1"    	:	[(1,5,7)],
	"DAC2"    	:	[(1,6,7)],
	"DCMI"    	:	[(2,1,1),(2,7,1)],
	"DFSDM1_FLT0" 	:	[(2,0,8),(2,4,8)],
	"DFSDM1_FLT1" 	:	[(2,1,8),(2,5,8)],
	"DFSDM1_FLT2" 	:	[(2,2,8),(2,6,8)],
	"DFSDM1_FLT3" 	:	[(2,3,8),(2,7,8)],
	"HASH_IN" 	:	[(2,7,2)],
	"I2C1_RX" 	:	[(1,0,1),(1,5,1)],
	"I2C1_TX" 	:	[(1,6,1),(1,7,1)],
	"I2C2_RX" 	:	[(1,2,7),(1,3,7)],
	"I2C2_TX" 	:	[(1,7,7),(1,4,8)],
	"I2C3_RX" 	:	[(1,2,3),(1,1,1)],
	"I2C3_TX" 	:	[(1,4,3),(1,0,8)],
	"I2C4_RX"    	:	[(1,2,2),(1,1,8)],
	"I2C4_TX" 	:	[(1,6,8),(1,5,2)],
	"JPEG_IN" 	:	[(2,0,9),(2,3,9)],
	"JPEG_OUT" 	:	[(2,1,9),(2,4,9)],
	"QUADSPI" 	:	[(2,2,11),(2,7,3)],
	"SAI1_A"  	:	[(2,1,0),(2,3,0),(2,6,10)],
	"SAI1_B"  	:	[(2,0,10),(2,4,1),(2,5,0)],
	"SAI2_A"  	:	[(2,2,10),(2,4,3)],
	"SAI2_B"  	:	[(2,1,10),(2,6,3),(2,7,0)],
	"SDMMC1"    	:	[(2,3,4),(2,6,4)],
	"SDMMC2"    	:	[(2,0,11),(2,5,11)],
	"SPDIFRX_CS" 	:	[(1,6,0)],
	"SPDIFRX_DT" 	:	[(1,1,0)],
	"SPI1_RX" 	:	[(2,0,3),(2,2,3)],
	"SPI1_TX" 	:	[(2,3,3),(2,5,3)],
	"SPI2_RX" 	:	[(1,1,9),(1,3,0)],
	"SPI2_TX" 	:	[(1,4,0),(1,6,9)],
	"SPI3_RX" 	:	[(1,0,0),(1,2,0)],
	"SPI3_TX" 	:	[(1,5,0),(1,7,0)],
	"SPI4_RX" 	:	[(2,0,4),(2,3,5)],
	"SPI4_TX" 	:	[(2,1,4),(2,2,9),(2,4,5)],
	"SPI5_RX" 	:	[(2,3,2),(2,5,7),(2,5,9)],
	"SPI5_TX" 	:	[(2,4,2),(2,6,7)],
	"SPI6_RX" 	:	[(2,6,1)],
	"SPI6_TX" 	:	[(2,5,1)],
	"TIM1_CH1"	:	[(2,1,6),(2,3,6),(2,6,0)],
	"TIM1_CH2"	:	[(2,2,6),(2,6,0)],
	"TIM1_CH3"	:	[(2,6,0),(2,6,6)],
	"TIM1_CH4"	:	[(2,4,6)],
	"TIM1_COM"	:	[(2,4,6)],
	"TIM1_TRIG"	:	[(2,0,6),(2,4,6)],
	"TIM1_UP" 	:	[(2,5,6)],
	"TIM2_CH1"	:	[(1,5,3)],
	"TIM2_CH2"	:	[(1,6,3)],
	"TIM2_CH3"	:	[(1,1,3)],
	"TIM2_CH4"	:	[(1,6,3),(1,7,3)],
	"TIM2_UP" 	:	[(1,1,3),(1,7,3)],
	"TIM3_CH1"	:	[(1,4,5)],
	"TIM3_CH2"	:	[(1,5,5)],
	"TIM3_CH3"	:	[(1,7,5)],
	"TIM3_CH4"	:	[(1,2,5)],
	"TIM3_TRIG"	:	[(1,4,5)],
	"TIM3_UP" 	:	[(1,2,5)],
	"TIM4_CH1"	:	[(1,0,2)],
	"TIM4_CH2"	:	[(1,3,2)],
	"TIM4_CH3"	:	[(1,7,2)],
	"TIM4_UP" 	:	[(1,6,2)],
	"TIM5_CH1"	:	[(1,2,6)],
	"TIM5_CH2"	:	[(1,4,6)],
	"TIM5_CH3"	:	[(1,0,6)],
	"TIM5_CH4"	:	[(1,1,6),(1,3,6)],
	"TIM5_TRIG"	:	[(1,1,6),(1,3,6)],
	"TIM5_UP" 	:	[(1,0,6),(1,6,6)],
	"TIM6_UP" 	:	[(1,1,7)],
	"TIM7_UP" 	:	[(1,2,1),(1,4,1)],
	"TIM8_CH1"	:	[(2,2,0),(2,2,7)],
	"TIM8_CH2"	:	[(2,2,0),(2,3,7)],
	"TIM8_CH3"	:	[(2,2,0),(2,4,7)],
	"TIM8_CH4"	:	[(2,7,7)],
	"TIM8_COM"	:	[(2,7,7)],
	"TIM8_TRIG"	:	[(2,7,7)],
	"TIM8_UP" 	:	[(2,1,7)],
	"UART4_RX"	:	[(1,2,4)],
	"UART4_TX"	:	[(1,4,4)],
	"UART5_RX"	:	[(1,0,4)],
	"UART5_TX"	:	[(1,7,4)],
	"UART7_RX"	:	[(1,3,5)],
	"UART7_TX"	:	[(1,1,5)],
	"UART8_RX"	:	[(1,6,5)],
	"UART8_TX"	:	[(1,0,5)],
	"USART1_RX"	:	[(2,2,4),(2,5,4)],
	"USART1_TX"	:	[(2,7,4)],
	"USART2_RX"	:	[(1,5,4)],
	"USART2_TX"	:	[(1,6,4)],
	"USART3_RX"	:	[(1,1,4)],
	"USART3_TX"	:	[(1,3,4),(1,4,7)],
	"USART6_RX"	:	[(2,1,5),(2,2,5)],
	"USART6_TX"	:	[(2,6,5),(2,7,5)],
}

AltFunction_map = {
	# format is PIN:FUNCTION : AFNUM
	"PA0:ETH_MII_CRS"   	:	11,
	"PA0:EVENTOUT"      	:	15,
	"PA0:TIM2_CH1"      	:	1,
	"PA0:TIM2_ETR"      	:	1,
	"PA0:TIM5_CH1"      	:	2,
	"PA0:TIM8_ETR"      	:	3,
	"PA0:UART4_TX"      	:	8,
	"PA0:USART2_CTS"    	:	7,
	"PA0:SAI2_SD_B"    	:	10,
	"PA10:DCMI_D1"      	:	13,
	"PA10:EVENTOUT"     	:	15,
	"PA10:OTG_FS_ID"    	:	10,
	"PA10:TIM1_CH3"     	:	1,
	"PA10:USART1_RX"    	:	7,
	"PA10:LCD_B4"    	:	9,
	"PA10:LCD_B1"    	:	14,
	"PA10:MDIOS_MDIO"    	:	12,
	"PA11:CAN1_RX"      	:	9,
	"PA11:EVENTOUT"     	:	15,
	"PA11:LCD_R4"       	:	14,
	"PA11:OTG_FS_DM"    	:	10,
	"PA11:TIM1_CH4"     	:	1,
	"PA11:USART1_CTS"   	:	7,
	"PA11:SPI2_NSS"   	:	5,
	"PA11:I2S2_WS"   	:	5,
	"PA11:UART4_RX"   	:	6,
	"PA12:CAN1_TX"      	:	9,
	"PA12:EVENTOUT"     	:	15,
	"PA12:LCD_R5"       	:	14,
	"PA12:OTG_FS_DP"    	:	10,
	"PA12:TIM1_ETR"     	:	1,
	"PA12:USART1_RTS"   	:	7,
	"PA12:SPI2_SCK"   	:	5,
	"PA12:I2S2_CK"   	:	5,
	"PA12:UART4_TX"   	:	6,
	"PA12:SAI2_FS_B"   	:	8,
	"PA13:EVENTOUT"     	:	15,
	"PA13:JTMS-SWDIO"   	:	0,
	"PA14:EVENTOUT"     	:	15,
	"PA14:JTCK-SWCLK"   	:	0,
	"PA15:EVENTOUT"     	:	15,
	"PA15:I2S3_WS"      	:	6,
	"PA15:JTDI"         	:	0,
	"PA15:SPI1_NSS"     	:	5,
	"PA15:SPI3_NSS"     	:	6,
	"PA15:TIM2_CH1"     	:	1,
	"PA15:TIM2_ETR"     	:	1,
	"PA15:HDMI-CEC"     	:	4,
	"PA15:SPI6_NSS"     	:	7,
	"PA15:UART4_RTS"     	:	8,
	"PA15:CAN3_TX"     	:	11,
	"PA15:UART7_TX"     	:	12,
	"PA1:ETH_MII_RX_CLK"	:	11,
	"PA1:ETH_RMII_REF_CLK"	:	11,
	"PA1:EVENTOUT"      	:	15,
	"PA1:TIM2_CH2"      	:	1,
	"PA1:TIM5_CH2"      	:	2,
	"PA1:UART4_RX"      	:	8,
	"PA1:USART2_RTS"    	:	7,
	"PA1:QUADSPI_BK1_IO3"  	:	9,
	"PA1:SAI2_MCK_B"    	:	10,
	"PA1:LCD_R2"    	:	14,
	"PA2:ETH_MDIO"      	:	11,
	"PA2:EVENTOUT"      	:	15,
	"PA2:TIM2_CH3"      	:	1,
	"PA2:TIM5_CH3"      	:	2,
	"PA2:TIM9_CH1"      	:	3,
	"PA2:USART2_TX"     	:	7,
	"PA2:SAI2_SCK_B"     	:	8,
	"PA2:MDIOS_MDIO"     	:	12,
	"PA2:LCD_R1"     	:	14,
	"PA3:ETH_MII_COL"   	:	11,
	"PA3:EVENTOUT"      	:	15,
	"PA3:LCD_B5"        	:	14,
	"PA3:OTG_HS_ULPI_D0"	:	10,
	"PA3:TIM2_CH4"      	:	1,
	"PA3:TIM5_CH4"      	:	2,
	"PA3:TIM9_CH2"      	:	3,
	"PA3:USART2_RX"     	:	7,
	"PA3:LCD_B2"     	:	9,
	"PA4:DCMI_HSYNC"    	:	13,
	"PA4:EVENTOUT"      	:	15,
	"PA4:I2S3_WS"       	:	6,
	"PA4:LCD_VSYNC"     	:	14,
	"PA4:OTG_HS_SOF"    	:	12,
	"PA4:SPI1_NSS"      	:	5,
	"PA4:SPI3_NSS"      	:	6,
	"PA4:USART2_CK"     	:	7,
	"PA4:I2S1_WS"       	:	5,
	"PA4:SPI6_NSS"       	:	8,
	"PA5:EVENTOUT"      	:	15,
	"PA5:OTG_HS_ULPI_CK"	:	10,
	"PA5:SPI1_SCK"      	:	5,
	"PA5:TIM2_CH1"      	:	1,
	"PA5:TIM2_ETR"      	:	1,
	"PA5:TIM8_CH1N"     	:	3,
	"PA5:I2S1_CK"     	:	5,
	"PA5:SPI6_SCK"     	:	8,
	"PA5:LCD_R4"     	:	14,
	"PA6:DCMI_PIXCLK"   	:	13,
	"PA6:EVENTOUT"      	:	15,
	"PA6:LCD_G2"        	:	14,
	"PA6:SPI1_MISO"     	:	5,
	"PA6:TIM13_CH1"     	:	9,
	"PA6:TIM1_BKIN"     	:	1,
	"PA6:TIM3_CH1"      	:	2,
	"PA6:TIM8_BKIN"     	:	3,
	"PA6:SPI6_MISO"     	:	8,
	"PA6:MDIOS_MDC"     	:	12,
	"PA7:ETH_MII_RX_DV" 	:	11,
	"PA7:ETH_RMII_CRS_DV"	:	11,
	"PA7:EVENTOUT"      	:	15,
	"PA7:SPI1_MOSI"     	:	5,
	"PA7:TIM14_CH1"     	:	9,
	"PA7:TIM1_CH1N"     	:	1,
	"PA7:TIM3_CH2"      	:	2,
	"PA7:TIM8_CH1N"     	:	3,
	"PA7:I2S1_SD"     	:	5,
	"PA7:SPI6_MOSI"     	:	8,
	"PA7:FMC_SDNWE"     	:	12,
	"PA8:EVENTOUT"      	:	15,
	"PA8:I2C3_SCL"      	:	4,
	"PA8:LCD_R6"        	:	14,
	"PA8:MCO1"          	:	0,
	"PA8:OTG_FS_SOF"    	:	10,
	"PA8:TIM1_CH1"      	:	1,
	"PA8:USART1_CK"     	:	7,
	"PA8:TIM8_BKIN2"     	:	3,
	"PA8:CAN3_RX"     	:	11,
	"PA8:UART7_RX"     	:	12,
	"PA8:LCD_B3"     	:	13,
	"PA9:DCMI_D0"       	:	13,
	"PA9:EVENTOUT"      	:	15,
	"PA9:I2C3_SMBA"     	:	4,
	"PA9:TIM1_CH2"      	:	1,
	"PA9:USART1_TX"     	:	7,
	"PA9:SPI2_SCK"     	:	5,
	"PA9:I2S2_CK"     	:	5,
	"PA9:LCD_R5"     	:	14,
	"PB0:ETH_MII_RXD2"  	:	11,
	"PB0:EVENTOUT"      	:	15,
	"PB0:LCD_R3"        	:	9,
	"PB0:OTG_HS_ULPI_D1"	:	10,
	"PB0:TIM3_CH3"      	:	2,
	"PB0:TIM8_CH2N"     	:	3,
	"PB0:DFSDM1_CKOUT"     	:	6,
	"PB0:UART4_CTS"     	:	8,
	"PB0:LCD_G1"     	:	14,
	"PB10:ETH_MII_RX_ER"	:	11,
	"PB10:EVENTOUT"     	:	15,
	"PB10:I2C2_SCL"     	:	4,
	"PB10:I2S2_CK"      	:	5,
	"PB10:LCD_G4"       	:	14,
	"PB10:OTG_HS_ULPI_D3"	:	10,
	"PB10:SPI2_SCK"     	:	5,
	"PB10:TIM2_CH3"     	:	1,
	"PB10:USART3_TX"    	:	7,
	"PB10:DFSDM1_DATIN7"   	:	6,
	"PB10:QUADSPI_BK1_NCS" 	:	9,
	"PB11:ETH_MII_TX_EN"	:	11,
	"PB11:ETH_RMII_TX_EN"	:	11,
	"PB11:EVENTOUT"     	:	15,
	"PB11:I2C2_SDA"     	:	4,
	"PB11:LCD_G5"       	:	14,
	"PB11:OTG_HS_ULPI_D4"	:	10,
	"PB11:TIM2_CH4"     	:	1,
	"PB11:USART3_RX"    	:	7,
	"PB11:DFSDM1_CKIN7"    	:	6,
	"PB11:DSI_TE"    	:	13,
	"PB12:CAN2_RX"      	:	9,
	"PB12:ETH_MII_TXD0" 	:	11,
	"PB12:ETH_RMII_TXD0"	:	11,
	"PB12:EVENTOUT"     	:	15,
	"PB12:I2C2_SMBA"    	:	4,
	"PB12:I2S2_WS"      	:	5,
	"PB12:OTG_HS_ID"    	:	12,
	"PB12:OTG_HS_ULPI_D5"	:	10,
	"PB12:SPI2_NSS"     	:	5,
	"PB12:TIM1_BKIN"    	:	1,
	"PB12:USART3_CK"    	:	7,
	"PB12:DFSDM1_DATIN1"   	:	6,
	"PB12:UART5_RX"    	:	8,
	"PB13:CAN2_TX"      	:	9,
	"PB13:ETH_MII_TXD1" 	:	11,
	"PB13:ETH_RMII_TXD1"	:	11,
	"PB13:EVENTOUT"     	:	15,
	"PB13:I2S2_CK"      	:	5,
	"PB13:OTG_HS_ULPI_D6"	:	10,
	"PB13:SPI2_SCK"     	:	5,
	"PB13:TIM1_CH1N"    	:	1,
	"PB13:USART3_CTS"   	:	7,
	"PB13:DFSDM1_CKIN1"    	:	6,
	"PB13:UART5_TX"   	:	8,
	"PB14:EVENTOUT"     	:	15,
	"PB14:DFSDM1_DATIN2"   	:	6,
	"PB14:OTG_HS_DM"    	:	12,
	"PB14:SPI2_MISO"    	:	5,
	"PB14:TIM12_CH1"    	:	9,
	"PB14:TIM1_CH2N"    	:	1,
	"PB14:TIM8_CH2N"    	:	3,
	"PB14:USART3_RTS"   	:	7,
	"PB14:USART1_TX"    	:	4,
	"PB14:UART4_RTS"    	:	8,
	"PB14:SDMMC2_D0"   	:	10,
	"PB15:EVENTOUT"     	:	15,
	"PB15:I2S2_SD"      	:	5,
	"PB15:OTG_HS_DP"    	:	12,
	"PB15:RTC_REFIN"    	:	0,
	"PB15:SPI2_MOSI"    	:	5,
	"PB15:TIM12_CH2"    	:	9,
	"PB15:TIM1_CH3N"    	:	1,
	"PB15:TIM8_CH3N"    	:	3,
	"PB15:USART1_RX"    	:	4,
	"PB15:DFSDM1_CKIN2"    	:	6,
	"PB15:UART4_CTS"    	:	8,
	"PB15:SDMMC2_D1"    	:	10,
	"PB1:ETH_MII_RXD3"  	:	11,
	"PB1:EVENTOUT"      	:	15,
	"PB1:LCD_R6"        	:	9,
	"PB1:OTG_HS_ULPI_D2"	:	10,
	"PB1:TIM3_CH4"      	:	2,
	"PB1:TIM8_CH3N"     	:	3,
	"PB1:DFSDM1_DATIN1"    	:	6,
	"PB1:LCD_G0"     	:	14,
	"PB2:EVENTOUT"      	:	15,
	"PB2:SAI1_SD_A"      	:	6,
	"PB2:SPI3_MOSI"      	:	7,
	"PB2:I2S3_SD"      	:	7,
	"PB2:QUADSPI_CLK"      	:	9,
	"PB2:DFSDM1_CKIN1"     	:	10,
	"PB3:EVENTOUT"      	:	15,
	"PB3:I2S3_CK"       	:	6,
	"PB3:JTDO"          	:	0,
	"PB3:SPI1_SCK"      	:	5,
	"PB3:SPI3_SCK"      	:	6,
	"PB3:TIM2_CH2"      	:	1,
	"PB3:TRACESWO"      	:	0,
	"PB3:I2S1_CK"       	:	5,
	"PB3:SPI6_SCK"         	:	8,
	"PB3:SDMMC2_D2"      	:	10,
	"PB3:CAN3_RX"      	:	11,
	"PB3:UART7_RX"      	:	12,
	"PB4:EVENTOUT"      	:	15,
	"PB4:I2S2_WS"    	:	7,
	"PB4:SPI2_NSS"    	:	7,
	"PB4:NJTRST"        	:	0,
	"PB4:SPI1_MISO"     	:	5,
	"PB4:SPI3_MISO"     	:	6,
	"PB4:TIM3_CH1"      	:	2,
	"PB4:SPI6_MISO"        	:	8,
	"PB4:SDMMC2_D3"     	:	10,
	"PB4:CAN3_TX"     	:	11,
	"PB4:UART7_TX"      	:	12,
	"PB5:CAN2_RX"       	:	9,
	"PB5:DCMI_D10"      	:	13,
	"PB5:ETH_PPS_OUT"   	:	11,
	"PB5:EVENTOUT"      	:	15,
	"PB5:FMC_SDCKE1"    	:	12,
	"PB5:I2C1_SMBA"     	:	4,
	"PB5:I2S3_SD"       	:	6,
	"PB5:OTG_HS_ULPI_D7"	:	10,
	"PB5:SPI1_MOSI"     	:	5,
	"PB5:SPI3_MOSI"     	:	6,
	"PB5:TIM3_CH2"      	:	2,
	"PB5:UART5_RX"		:	1,
	"PB5:I2S1_SD"     	:	5,
	"PB5:SPI6_MOSI"     	:	8,
	"PB5:LCD_G7"      	:	14,
	"PB6:CAN2_TX"       	:	9,
	"PB6:DCMI_D5"       	:	13,
	"PB6:EVENTOUT"      	:	15,
	"PB6:FMC_SDNE1"     	:	12,
	"PB6:I2C1_SCL"      	:	4,
	"PB6:TIM4_CH1"      	:	2,
	"PB6:USART1_TX"     	:	7,
	"PB6:UART5_TX"     	:	1,
	"PB6:HDMI-CEC"      	:	3,
	"PB6:DFSDM1_DATIN5"    	:	6,
	"PB6:QUADSPI_BK1_NCS"  	:	10,
	"PB6:I2C4_SCL"  	:	11,
	"PB7:DCMI_VSYNC"    	:	13,
	"PB7:EVENTOUT"      	:	15,
	"PB7:FMC_NL"        	:	12,
	"PB7:I2C1_SDA"      	:	4,
	"PB7:TIM4_CH2"      	:	2,
	"PB7:USART1_RX"     	:	7,
	"PB7:DFSDM1_CKIN5"     	:	6,
	"PB7:I2S4_SDA"     	:	11,
	"PB8:CAN1_RX"       	:	9,
	"PB8:DCMI_D6"       	:	13,
	"PB8:ETH_MII_TXD3"  	:	11,
	"PB8:EVENTOUT"      	:	15,
	"PB8:I2C1_SCL"      	:	4,
	"PB8:LCD_B6"        	:	14,
	"PB8:SDMMC_D4"       	:	12,
	"PB8:TIM10_CH1"     	:	3,
	"PB8:TIM4_CH3"      	:	2,
	"PB8:I2C4_SCL"        	:	1,
	"PB8:DFSDM1_CKIN7"     	:	6,
	"PB8:UART5_RX"     	:	7,
	"PB8:SDMMC2_D4"      	:	10,
	"PB9:CAN1_TX"       	:	9,
	"PB9:DCMI_D7"       	:	13,
	"PB9:EVENTOUT"      	:	15,
	"PB9:I2C1_SDA"      	:	4,
	"PB9:I2S2_WS"       	:	5,
	"PB9:LCD_B7"        	:	14,
	"PB9:SDMMC_D5"       	:	12,
	"PB9:SPI2_NSS"      	:	5,
	"PB9:TIM11_CH1"     	:	3,
	"PB9:TIM4_CH4"      	:	2,
	"PB9:I2C4_SDA"       	:	1,
	"PB9:DFSDM1_DATIN7"    	:	6,
	"PB9:UART5_TX"       	:	7,
	"PB9:SDMMC2_D5"      	:	10,
	"PB9:I2C4_SMBA"     	:	11,
	"PC0:EVENTOUT"      	:	15,
	"PC0:FMC_SDNWE"     	:	12,
	"PC0:OTG_HS_ULPI_STP"	:	10,
	"PC0:DFSDM1_DATAIN0"	:	3,
	"PC0:DFSDM1_DATAIN4"	:	6,
	"PC0:SAI2_FS_B" 	:	8,
	"PC0:LCD_R5"		:	14,
	"PC10:DCMI_D8"      	:	13,
	"PC10:EVENTOUT"     	:	15,
	"PC10:I2S3_CK"      	:	6,
	"PC10:LCD_R2"       	:	14,
	"PC10:SDMMC_D2"      	:	12,
	"PC10:SPI3_SCK"     	:	6,
	"PC10:UART4_TX"     	:	8,
	"PC10:USART3_TX"    	:	7,
	"PC10:DFSDM1_CKIN5"    	:	3,
	"PC10:QUADSPI_BK1_IO1" 	:	9,
	"PC11:DCMI_D4"      	:	13,
	"PC11:EVENTOUT"     	:	15,
	"PC11:DFSDM1_DATAIN5"  	:	3,
	"PC11:SDMMC_D3"      	:	12,
	"PC11:SPI3_MISO"    	:	6,
	"PC11:UART4_RX"     	:	8,
	"PC11:USART3_RX"    	:	7,
	"PC11:QUADSPI_BK2_NCS" 	:	9,
	"PC12:DCMI_D9"      	:	13,
	"PC12:EVENTOUT"     	:	15,
	"PC12:I2S3_SD"      	:	6,
	"PC12:SDMMC_CK"      	:	12,
	"PC12:SPI3_MOSI"    	:	6,
	"PC12:UART5_TX"     	:	8,
	"PC12:USART3_CK"    	:	7,
	"PC12:TRACED3"    	:	0,
	"PC13:EVENTOUT"     	:	15,
	"PC14:EVENTOUT"     	:	15,
	"PC15:EVENTOUT"     	:	15,
	"PC1:ETH_MDC"       	:	11,
	"PC1:EVENTOUT"      	:	15,
	"PC1:TRACED0"      	:	0,
	"PC1:DFSDM1_DATAIN0"   	:	3,
	"PC1:SPI2_MOSI"      	:	5,
	"PC1:I2S2_SD"      	:	5,
	"PC1:SAI1_SD_A"      	:	6,
	"PC1:DFSDM1_CKIN4"     	:	10,
	"PC1:MDIOS_MDC"      	:	12,
	"PC2:ETH_MII_TXD2"  	:	11,
	"PC2:EVENTOUT"      	:	15,
	"PC2:FMC_SDNE0"     	:	12,
	"PC2:DFSDM1_CKOUT"    	:	6,
	"PC2:OTG_HS_ULPI_DIR"	:	10,
	"PC2:SPI2_MISO"     	:	5,
	"PC2:DFSDM1_CKIN1"     	:	3,
	"PC3:ETH_MII_TX_CLK"	:	11,
	"PC3:EVENTOUT"      	:	15,
	"PC3:FMC_SDCKE0"    	:	12,
	"PC3:I2S2_SD"       	:	5,
	"PC3:OTG_HS_ULPI_NXT"	:	10,
	"PC3:SPI2_MOSI"     	:	5,
	"PC3:DFSDM1_DATAIN1"   	:	3,
	"PC4:ETH_MII_RXD0"  	:	11,
	"PC4:ETH_RMII_RXD0" 	:	11,
	"PC4:EVENTOUT"      	:	15,
	"PC4:DFSDM1_CKIN2"     	:	3,
	"PC4:I2S1_MCK"      	:	5,
	"PC4:SPDIF_RX2"      	:	8,
	"PC4:FMC_SDNE0"      	:	12,
	"PC5:ETH_MII_RXD1"  	:	11,
	"PC5:ETH_RMII_RXD1" 	:	11,
	"PC5:EVENTOUT"      	:	15,
	"PC5:DFSDM1_DATAIN2"  	:	3,
	"PC5:SPDIF_RX3" 	:	8,
	"PC5:FMC_SDCKE0"      	:	12,
	"PC6:DCMI_D0"       	:	13,
	"PC6:EVENTOUT"      	:	15,
	"PC6:I2S2_MCK"      	:	5,
	"PC6:LCD_HSYNC"     	:	14,
	"PC6:SDMMC_D6"       	:	12,
	"PC6:TIM3_CH1"      	:	2,
	"PC6:TIM8_CH1"      	:	3,
	"PC6:USART6_TX"     	:	8,
	"PC6:DFSDM1_CKIN3"     	:	7,
	"PC6:FMC_NWAIT"      	:	9,
    "PC6:SDMMC2_D6"     	:	10,
	"PC7:DCMI_D1"       	:	13,
	"PC7:EVENTOUT"      	:	15,
	"PC7:I2S3_MCK"      	:	6,
	"PC7:LCD_G6"        	:	14,
	"PC7:SDMMC_D7"       	:	12,
	"PC7:TIM3_CH2"      	:	2,
	"PC7:TIM8_CH2"      	:	3,
	"PC7:USART6_RX"     	:	8,
	"PC7:DFSDM1_DATAIN3"   	:	7,
	"PC7:FMC_NE1"      	:	9,
	"PC7:SDMMC2_D7"     	:	10,
	"PC8:DCMI_D2"       	:	13,
	"PC8:EVENTOUT"      	:	15,
	"PC8:SDMMC_D0"       	:	12,
	"PC8:TIM3_CH3"      	:	2,
	"PC8:TIM8_CH3"      	:	3,
	"PC8:USART6_CK"     	:	8,
	"PC8:TRACED1"      	:	0,
	"PC8:UART5_RTS"      	:	7,
	"PC8:FMC_NE2"     	:	9,
	"PC8:FMC_NCE"     	:	9,
	"PC9:DCMI_D3"       	:	13,
	"PC9:EVENTOUT"      	:	15,
	"PC9:I2C3_SDA"      	:	4,
	"PC9:I2S_CKIN"      	:	5,
	"PC9:MCO2"          	:	0,
	"PC9:SDMMC_D1"       	:	12,
	"PC9:TIM3_CH4"      	:	2,
	"PC9:TIM8_CH4"      	:	3,
	"PC9:UART5_CTS"        	:	7,
	"PC9:QUADSPI_BK1_IO0"  	:	9,
	"PC9:LCD_G3"      	:	10,
	"PC9:LCD_B2"      	:	14,
	"PD0:CAN1_RX"       	:	9,
	"PD0:EVENTOUT"      	:	15,
	"PD0:FMC_D2"        	:	12,
	"PD0:DFSDM1_DATAIN6"   	:	3,
	"PD0:DFSDM1_DATAIN7"   	:	6,
	"PD0:UART4_RX"        	:	8,
	"PD10:EVENTOUT"     	:	15,
	"PD10:FMC_D15"      	:	12,
	"PD10:LCD_B3"       	:	14,
	"PD10:USART3_CK"    	:	7,
	"PD10:DFSDM1_CKOUT"    	:	3,
	"PD11:EVENTOUT"     	:	15,
	"PD11:FMC_A16"      	:	12,
	"PD11:USART3_CTS"   	:	7,
	"PD11:I2C4_SMBA"      	:	4,
	"PD11:QUADSPI_BK1_IO0" 	:	9,
	"PD11:SAI2_SD_A"      	:	10,
	"PD11:FMC_CLE"   	:	12,
	"PD12:EVENTOUT"     	:	15,
	"PD12:FMC_A17"      	:	12,
	"PD12:TIM4_CH1"     	:	2,
	"PD12:USART3_RTS"   	:	7,
	"PD12:LPTIM1_IN1"     	:	3,
	"PD12:I2C4_SCL"      	:	4,
	"PD12:QUADSPI_BK1_IO1" 	:	9,
	"PD12:SAI2_FS_A"   	:	10,
	"PD12:FMC_ALE"   	:	12,
	"PD13:EVENTOUT"     	:	15,
	"PD13:FMC_A18"      	:	12,
	"PD13:TIM4_CH2"     	:	2,
	"PD13:LPTIM1_OUT"     	:	3,
	"PD13:I2C4_SDA"      	:	4,
	"PD13:QUADSPI_BK1_IO3" 	:	9,
	"PD13:SAI2_SCK_A" 	:	10,
	"PD14:EVENTOUT"     	:	15,
	"PD14:FMC_D0"       	:	12,
	"PD14:TIM4_CH3"     	:	2,
	"PD14:UART8_CTS"     	:	8,
	"PD15:EVENTOUT"     	:	15,
	"PD15:FMC_D1"       	:	12,
	"PD15:TIM4_CH4"     	:	2,
	"PD15:UART8_RTS"     	:	8,
	"PD1:CAN1_TX"       	:	9,
	"PD1:EVENTOUT"      	:	15,
	"PD1:FMC_D3"        	:	12,
	"PD1:DFSDM1_DATAIN6"   	:	3,
	"PD1:DFSDM1_CKIN7"     	:	6,
	"PD1:UART4_TX"        	:	8,
	"PD2:DCMI_D11"      	:	13,
	"PD2:EVENTOUT"      	:	15,
	"PD2:SDMMC_CMD"      	:	12,
	"PD2:TIM3_ETR"      	:	2,
	"PD2:UART5_RX"      	:	8,
	"PD2:TRACED2"      	:	0,
	"PD3:DCMI_D5"       	:	13,
	"PD3:EVENTOUT"      	:	15,
	"PD3:FMC_CLK"       	:	12,
	"PD3:I2S2_CK"       	:	5,
	"PD3:LCD_G7"        	:	14,
	"PD3:SPI2_SCK"      	:	5,
	"PD3:USART2_CTS"    	:	7,
	"PD3:DFSDM1_CKOUT"     	:	3,
	"PD3:DFSDM1_DATAIN0"   	:	6,
	"PD4:EVENTOUT"      	:	15,
	"PD4:FMC_NOE"       	:	12,
	"PD4:USART2_RTS"    	:	7,
	"PD4:DFSDM1_CKIN0"    	:	6,
	"PD5:EVENTOUT"      	:	15,
	"PD5:FMC_NWE"       	:	12,
	"PD5:USART2_TX"     	:	7,
	"PD6:DCMI_D10"      	:	13,
	"PD6:EVENTOUT"      	:	15,
	"PD6:FMC_NWAIT"     	:	12,
	"PD6:I2S3_SD"       	:	5,
	"PD6:LCD_B2"        	:	14,
	"PD6:SAI1_SD_A"     	:	6,
	"PD6:SPI3_MOSI"     	:	5,
	"PD6:USART2_RX"     	:	7,
	"PD6:DFSDM1_CKIN4"     	:	3,
	"PD6:DFSDM1_DATAIN1"   	:	10,
	"PD6:SDMMC2_CK"     	:	11,
	"PD7:EVENTOUT"      	:	15,
	"PD7:SDMMC2_CMD"      	:	11,
	"PD7:FMC_NE1"       	:	12,
	"PD7:USART2_CK"     	:	7,
	"PD7:DFSDM1_DATAIN4"   	:	3,
	"PD7:SPI1_MOSI"      	:	5,
	"PD7:I2S1_SD"       	:	5,
	"PD7:DFSDM1_CKIN1"     	:	6,
	"PD7:SPDIF_RX0"     	:	8,
	"PD8:EVENTOUT"      	:	15,
	"PD8:FMC_D13"       	:	12,
	"PD8:USART3_TX"     	:	7,
	"PD8:DFSDM1_CKIN3"     	:	3,
	"PD8:SPDIF_RX1"     	:	8,
	"PD9:EVENTOUT"      	:	15,
	"PD9:FMC_D14"       	:	12,
	"PD9:USART3_RX"     	:	7,
	"PD9:DFSDM1_DATAIN3"   	:	3,
	"PE0:DCMI_D2"       	:	13,
	"PE0:EVENTOUT"      	:	15,
	"PE0:FMC_NBL0"      	:	12,
	"PE0:TIM4_ETR"      	:	2,
	"PE0:UART8_RX"      	:	8,
	"PE0:LPTIM1_ETR"      	:	3,
	"PE0:SAI2_MCK_A"      	:	10,
	"PE10:EVENTOUT"     	:	15,
	"PE10:FMC_D7"       	:	12,
	"PE10:TIM1_CH2N"    	:	1,
	"PE10:DFSDM1_DATAIN4"  	:	6,
	"PE10:UART7_CTS"       	:	8,
	"PE10:QUADSPI_BK2_IO3"	:	10,
	"PE11:EVENTOUT"     	:	15,
	"PE11:FMC_D8"       	:	12,
	"PE11:LCD_G3"       	:	14,
	"PE11:SPI4_NSS"     	:	5,
	"PE11:TIM1_CH2"     	:	1,
	"PE11:DFSDM1_CKIN4"    	:	6,
	"PE11:SAI2_SD_B"     	:	10,
	"PE12:EVENTOUT"     	:	15,
	"PE12:FMC_D9"       	:	12,
	"PE12:LCD_B4"       	:	14,
	"PE12:SPI4_SCK"     	:	5,
	"PE12:TIM1_CH3N"    	:	1,
	"PE12:DFSDM1_DATAIN5"  	:	6,
	"PE12:SAI2_SCK_B"     	:	10,
	"PE13:EVENTOUT"     	:	15,
	"PE13:FMC_D10"      	:	12,
	"PE13:LCD_DE"       	:	14,
	"PE13:SPI4_MISO"    	:	5,
	"PE13:TIM1_CH3"     	:	1,
	"PE13:DFSDM1_CKIN5"    	:	6,
	"PE13:SAI2_FS_B"     	:	10,
	"PE14:EVENTOUT"     	:	15,
	"PE14:FMC_D11"      	:	12,
	"PE14:LCD_CLK"      	:	14,
	"PE14:SPI4_MOSI"    	:	5,
	"PE14:TIM1_CH4"     	:	1,
	"PE14:SAI2_MCK_B"     	:	10,
	"PE15:EVENTOUT"     	:	15,
	"PE15:FMC_D12"      	:	12,
	"PE15:LCD_R7"       	:	14,
	"PE15:TIM1_BKIN"    	:	1,
	"PE1:DCMI_D3"       	:	13,
	"PE1:EVENTOUT"      	:	15,
	"PE1:FMC_NBL1"      	:	12,
	"PE1:UART8_TX"      	:	8,
	"PE1:LPTIM1_IN2"      	:	3,
	"PE2:ETH_MII_TXD3"  	:	11,
	"PE2:EVENTOUT"      	:	15,
	"PE2:FMC_A23"       	:	12,
	"PE2:SAI1_MCLK_A"   	:	6,
	"PE2:SPI4_SCK"      	:	5,
	"PE2:TRACECLK"      	:	0,
	"PE2:QUADSPI_BK1_IO2"  	:	9,
	"PE3:EVENTOUT"      	:	15,
	"PE3:FMC_A19"       	:	12,
	"PE3:SAI1_SD_B"     	:	6,
	"PE3:TRACED0"       	:	0,
	"PE4:DCMI_D4"       	:	13,
	"PE4:EVENTOUT"      	:	15,
	"PE4:FMC_A20"       	:	12,
	"PE4:LCD_B0"        	:	14,
	"PE4:SAI1_FS_A"     	:	6,
	"PE4:SPI4_NSS"      	:	5,
	"PE4:TRACED1"       	:	0,
	"PE4:DFSDM1_DATAIN3"   	:	10,
	"PE5:DCMI_D6"       	:	13,
	"PE5:EVENTOUT"      	:	15,
	"PE5:FMC_A21"       	:	12,
	"PE5:LCD_G0"        	:	14,
	"PE5:SAI1_SCK_A"    	:	6,
	"PE5:SPI4_MISO"     	:	5,
	"PE5:TIM9_CH1"      	:	3,
	"PE5:TRACED2"       	:	0,
	"PE5:DFSDM1_CKIN3"     	:	10,
	"PE6:DCMI_D7"       	:	13,
	"PE6:EVENTOUT"      	:	15,
	"PE6:FMC_A22"       	:	12,
	"PE6:LCD_G1"        	:	14,
	"PE6:SAI1_SD_A"     	:	6,
	"PE6:SPI4_MOSI"     	:	5,
	"PE6:TIM9_CH2"      	:	3,
	"PE6:TRACED3"       	:	0,
	"PE6:TIM1_BKIN2"      	:	1,
	"PE6:SAI2_MCK_B"       	:	10,
	"PE7:EVENTOUT"      	:	15,
	"PE7:FMC_D4"        	:	12,
	"PE7:TIM1_ETR"      	:	1,
	"PE7:UART7_RX"      	:	8,
	"PE7:DFSDM1_DATAIN2"   	:	6,
	"PE7:QUADSPI_BK2_IO0"  	:	10,
	"PE8:EVENTOUT"      	:	15,
	"PE8:FMC_D5"        	:	12,
	"PE8:TIM1_CH1N"     	:	1,
	"PE8:UART7_TX"      	:	8,
	"PE8:DFSDM1_CKIN2"     	:	6,
	"PE8:QUADSPI_BK2_IO1" 	:	10,
	"PE9:EVENTOUT"      	:	15,
	"PE9:FMC_D6"        	:	12,
	"PE9:TIM1_CH1"      	:	1,
	"PE9:DFSDM1_CKOUT"     	:	6,
	"PE9:UART7_RTS"        	:	8,
	"PE9:QUADSPI_BK2_IO2"  	:	10,
	"PF0:EVENTOUT"      	:	15,
	"PF0:FMC_A0"        	:	12,
	"PF0:I2C2_SDA"      	:	4,
	"PF10:DCMI_D11"     	:	13,
	"PF10:EVENTOUT"     	:	15,
	"PF10:QUADSPI_CLK"     	:	9,
	"PF10:LCD_DE"       	:	14,
	"PF11:DCMI_D12"     	:	13,
	"PF11:EVENTOUT"     	:	15,
	"PF11:FMC_SDNRAS"   	:	12,
	"PF11:SPI5_MOSI"    	:	5,
	"PF11:SAI2_SD_B"    	:	10,
	"PF12:EVENTOUT"     	:	15,
	"PF12:FMC_A6"       	:	12,
	"PF13:EVENTOUT"     	:	15,
	"PF13:FMC_A7"       	:	12,
	"PF13:I2C4_SMBA"     	:	4,
	"PF13:DFSDM1_DATAIN6"  	:	6,
	"PF14:EVENTOUT"     	:	15,
	"PF14:FMC_A8"       	:	12,
	"PF14:I2C4_SCL"     	:	4,
	"PF14:DFSDM1_CKIN6"  	:	6,
	"PF15:EVENTOUT"     	:	15,
	"PF15:FMC_A9"       	:	12,
	"PF15:I2C4_SDA"     	:	4,
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
	"PF6:QUADSPI_BK1_IO3"  	:	9,
	"PF6:SAI1_SD_B"     	:	6,
	"PF6:SPI5_NSS"      	:	5,
	"PF6:TIM10_CH1"     	:	3,
	"PF6:UART7_RX"      	:	8,
	"PF7:EVENTOUT"      	:	15,
	"PF7:QUADSPI_BK1_IO2"  	:	9,
	"PF7:SAI1_MCLK_B"   	:	6,
	"PF7:SPI5_SCK"      	:	5,
	"PF7:TIM11_CH1"     	:	3,
	"PF7:UART7_TX"      	:	8,
	"PF8:EVENTOUT"      	:	15,
	"PF8:QUADSPI_BK1_IO0"  	:	10,
	"PF8:SAI1_SCK_B"    	:	6,
	"PF8:SPI5_MISO"     	:	5,
	"PF8:TIM13_CH1"     	:	9,
	"PF8:UART7_RTS"     	:	8,
	"PF9:EVENTOUT"      	:	15,
	"PF9:QUADSPI_BK1_IO1"  	:	10,
	"PF9:SAI1_FS_B"     	:	6,
	"PF9:SPI5_MOSI"     	:	5,
	"PF9:TIM14_CH1"     	:	9,
	"PF9:UART7_CTS"     	:	8,
	"PG0:EVENTOUT"      	:	15,
	"PG0:FMC_A10"       	:	12,
	"PG10:DCMI_D2"      	:	13,
	"PG10:EVENTOUT"     	:	15,
	"PG10:SPI1_NSS"   	:	5,
	"PG10:FMC_NE3"      	:	12,
	"PG10:LCD_B2"       	:	14,
	"PG10:LCD_G3"       	:	9,
	"PG10:I2S1_WS"       	:	5,
	"PG10:SAI2_SD_B"       	:	10,
	"PG10:SDMMC2_D1"       	:	11,
	"PG11:DCMI_D3"      	:	13,
	"PG11:ETH_MII_TX_EN"	:	11,
	"PG11:ETH_RMII_TX_EN"	:	11,
	"PG11:EVENTOUT"     	:	15,
	"PG11:LCD_B3"       	:	14,
	"PG11:SPI1_SCK"   	:	5,
	"PG11:I2S1_CK"   	:	5,
	"PG11:SPDIF_RX0"   	:	7,
	"PG11:SDMMC2_D2"   	:	10,
	"PG12:EVENTOUT"     	:	15,
	"PG12:FMC_NE4"      	:	12,
	"PG12:LCD_B1"       	:	14,
	"PG12:LCD_B4"       	:	9,
	"PG12:SPI6_MISO"    	:	5,
	"PG12:USART6_RTS"   	:	8,
	"PG12:LPTIM1_IN1"      	:	3,
	"PG12:SPDIF_RX1"    	:	7,
	"PG12:SDMMC2_D3"   	:	11,
	"PG13:ETH_MII_TXD0" 	:	11,
	"PG13:ETH_RMII_TXD0"	:	11,
	"PG13:EVENTOUT"     	:	15,
	"PG13:FMC_A24"      	:	12,
	"PG13:SPI6_SCK"     	:	5,
	"PG13:USART6_CTS"   	:	8,
	"PG13:TRACED0"      	:	0,
	"PG13:LPTIM1_OUT"     	:	3,
	"PG13:LCD_R0"   	:	14,
	"PG14:ETH_MII_TXD1" 	:	11,
	"PG14:ETH_RMII_TXD1"	:	11,
	"PG14:EVENTOUT"     	:	15,
	"PG14:FMC_A25"      	:	12,
	"PG14:SPI6_MOSI"    	:	5,
	"PG14:USART6_TX"    	:	8,
	"PG14:QUADSPI_BK2_IO3" 	:	9,
	"PG14:LCD_B0"      	:	14,
	"PG14:TRACED1"    	:	0,
	"PG14:LPTIM1_ETR"    	:	3,
	"PG15:DCMI_D13"     	:	13,
	"PG15:EVENTOUT"     	:	15,
	"PG15:FMC_SDNCAS"   	:	12,
	"PG15:USART6_CTS"   	:	8,
	"PG1:EVENTOUT"      	:	15,
	"PG1:FMC_A11"       	:	12,
	"PG2:EVENTOUT"      	:	15,
	"PG2:FMC_A12"       	:	12,
	"PG3:EVENTOUT"      	:	15,
	"PG3:FMC_A13"       	:	12,
	"PG4:EVENTOUT"      	:	15,
	"PG4:FMC_A14"       	:	12,
	"PG4:FMC_BA0"       	:	12,
	"PG5:EVENTOUT"      	:	15,
	"PG5:FMC_A15"       	:	12,
	"PG5:FMC_BA1"       	:	12,
	"PG6:DCMI_D12"      	:	13,
	"PG6:EVENTOUT"      	:	15,
	"PG6:FMC_NE3"      	:	12,
	"PG6:LCD_R7"        	:	14,
	"PG7:DCMI_D13"      	:	13,
	"PG7:EVENTOUT"      	:	15,
	"PG7:FMC_INT"      	:	12,
	"PG7:LCD_CLK"       	:	14,
	"PG7:USART6_CK"     	:	8,
	"PG7:SAI1_MCLK_A"     	:	6,
	"PG8:ETH_PPS_OUT"   	:	11,
	"PG8:EVENTOUT"      	:	15,
	"PG8:FMC_SDCLK"     	:	12,
	"PG8:SPI6_NSS"      	:	5,
	"PG8:USART6_RTS"    	:	8,
	"PG8:SPDIF_RX2"      	:	7,
	"PG8:LCD_G7"    	:	14,
	"PG9:DCMI_VSYNC" 	:	13,
	"PG9:EVENTOUT"      	:	15,
	"PG9:FMC_NCE3"      	:	12,
	"PG9:FMC_NE2"       	:	12,
	"PG9:USART6_RX"     	:	8,
	"PG9:SPI1_MISO" 	:	5,
	"PG9:SPDIF_RX3"      	:	7,
	"PG9:QUADSPI_BK2_IO2"  	:	9,
	"PG9:SAI2_FS_B"       	:	10,
	"PG9:SDMMC2_D0"     	:	11,
	"PH0:EVENTOUT"      	:	15,
	"PH10:DCMI_D1"      	:	13,
	"PH10:EVENTOUT"     	:	15,
	"PH10:FMC_D18"      	:	12,
	"PH10:LCD_R4"       	:	14,
	"PH10:TIM5_CH1"     	:	2,
	"PH10:I2C4_SMBA"     	:	4,
	"PH11:DCMI_D2"      	:	13,
	"PH11:EVENTOUT"     	:	15,
	"PH11:FMC_D19"      	:	12,
	"PH11:LCD_R5"       	:	14,
	"PH11:TIM5_CH2"     	:	2,
	"PH11:I2C4_SCL"     	:	4,
	"PH12:DCMI_D3"      	:	13,
	"PH12:EVENTOUT"     	:	15,
	"PH12:FMC_D20"      	:	12,
	"PH12:LCD_R6"       	:	14,
	"PH12:TIM5_CH3"     	:	2,
	"PH12:I2C4_SDA"     	:	4,
	"PH13:CAN1_TX"      	:	9,
	"PH13:EVENTOUT"     	:	15,
	"PH13:FMC_D21"      	:	12,
	"PH13:LCD_G2"       	:	14,
	"PH13:TIM8_CH1N"    	:	3,
	"PH13:UART4_TX"    	:	8,
	"PH14:DCMI_D4"      	:	13,
	"PH14:EVENTOUT"     	:	15,
	"PH14:FMC_D22"      	:	12,
	"PH14:LCD_G3"       	:	14,
	"PH14:TIM8_CH2N"    	:	3,
	"PH14:UART4_RX"       	:	8,
	"PH14:CAN1_RX"    	:	9,
	"PH15:DCMI_D11"     	:	13,
	"PH15:EVENTOUT"     	:	15,
	"PH15:FMC_D23"      	:	12,
	"PH15:LCD_G4"       	:	14,
	"PH15:TIM8_CH3N"    	:	3,
	"PH1:EVENTOUT"      	:	15,
	"PH2:ETH_MII_CRS"   	:	11,
	"PH2:EVENTOUT"      	:	15,
	"PH2:FMC_SDCKE0"    	:	12,
	"PH2:LCD_R0"        	:	14,
	"PH2:LPTIM1_IN2"      	:	3,
	"PH2:QUADSPI_BK2_IO1"  	:	9,
	"PH2:SAI2_SCK_B"       	:	10,
	"PH3:ETH_MII_COL"   	:	11,
	"PH3:EVENTOUT"      	:	15,
	"PH3:FMC_SDNE0"     	:	12,
	"PH3:LCD_R1"        	:	14,
	"PH3:QUADSPI_BK2_IO1"  	:	9,
	"PH3:SAI2_MCK_B"       	:	10,
	"PH4:EVENTOUT"      	:	15,
	"PH4:I2C2_SCL"      	:	4,
	"PH4:OTG_HS_ULPI_NXT"	:	10,
	"PH4:LCD_G5"      	:	9,
	"PH4:LCD_G4"    	:	14,
	"PH5:EVENTOUT"      	:	15,
	"PH5:FMC_SDNWE"     	:	12,
	"PH5:I2C2_SDA"      	:	4,
	"PH5:SPI5_NSS"      	:	5,
	"PH6:DCMI_D8"       	:	13,
	"PH6:FMC_SDNE1"     	:	12,
	"PH6:I2C2_SMBA"     	:	4,
	"PH6:SPI5_SCK"      	:	5,
	"PH6:TIM12_CH1"     	:	9,
	"PH6:ETH_MII_RXD2"    	:	11,
	"PH6:EVENTOUT"     	:	15,
	"PH7:DCMI_D9"       	:	13,
	"PH7:ETH_MII_RXD3"  	:	11,
	"PH7:FMC_SDCKE1"    	:	12,
	"PH7:I2C3_SCL"      	:	4,
	"PH7:SPI5_MISO"     	:	5,
	"PH8:DCMI_HSYNC"    	:	13,
	"PH8:EVENTOUT"      	:	15,
	"PH8:FMC_D16"       	:	12,
	"PH8:I2C3_SDA"      	:	4,
	"PH8:LCD_R2"        	:	14,
	"PH9:DCMI_D0"       	:	13,
	"PH9:EVENTOUT"      	:	15,
	"PH9:FMC_D17"       	:	12,
	"PH9:I2C3_SMBA"     	:	4,
	"PH9:LCD_R3"        	:	14,
	"PH9:TIM12_CH2"     	:	9,
	"PI0:TIM5_CH4"     	:	2,
	"PI0:SPI2_NSS"     	:	5,
	"PI0:I2S2_WS"     	:	5,
	"PI0:FMC_D24"     	:	12,
	"PI0:DCMI_D13"     	:	13,
	"PI0:LCD_G5"     	:	14,
	"PI0:EVENTOUT"     	:	15,
	"PI1:TIM8_BKIN2"     	:	3,
	"PI1:SPI2_SCK"     	:	5,
	"PI1:I2S2_CK"     	:	5,
	"PI1:FMC_D25"     	:	12,
	"PI1:DCMI_D8"     	:	13,
	"PI1:LCD_G6"     	:	14,
	"PI1:EVENTOUT"     	:	15,
	"PI2:TIM8_CH4"     	:	3,
	"PI2:SPI2_MISO"     	:	5,
	"PI2:FMC_D26"     	:	12,
	"PI2:DCMI_D9"     	:	13,
	"PI2:LCD_G7"     	:	14,
	"PI2:EVENTOUT"     	:	15,
	"PI3:TIM8_ETR"     	:	3,
	"PI3:SPI2_MOSI"     	:	5,
	"PI3:I2S2_SD"     	:	5,
	"PI3:FMC_D27"     	:	12,
	"PI3:DCMI_D10"     	:	13,
	"PI3:EVENTOUT"     	:	15,
	"PI4:TIM8_BKIN"     	:	3,
	"PI4:SAI2_MCK_A"     	:	10,
	"PI4:FMC_NBL2"     	:	12,
	"PI4:DCMI_D5"     	:	13,
	"PI4:LCD_B4"     	:	14,
	"PI4:EVENTOUT"     	:	15,
	"PI5:TIM8_CH1"     	:	3,
	"PI5:SAI2_SCA_A"     	:	10,
	"PI5:FMC_NBL3"     	:	12,
	"PI5:DCMI_VSYNC"     	:	13,
	"PI5:LCD_B5"     	:	14,
	"PI5:EVENTOUT"     	:	15,
	"PI6:TIM8_CH2"     	:	3,
	"PI6:SAI2_SD_A"     	:	10,
	"PI6:FMC_D28"     	:	12,
	"PI6:DCMI_D6"     	:	13,
	"PI6:LCD_B6"     	:	14,
	"PI6:EVENTOUT"     	:	15,
	"PI7:TIM8_CH3"     	:	3,
	"PI7:SAI2_FS_A"     	:	10,
	"PI7:FMC_D29"     	:	12,
	"PI7:DCMI_D7"     	:	13,
	"PI7:LCD_B7"     	:	14,
	"PI7:EVENTOUT"     	:	15,
	"PI8:EVENTOUT"     	:	15,
	"PI9:UART4_RX"     	:	8,
	"PI9:CAN1_RX"     	:	9,
	"PI9:FMC_D30"     	:	12,
	"PI9:LCD_VSYNC"     	:	14,
	"PI9:EVENTOUT"     	:	15,
	"PI10:ETH_MII_RX_ER"   	:	11,
	"PI10:FMC_D31"     	:	12,
	"PI10:LCD_HSYNC"     	:	14,
	"PI11:LCD_G6"     	:	9,
	"PI11:OTG_HS_ULPI_DIR" 	:	10,
	"PI11:EVENTOUT"     	:	15,
	"PI12:LCD_HSYNC" 	:	14,
	"PI12:EVENTOUT"     	:	15,
	"PI13:LCD_VSYNC" 	:	14,
	"PI13:EVENTOUT"     	:	15,
	"PI14:LCD_CLK"  	:	14,
	"PI14:EVENTOUT"     	:	15,
	"PI15:LCD_G2"   	:	9,
	"PI15:LCD_R0"   	:	14,
	"PI15:EVENTOUT"     	:	15,
	"PJ0:LCD_R7"    	:	9,
	"PJ0:LCD_R1"    	:	14,
	"PJ0:EVENTOUT"     	:	15,
	"PJ1:LCD_R2"    	:	14,
	"PJ1:EVENTOUT"     	:	15,
	"PJ2:DSI_TE"    	:	13,
	"PJ2:LCD_R3"    	:	14,
	"PJ2:EVENTOUT"     	:	15,
	"PJ3:LCD_R4"    	:	14,
	"PJ3:EVENTOUT"     	:	15,
	"PJ4:LCD_R5"    	:	14,
	"PJ4:EVENTOUT"     	:	15,
	"PJ5:LCD_R6"    	:	14,
	"PJ5:EVENTOUT"     	:	15,
	"PJ6:LCD_R7"    	:	14,
	"PJ6:EVENTOUT"     	:	15,
	"PJ7:LCD_G0"    	:	14,
	"PJ7:EVENTOUT"     	:	15,
	"PJ8:LCD_G1"    	:	14,
	"PJ8:EVENTOUT"     	:	15,
	"PJ9:LCD_G2"    	:	14,
	"PJ9:EVENTOUT"     	:	15,
	"PJ10:LCD_G3"    	:	14,
	"PJ10:EVENTOUT"     	:	15,
	"PJ11:LCD_G4"    	:	14,
	"PJ11:EVENTOUT"     	:	15,
	"PJ12:LCD_G3"    	:	9,
	"PJ12:LCD_B0"    	:	14,
	"PJ12:EVENTOUT"     	:	15,
	"PJ12:LCD_G4"    	:	9,
	"PJ13:LCD_B1"    	:	14,
	"PJ13:EVENTOUT"     	:	15,
	"PJ14:LCD_B2"    	:	14,
	"PJ14:EVENTOUT"     	:	15,
	"PJ15:LCD_B3"    	:	14,
	"PJ15:EVENTOUT"     	:	15,
	"PK0:LCD_G5"    	:	14,
	"PK0:EVENTOUT"     	:	15,
	"PK1:LCD_G6"    	:	14,
	"PK1:EVENTOUT"     	:	15,
	"PK2:LCD_G7"    	:	14,
	"PK2:EVENTOUT"     	:	15,
	"PK3:LCD_B4"    	:	14,
	"PK3:EVENTOUT"     	:	15,
	"PK4:LCD_B5"    	:	14,
	"PK4:EVENTOUT"     	:	15,
	"PK5:LCD_B6"    	:	14,
	"PK5:EVENTOUT"     	:	15,
	"PK6:LCD_B7"    	:	14,
	"PK6:EVENTOUT"     	:	15,
	"PK7:LCD_DE"    	:	14,
	"PK7:EVENTOUT"     	:	15,
}

ADC1_map = {
	# format is PIN : ADC1_CHAN
	"PA0"	:	0,
	"PA1"	:	1,
	"PA2"	:	2,
	"PA3"	:	3,
	"PA4"	:	4,
	"PA5"	:	5,
	"PA6"	:	6,
	"PA7"	:	7,
	"PB0"	:	8,
	"PB1"	:	9,
	"PC0"	:	10,
	"PC1"	:	11,
	"PC2"	:	12,
	"PC3"	:	13,
	"PC4"	:	14,
	"PC5"	:	15,
}
