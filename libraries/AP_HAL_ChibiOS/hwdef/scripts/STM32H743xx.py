#!/usr/bin/env python
'''
these tables are generated from the STM32 datasheets for the STM32H743bi
'''

# additional build information for ChibiOS
build = {
    "CHIBIOS_STARTUP_MK"  : "os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32h7xx.mk",
    "CHIBIOS_PLATFORM_MK" : "os/hal/ports/STM32/STM32H7xx/platform.mk"
    }

# MCU parameters
mcu = {
    # DMA peripheral capabilities:
    # - can't use ITCM or DTCM for any DMA
    # - SPI1 to SPI5 can use AXI SRAM, SRAM1 to SRAM3 and SRAM4 for DMA
    # - SPI6, I2C4 and ADC3 can use SRAM4 on BDMA
    # - UARTS can use AXI SRAM, SRAM1 to SRAM3 and SRAM4 for DMA
    # - I2C1, I2C2 and I2C3 can use AXI SRAM, SRAM1 to SRAM3 and SRAM4 with DMA
    # - timers can use AXI SRAM, SRAM1 to SRAM3 and SRAM4 with DMA
    # - ADC12 can use AXI SRAM, SRAM1 to SRAM3 and SRAM4
    # - SDMMC can use AXI SRAM, SRAM1 to SRAM3 with IDMA (cannot use SRAM4)

    # ram map, as list of (address, size-kb, flags)
    # flags of 1 means DMA-capable (DMA and BDMA)
    # flags of 2 means faster memory for CPU intensive work
    # flags of 4 means memory can be used for SDMMC DMA
    'RAM_MAP' : [
        (0x30000000, 256, 0), # SRAM1, SRAM2
        (0x20000000, 128, 2), # DTCM, tightly coupled, no DMA, fast
        (0x24000000, 512, 4), # AXI SRAM. Use this for SDMMC IDMA ops
        (0x00000400,  63, 2), # ITCM (first 1k removed, to keep address 0 unused)
        (0x30040000,  32, 0), # SRAM3.
        (0x38000000,  64, 1), # SRAM4.
    ],

    'EXPECTED_CLOCK' : 400000000,

    # this MCU has M7 instructions and hardware double precision
    'CORTEX'    : 'cortex-m7',
    'CPU_FLAGS' : '-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard',

    'DEFINES' : {
        'HAL_HAVE_HARDWARE_DOUBLE' : '1'
    }
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
    'J': 16,
    'K': 16
}
    
# no DMA map as we will dynamically allocate DMA channels using the DMAMUX
DMA_Map = None

AltFunction_map = {
	# format is PIN:FUNCTION : AFNUM
	# extracted from tabula-AF-H743.csv
	"PA0:ETH_MII_CRS"   	:	11,
	"PA0:EVENT-OUT"     	:	15,
	"PA0:SAI2_SD_B"     	:	10,
	"PA0:SDMMC2_CMD"    	:	9,
	"PA0:TIM15_BKIN"    	:	4,
	"PA0:TIM2_CH1"      	:	1,
	"PA0:TIM2_ETR"      	:	1,
	"PA0:TIM5_CH1"      	:	2,
	"PA0:TIM8_ETR"      	:	3,
	"PA0:UART4_TX"      	:	8,
	"PA0:USART2_CTS_NSS"	:	7,
	"PA1:ETH_MII_RX_CLK"	:	11,
	"PA1:ETH_RMII_REF_CLK"	:	11,
	"PA1:EVENT-OUT"     	:	15,
	"PA1:LCD_R2"        	:	14,
	"PA1:LPTIM3_OUT"    	:	3,
	"PA1:QUADSPI_BK1_IO3"	:	9,
	"PA1:SAI2_MCK_B"    	:	10,
	"PA1:TIM15_CH1N"    	:	4,
	"PA1:TIM2_CH2"      	:	1,
	"PA1:TIM5_CH2"      	:	2,
	"PA1:UART4_RX"      	:	8,
	"PA1:USART2_RTS"    	:	7,
	"PA2:ETH_MDIO"      	:	11,
	"PA2:EVENT-OUT"     	:	15,
	"PA2:LCD_R1"        	:	14,
	"PA2:LPTIM4_OUT"    	:	3,
	"PA2:MDIOS_MDIO"    	:	12,
	"PA2:SAI2_SCK_B"    	:	8,
	"PA2:TIM15_CH1"     	:	4,
	"PA2:TIM2_CH3"      	:	1,
	"PA2:TIM5_CH3"      	:	2,
	"PA2:USART2_TX"     	:	7,
	"PA3:ETH_MII_COL"   	:	11,
	"PA3:EVENT-OUT"     	:	15,
	"PA3:LCD_B2"        	:	9,
	"PA3:LCD_B5"        	:	14,
	"PA3:LPTIM5_OUT"    	:	3,
	"PA3:OTG_HS_ULPI_D0"	:	10,
	"PA3:TIM15_CH2"     	:	4,
	"PA3:TIM2_CH4"      	:	1,
	"PA3:TIM5_CH4"      	:	2,
	"PA3:USART2_RX"     	:	7,
	"PA4:DCMI_HSYNC"    	:	13,
	"PA4:EVENT-OUT"     	:	15,
	"PA4:I2S1_WS"       	:	5,
	"PA4:I2S3_WS"       	:	6,
	"PA4:LCD_VSYNC"     	:	14,
	"PA4:OTG_HS_SOF"    	:	12,
	"PA4:SPI1_NSS"      	:	5,
	"PA4:SPI3_NSS"      	:	6,
	"PA4:SPI6_NSS"      	:	8,
	"PA4:TIM5_ETR"      	:	2,
	"PA4:USART2_CK"     	:	7,
	"PA5:EVENT-OUT"     	:	15,
	"PA5:I2S1_CK"       	:	5,
	"PA5:LCD_R4"        	:	14,
	"PA5:OTG_HS_ULPI_CK"	:	10,
	"PA5:SPI1_SCK"      	:	5,
	"PA5:SPI6_SCK"      	:	8,
	"PA5:TIM2_CH1"      	:	1,
	"PA5:TIM2_ETR"      	:	1,
	"PA5:TIM8_CH1N"     	:	3,
	"PA6:DCMI_PIXCLK"   	:	13,
	"PA6:EVENT-OUT"     	:	15,
	"PA6:I2S1_SDI"      	:	5,
	"PA6:LCD_G2"        	:	14,
	"PA6:MDIOS_MDC"     	:	11,
	"PA6:SPI1_MISO"     	:	5,
	"PA6:SPI6_MISO"     	:	8,
	"PA6:TIM13_CH1"     	:	9,
	"PA6:TIM1_BKIN"     	:	1,
	"PA6:TIM1_BKIN_COMP12"	:	12,
	"PA6:TIM3_CH1"      	:	2,
	"PA6:TIM8_BKIN"     	:	3,
	"PA6:TIM8_BKIN_COMP12"	:	10,
	"PA7:ETH_MII_RX_DV" 	:	11,
	"PA7:ETH_RMII_CRS_DV"	:	11,
	"PA7:EVENT-OUT"     	:	15,
	"PA7:FMC_SDNWE"     	:	12,
	"PA7:I2S1_SDO"      	:	5,
	"PA7:SPI1_MOSI"     	:	5,
	"PA7:SPI6_MOSI"     	:	8,
	"PA7:TIM14_CH1"     	:	9,
	"PA7:TIM1_CH1N"     	:	1,
	"PA7:TIM3_CH2"      	:	2,
	"PA7:TIM8_CH1N"     	:	3,
	"PA8:EVENT-OUT"     	:	15,
	"PA8:HRTIM_CHB2"    	:	2,
	"PA8:I2C3_SCL"      	:	4,
	"PA8:LCD_B3"        	:	13,
	"PA8:LCD_R6"        	:	14,
	"PA8:MCO1"          	:	0,
	"PA8:OTG_FS_SOF"    	:	10,
	"PA8:TIM1_CH1"      	:	1,
	"PA8:TIM8_BKIN2"    	:	3,
	"PA8:TIM8_BKIN2_COMP12"	:	12,
	"PA8:UART7_RX"      	:	11,
	"PA8:USART1_CK"     	:	7,
	"PA9:DCMI_D0"       	:	13,
	"PA9:EVENT-OUT"     	:	15,
    "PA9:CAN1_RX"			:	9,
	"PA9:HRTIM_CHC1"    	:	2,
	"PA9:I2C3_SMBA"     	:	4,
	"PA9:I2S2_CK"       	:	5,
	"PA9:LCD_R5"        	:	14,
	"PA9:LPUART1_TX"    	:	3,
	"PA9:SPI2_SCK"      	:	5,
	"PA9:TIM1_CH2"      	:	1,
	"PA9:USART1_TX"     	:	7,
	"PA10:DCMI_D1"      	:	13,
	"PA10:EVENT-OUT"    	:	15,
    "PA10:CAN1_TX"			:	9,
	"PA10:HRTIM_CHC2"   	:	2,
	"PA10:LCD_B1"       	:	14,
	"PA10:LCD_B4"       	:	12,
	"PA10:LPUART1_RX"   	:	3,
	"PA10:MDIOS_MDIO"   	:	11,
	"PA10:OTG_FS_ID"    	:	10,
	"PA10:TIM1_CH3"     	:	1,
	"PA10:USART1_RX"    	:	7,
	"PA11:EVENT-OUT"    	:	15,
    "PA11:CAN1_RX"    		:	9,
	"PA11:HRTIM_CHD1"   	:	2,
	"PA11:I2S2_WS"      	:	5,
	"PA11:LCD_R4"       	:	14,
	"PA11:LPUART1_CTS"  	:	3,
	"PA11:OTG_FS_DM"    	:	10,
	"PA11:SPI2_NSS"     	:	5,
	"PA11:TIM1_CH4"     	:	1,
	"PA11:UART4_RX"     	:	6,
	"PA11:USART1_CTS_NSS"	:	7,
	"PA12:EVENT-OUT"    	:	15,
    "PA12:CAN1_TX"    		:	9,
	"PA12:HRTIM_CHD2"   	:	2,
	"PA12:I2S2_CK"      	:	5,
	"PA12:LCD_R5"       	:	14,
	"PA12:LPUART1_RTS"  	:	3,
	"PA12:OTG_FS_DP"    	:	10,
	"PA12:SAI2_FS_B"    	:	8,
	"PA12:SPI2_SCK"     	:	5,
	"PA12:TIM1_ETR"     	:	1,
	"PA12:UART4_TX"     	:	6,
	"PA12:USART1_RTS"   	:	7,
	"PA13:EVENT-OUT"    	:	15,
	"PA13:JTMS-SWDIO"   	:	0,
	"PA14:EVENT-OUT"    	:	15,
	"PA14:JTCK-SWCLK"   	:	0,
	"PA15:EVENT-OUT"    	:	15,
	"PA15:HDMI_CEC"     	:	4,
	"PA15:HRTIM_FLT1"   	:	2,
	"PA15:I2S1_WS"      	:	5,
	"PA15:I2S3_WS"      	:	6,
	"PA15:JTDI"         	:	0,
	"PA15:SPI1_NSS"     	:	5,
	"PA15:SPI3_NSS"     	:	6,
	"PA15:SPI6_NSS"     	:	7,
	"PA15:TIM2_CH1"     	:	1,
	"PA15:TIM2_ETR"     	:	1,
	"PA15:UART4_RTS"    	:	8,
	"PA15:UART7_TX"     	:	11,
	"PB0:DFSDM_CKOUT"   	:	6,
	"PB0:ETH_MII_RXD2"  	:	11,
	"PB0:EVENT-OUT"     	:	15,
	"PB0:LCD_G1"        	:	14,
	"PB0:LCD_R3"        	:	9,
	"PB0:OTG_HS_ULPI_D1"	:	10,
	"PB0:TIM1_CH2N"     	:	1,
	"PB0:TIM3_CH3"      	:	2,
	"PB0:TIM8_CH2N"     	:	3,
	"PB0:UART4_CTS"     	:	8,
	"PB1:DFSDM_DATIN1"  	:	6,
	"PB1:ETH_MII_RXD3"  	:	11,
	"PB1:EVENT-OUT"     	:	15,
	"PB1:LCD_G0"        	:	14,
	"PB1:LCD_R6"        	:	9,
	"PB1:OTG_HS_ULPI_D2"	:	10,
	"PB1:TIM1_CH3N"     	:	1,
	"PB1:TIM3_CH4"      	:	2,
	"PB1:TIM8_CH3N"     	:	3,
	"PB2:DFSDM_CKIN1"   	:	4,
	"PB2:EVENT-OUT"     	:	15,
	"PB2:I2S3_SDO"      	:	7,
	"PB2:QUADSPI_CLK"   	:	9,
	"PB2:SAI1_D1"       	:	2,
	"PB2:SAI1_SD_A"     	:	6,
	"PB2:SAI4_D1"       	:	10,
	"PB2:SAI4_SD_A"     	:	8,
	"PB2:SPI3_MOSI"     	:	7,
	"PB3:EVENT-OUT"     	:	15,
	"PB3:HRTIM_FLT4"    	:	2,
	"PB3:I2S1_CK"       	:	5,
	"PB3:I2S3_CK"       	:	6,
	"PB3:JTDO"          	:	0,
	"PB3:SDMMC2_D2"     	:	9,
	"PB3:SPI1_SCK"      	:	5,
	"PB3:SPI3_SCK"      	:	6,
	"PB3:SPI6_SCK"      	:	8,
	"PB3:TIM2_CH2"      	:	1,
	"PB3:TRACESWO"      	:	0,
	"PB3:UART7_RX"      	:	11,
	"PB4:EVENT-OUT"     	:	15,
	"PB4:HRTIM_EEV6"    	:	3,
	"PB4:I2S1_SDI"      	:	5,
	"PB4:I2S2_WS"       	:	7,
	"PB4:I2S3_SDI"      	:	6,
	"PB4:NJTRST"        	:	0,
	"PB4:SDMMC2_D3"     	:	9,
	"PB4:SPI1_MISO"     	:	5,
	"PB4:SPI2_NSS"      	:	7,
	"PB4:SPI3_MISO"     	:	6,
	"PB4:SPI6_MISO"     	:	8,
	"PB4:TIM16_BKIN"    	:	1,
	"PB4:TIM3_CH1"      	:	2,
	"PB4:UART7_TX"      	:	11,
	"PB5:DCMI_D10"      	:	13,
	"PB5:ETH_PPS_OUT"   	:	11,
	"PB5:EVENT-OUT"     	:	15,
    "PB5:CAN2_RX"	     	:	9,
	"PB5:FMC_SDCKE1"    	:	12,
	"PB5:HRTIM_EEV7"    	:	3,
	"PB5:I2C1_SMBA"     	:	4,
	"PB5:I2C4_SMBA"     	:	6,
	"PB5:I2S1_SDO"      	:	5,
	"PB5:I2S3_SDO"      	:	7,
	"PB5:OTG_HS_ULPI_D7"	:	10,
	"PB5:SPI1_MOSI"     	:	5,
	"PB5:SPI3_MOSI"     	:	7,
	"PB5:SPI6_MOSI"     	:	8,
	"PB5:TIM17_BKIN"    	:	1,
	"PB5:TIM3_CH2"      	:	2,
	"PB5:UART5_RX"      	:	14,
	"PB6:DCMI_D5"       	:	13,
	"PB6:DFSDM_DATIN5"  	:	11,
	"PB6:EVENT-OUT"     	:	15,
    "PB6:CAN2_TX"   	  	:	9,
	"PB6:FMC_SDNE1"     	:	12,
	"PB6:HDMI_CEC"      	:	5,
	"PB6:HRTIM_EEV8"    	:	3,
	"PB6:I2C1_SCL"      	:	4,
	"PB6:I2C4_SCL"      	:	6,
	"PB6:LPUART1_TX"    	:	8,
	"PB6:QUADSPI_BK1_NCS"	:	10,
	"PB6:TIM16_CH1N"    	:	1,
	"PB6:TIM4_CH1"      	:	2,
	"PB6:UART5_TX"      	:	14,
	"PB6:USART1_TX"     	:	7,
	"PB7:DCMI_VSYNC"    	:	13,
	"PB7:DFSDM_CKIN5"   	:	11,
	"PB7:EVENT-OUT"     	:	15,
    "PB7:CAN2_TX"			:	9,
	"PB7:FMC_NL"        	:	12,
	"PB7:HRTIM_EEV9"    	:	3,
	"PB7:I2C1_SDA"      	:	4,
	"PB7:I2C4_SDA"      	:	6,
	"PB7:LPUART1_RX"    	:	8,
	"PB7:TIM17_CH1N"    	:	1,
	"PB7:TIM4_CH2"      	:	2,
	"PB7:USART1_RX"     	:	7,
	"PB8:DCMI_D6"       	:	13,
	"PB8:DFSDM_CKIN7"   	:	3,
	"PB8:ETH_MII_TXD3"  	:	11,
	"PB8:EVENT-OUT"     	:	15,
    "PB8:CAN1_RX"	     	:	9,
	"PB8:I2C1_SCL"      	:	4,
	"PB8:I2C4_SCL"      	:	6,
	"PB8:LCD_B6"        	:	14,
	"PB8:SDMMC1_CKIN"   	:	7,
	"PB8:SDMMC1_D4"     	:	12,
	"PB8:SDMMC2_D4"     	:	10,
	"PB8:TIM16_CH1"     	:	1,
	"PB8:TIM4_CH3"      	:	2,
	"PB8:UART4_RX"      	:	8,
	"PB9:DCMI_D7"       	:	13,
	"PB9:DFSDM_DATIN7"  	:	3,
	"PB9:EVENT-OUT"     	:	15,
    "PB9:CAN1_TX"   	  	:	9,
	"PB9:I2C1_SDA"      	:	4,
	"PB9:I2C4_SDA"      	:	6,
	"PB9:I2C4_SMBA"     	:	11,
	"PB9:I2S2_WS"       	:	5,
	"PB9:LCD_B7"        	:	14,
	"PB9:SDMMC1_CDIR"   	:	7,
	"PB9:SDMMC1_D5"     	:	12,
	"PB9:SDMMC2_D5"     	:	10,
	"PB9:SPI2_NSS"      	:	5,
	"PB9:TIM17_CH1"     	:	1,
	"PB9:TIM4_CH4"      	:	2,
	"PB9:UART4_TX"      	:	8,
	"PB10:DFSDM_DATIN7" 	:	6,
	"PB10:ETH_MII_RX_ER"	:	11,
	"PB10:EVENT-OUT"    	:	15,
	"PB10:HRTIM_SCOUT"  	:	2,
	"PB10:I2C2_SCL"     	:	4,
	"PB10:I2S2_CK"      	:	5,
	"PB10:LCD_G4"       	:	14,
	"PB10:LPTIM2_IN1"   	:	3,
	"PB10:OTG_HS_ULPI_D3"	:	10,
	"PB10:QUADSPI_BK1_NCS"	:	9,
	"PB10:SPI2_SCK"     	:	5,
	"PB10:TIM2_CH3"     	:	1,
	"PB10:USART3_TX"    	:	7,
	"PB11:DFSDM_CKIN7"  	:	6,
	"PB11:ETH_MII_TX_EN"	:	11,
	"PB11:ETH_RMII_TX_EN"	:	11,
	"PB11:EVENT-OUT"    	:	15,
	"PB11:HRTIM_SCIN"   	:	2,
	"PB11:I2C2_SDA"     	:	4,
	"PB11:LCD_G5"       	:	14,
	"PB11:LPTIM2_ETR"   	:	3,
	"PB11:OTG_HS_ULPI_D4"	:	10,
	"PB11:TIM2_CH4"     	:	1,
	"PB11:USART3_RX"    	:	7,
	"PB12:DFSDM_DATIN1" 	:	6,
	"PB12:ETH_MII_TXD0" 	:	11,
	"PB12:ETH_RMII_TXD0"	:	11,
	"PB12:EVENT-OUT"    	:	15,
    "PB12:CAN2_RX"	    	:	9,
	"PB12:I2C2_SMBA"    	:	4,
	"PB12:I2S2_WS"      	:	5,
	"PB12:OTG_HS_ID"    	:	12,
	"PB12:OTG_HS_ULPI_D5"	:	10,
	"PB12:SPI2_NSS"     	:	5,
	"PB12:TIM1_BKIN"    	:	1,
	"PB12:TIM1_BKIN_COMP12"	:	13,
	"PB12:UART5_RX"     	:	14,
	"PB12:USART3_CK"    	:	7,
	"PB13:DFSDM_CKIN1"  	:	6,
	"PB13:ETH_MII_TXD1" 	:	11,
	"PB13:ETH_RMII_TXD1"	:	11,
	"PB13:EVENT-OUT"    	:	15,
    "PB13:CAN2_TX"    		:	9,
	"PB13:I2S2_CK"      	:	5,
	"PB13:LPTIM2_OUT"   	:	3,
	"PB13:OTG_HS_ULPI_D6"	:	10,
	"PB13:SPI2_SCK"     	:	5,
	"PB13:TIM1_CH1N"    	:	1,
	"PB13:UART5_TX"     	:	14,
	"PB13:USART3_CTS_NSS"	:	7,
	"PB14:DFSDM_DATIN2" 	:	6,
	"PB14:EVENT-OUT"    	:	15,
	"PB14:I2S2_SDI"     	:	5,
	"PB14:OTG_HS_DM"    	:	12,
	"PB14:SDMMC2_D0"    	:	9,
	"PB14:SPI2_MISO"    	:	5,
	"PB14:TIM12_CH1"    	:	2,
	"PB14:TIM1_CH2N"    	:	1,
	"PB14:TIM8_CH2N"    	:	3,
	"PB14:UART4_RTS"    	:	8,
	"PB14:USART1_TX"    	:	4,
	"PB14:USART3_RTS"   	:	7,
	"PB15:DFSDM_CKIN2"  	:	6,
	"PB15:EVENT-OUT"    	:	15,
	"PB15:I2S2_SDO"     	:	5,
	"PB15:OTG_HS_DP"    	:	12,
	"PB15:RTC_REFIN"    	:	0,
	"PB15:SDMMC2_D1"    	:	9,
	"PB15:SPI2_MOSI"    	:	5,
	"PB15:TIM12_CH2"    	:	2,
	"PB15:TIM1_CH3N"    	:	1,
	"PB15:TIM8_CH3N"    	:	3,
	"PB15:UART4_CTS"    	:	8,
	"PB15:USART1_RX"    	:	4,
	"PC0:DFSDM_CKIN0"   	:	3,
	"PC0:DFSDM_DATIN4"  	:	6,
	"PC0:EVENT-OUT"     	:	15,
	"PC0:FMC_SDNWE"     	:	12,
	"PC0:LCD_R5"        	:	14,
	"PC0:OTG_HS_ULPI_STP"	:	10,
	"PC0:SAI2_FS_B"     	:	8,
	"PC1:DFSDM_CKIN4"   	:	4,
	"PC1:DFSDM_DATIN0"  	:	3,
	"PC1:ETH_MDC"       	:	11,
	"PC1:EVENT-OUT"     	:	15,
	"PC1:I2S2_SDO"      	:	5,
	"PC1:MDIOS_MDC"     	:	12,
	"PC1:SAI1_D1"       	:	2,
	"PC1:SAI1_SD_A"     	:	6,
	"PC1:SAI4_D1"       	:	10,
	"PC1:SAI4_SD_A"     	:	8,
	"PC1:SDMMC2_CK"     	:	9,
	"PC1:SPI2_MOSI"     	:	5,
	"PC1:TRACED0"       	:	0,
	"PC2:DFSDM_CKIN1"   	:	3,
	"PC2:DFSDM_CKOUT"   	:	6,
	"PC2:ETH_MII_TXD2"  	:	11,
	"PC2:EVENT-OUT"     	:	15,
	"PC2:FMC_SDNE0"     	:	12,
	"PC2:I2S2_SDI"      	:	5,
	"PC2:OTG_HS_ULPI_DIR"	:	10,
	"PC2:SPI2_MISO"     	:	5,
	"PC3:DFSDM_DATIN1"  	:	3,
	"PC3:ETH_MII_TX_CLK"	:	11,
	"PC3:EVENT-OUT"     	:	15,
	"PC3:FMC_SDCKE0"    	:	12,
	"PC3:I2S2_SDO"      	:	5,
	"PC3:OTG_HS_ULPI_NXT"	:	10,
	"PC3:SPI2_MOSI"     	:	5,
	"PC4:DFSDM_CKIN2"   	:	3,
	"PC4:ETH_MII_RXD0"  	:	11,
	"PC4:ETH_RMII_RXD0" 	:	11,
	"PC4:EVENT-OUT"     	:	15,
	"PC4:FMC_SDNE0"     	:	12,
	"PC4:I2S1_MCK"      	:	5,
	"PC4:SPDIFRX_IN2"   	:	9,
	"PC5:COMP_1_OUT"    	:	13,
	"PC5:DFSDM_DATIN2"  	:	3,
	"PC5:ETH_MII_RXD1"  	:	11,
	"PC5:ETH_RMII_RXD1" 	:	11,
	"PC5:EVENT-OUT"     	:	15,
	"PC5:FMC_SDCKE0"    	:	12,
	"PC5:SAI1_D3"       	:	2,
	"PC5:SAI4_D3"       	:	10,
	"PC5:SPDIFRX_IN3"   	:	9,
	"PC6:DCMI_D0"       	:	13,
	"PC6:DFSDM_CKIN3"   	:	4,
	"PC6:EVENT-OUT"     	:	15,
	"PC6:FMC_NWAIT"     	:	9,
	"PC6:HRTIM_CHA1"    	:	1,
	"PC6:I2S2_MCK"      	:	5,
	"PC6:LCD_HSYNC"     	:	14,
	"PC6:SDMMC1_D0DIR"  	:	8,
	"PC6:SDMMC1_D6"     	:	12,
	"PC6:SDMMC2_D6"     	:	10,
	"PC6:TIM3_CH1"      	:	2,
	"PC6:TIM8_CH1"      	:	3,
	"PC6:USART6_TX"     	:	7,
	"PC7:DCMI_D1"       	:	13,
	"PC7:DFSDM_DATIN3"  	:	4,
	"PC7:EVENT-OUT"     	:	15,
	"PC7:FMC_NE1"       	:	9,
	"PC7:HRTIM_CHA2"    	:	1,
	"PC7:I2S3_MCK"      	:	6,
	"PC7:LCD_G6"        	:	14,
	"PC7:SDMMC1_D123DIR"	:	8,
	"PC7:SDMMC1_D7"     	:	12,
	"PC7:SDMMC2_D7"     	:	10,
	"PC7:SWPMI_TX"      	:	11,
	"PC7:TIM3_CH2"      	:	2,
	"PC7:TIM8_CH2"      	:	3,
	"PC7:TRGIO"         	:	0,
	"PC7:USART6_RX"     	:	7,
	"PC8:DCMI_D2"       	:	13,
	"PC8:EVENT-OUT"     	:	15,
	"PC8:FMC_NCE"       	:	9,
	"PC8:FMC_NE2"       	:	9,
	"PC8:HRTIM_CHB1"    	:	1,
	"PC8:SDMMC1_D0"     	:	12,
	"PC8:SWPMI_RX"      	:	11,
	"PC8:TIM3_CH3"      	:	2,
	"PC8:TIM8_CH3"      	:	3,
	"PC8:TRACED1"       	:	0,
	"PC8:UART5_RTS"     	:	8,
	"PC8:USART6_CK"     	:	7,
	"PC9:DCMI_D3"       	:	13,
	"PC9:EVENT-OUT"     	:	15,
	"PC9:I2C3_SDA"      	:	4,
	"PC9:I2S_CKIN"      	:	5,
	"PC9:LCD_B2"        	:	14,
	"PC9:LCD_G3"        	:	10,
	"PC9:MCO2"          	:	0,
	"PC9:QUADSPI_BK1_IO0"	:	9,
	"PC9:SDMMC1_D1"     	:	12,
	"PC9:SWPMI_SUSPEND" 	:	11,
	"PC9:TIM3_CH4"      	:	2,
	"PC9:TIM8_CH4"      	:	3,
	"PC9:UART5_CTS"     	:	8,
	"PC10:DCMI_D8"      	:	13,
	"PC10:DFSDM_CKIN5"  	:	3,
	"PC10:EVENT-OUT"    	:	15,
	"PC10:HRTIM_EEV1"   	:	2,
	"PC10:I2S3_CK"      	:	6,
	"PC10:LCD_R2"       	:	14,
	"PC10:QUADSPI_BK1_IO1"	:	9,
	"PC10:SDMMC1_D2"    	:	12,
	"PC10:SPI3_SCK"     	:	6,
	"PC10:UART4_TX"     	:	8,
	"PC10:USART3_TX"    	:	7,
	"PC11:DCMI_D4"      	:	13,
	"PC11:DFSDM_DATIN5" 	:	3,
	"PC11:EVENT-OUT"    	:	15,
	"PC11:HRTIM_FLT2"   	:	2,
	"PC11:I2S3_SDI"     	:	6,
	"PC11:QUADSPI_BK2_NCS"	:	9,
	"PC11:SDMMC1_D3"    	:	12,
	"PC11:SPI3_MISO"    	:	6,
	"PC11:UART4_RX"     	:	8,
	"PC11:USART3_RX"    	:	7,
	"PC12:DCMI_D9"      	:	13,
	"PC12:EVENT-OUT"    	:	15,
	"PC12:HRTIM_EEV2"   	:	2,
	"PC12:I2S3_SDO"     	:	6,
	"PC12:SDMMC1_CK"    	:	12,
	"PC12:SPI3_MOSI"    	:	6,
	"PC12:TRACED3"      	:	0,
	"PC12:UART5_TX"     	:	8,
	"PC12:USART3_CK"    	:	7,
	"PC13:EVENT-OUT"    	:	15,
	"PC14:EVENT-OUT"    	:	15,
	"PC15:EVENT-OUT"    	:	15,
	"PD0:DFSDM_CKIN6"   	:	3,
	"PD0:EVENT-OUT"     	:	15,
    "PD0:CAN1_RX"	     	:	9,
	"PD0:FMC_D2"        	:	12,
	"PD0:FMC_DA2"       	:	12,
	"PD0:SAI3_SCK_A"    	:	6,
	"PD0:UART4_RX"      	:	8,
	"PD1:DFSDM_DATIN6"  	:	3,
	"PD1:EVENT-OUT"     	:	15,
    "PD1:CAN1_TX"   	  	:	9,
	"PD1:FMC_D3"        	:	12,
	"PD1:FMC_DA3"       	:	12,
	"PD1:SAI3_SD_A"     	:	6,
	"PD1:UART4_TX"      	:	8,
	"PD2:DCMI_D11"      	:	13,
	"PD2:EVENT-OUT"     	:	15,
	"PD2:SDMMC1_CMD"    	:	12,
	"PD2:TIM3_ETR"      	:	2,
	"PD2:TRACED2"       	:	0,
	"PD2:UART5_RX"      	:	8,
	"PD3:DCMI_D5"       	:	13,
	"PD3:DFSDM_CKOUT"   	:	3,
	"PD3:EVENT-OUT"     	:	15,
	"PD3:FMC_CLK"       	:	12,
	"PD3:I2S2_CK"       	:	5,
	"PD3:LCD_G7"        	:	14,
	"PD3:SPI2_SCK"      	:	5,
    "PD3:USART2_CTS"		:	7,
	"PD4:EVENT-OUT"     	:	15,
    "PD4:CAN1_RX"			:	9,
	"PD4:FMC_NOE"       	:	12,
	"PD4:HRTIM_FLT3"    	:	2,
	"PD4:SAI3_FS_A"     	:	6,
	"PD4:USART2_RTS"    	:	7,
	"PD5:EVENT-OUT"     	:	15,
    "PD5:CAN1_TX"			:	9,
	"PD5:FMC_NWE"       	:	12,
	"PD5:HRTIM_EEV3"    	:	2,
	"PD5:USART2_TX"     	:	7,
	"PD6:DCMI_D10"      	:	13,
	"PD6:DFSDM_CKIN4"   	:	3,
	"PD6:DFSDM_DATIN1"  	:	4,
	"PD6:EVENT-OUT"     	:	15,
    "PD6:CAN2_RX"			:	9,
	"PD6:FMC_NWAIT"     	:	12,
	"PD6:I2S3_SDO"      	:	5,
	"PD6:LCD_B2"        	:	14,
	"PD6:SAI1_D1"       	:	2,
	"PD6:SAI1_SD_A"     	:	6,
	"PD6:SAI4_D1"       	:	10,
	"PD6:SAI4_SD_A"     	:	8,
	"PD6:SDMMC2_CK"     	:	11,
	"PD6:SPI3_MOSI"     	:	5,
	"PD6:USART2_RX"     	:	7,
	"PD7:DFSDM_CKIN1"   	:	6,
	"PD7:DFSDM_DATIN4"  	:	3,
	"PD7:EVENT-OUT"     	:	15,
	"PD7:FMC_NE1"       	:	12,
	"PD7:I2S1_SDO"      	:	5,
	"PD7:SDMMC2_CMD"    	:	11,
	"PD7:SPDIFRX_IN0"   	:	9,
	"PD7:SPI1_MOSI"     	:	5,
	"PD7:USART2_CK"     	:	7,
	"PD8:DFSDM_CKIN3"   	:	3,
	"PD8:EVENT-OUT"     	:	15,
	"PD8:FMC_D13"       	:	12,
	"PD8:FMC_DA13"      	:	12,
	"PD8:SAI3_SCK_B"    	:	6,
	"PD8:SPDIFRX_IN1"   	:	9,
	"PD8:USART3_TX"     	:	7,
	"PD9:DFSDM_DATIN3"  	:	3,
	"PD9:EVENT-OUT"     	:	15,
    "PD9:CAN2_RX"			:	9,
	"PD9:FMC_D14"       	:	12,
	"PD9:FMC_DA14"      	:	12,
	"PD9:SAI3_SD_B"     	:	6,
	"PD9:USART3_RX"     	:	7,
	"PD10:DFSDM_CKOUT"  	:	3,
	"PD10:EVENT-OUT"    	:	15,
    "PD10:CAN2_TX"			:	9,
	"PD10:FMC_D15"      	:	12,
	"PD10:FMC_DA15"     	:	12,
	"PD10:LCD_B3"       	:	14,
	"PD10:SAI3_FS_B"    	:	6,
	"PD10:USART3_CK"    	:	7,
	"PD11:EVENT-OUT"    	:	15,
	"PD11:FMC_A16"      	:	12,
	"PD11:I2C4_SMBA"    	:	4,
	"PD11:LPTIM2_IN2"   	:	3,
	"PD11:QUADSPI_BK1_IO0"	:	9,
	"PD11:SAI2_SD_A"    	:	10,
    "PD11:USART3_CTS"		:	7,
	"PD12:EVENT-OUT"    	:	15,
	"PD12:FMC_A17"      	:	12,
	"PD12:I2C4_SCL"     	:	4,
	"PD12:LPTIM1_IN1"   	:	1,
	"PD12:LPTIM2_IN1"   	:	3,
	"PD12:QUADSPI_BK1_IO1"	:	9,
	"PD12:SAI2_FS_A"    	:	10,
	"PD12:TIM4_CH1"     	:	2,
	"PD12:USART3_RTS"   	:	7,
	"PD13:EVENT-OUT"    	:	15,
	"PD13:FMC_A18"      	:	12,
	"PD13:I2C4_SDA"     	:	4,
	"PD13:LPTIM1_OUT"   	:	1,
	"PD13:QUADSPI_BK1_IO3"	:	9,
	"PD13:SAI2_SCK_A"   	:	10,
	"PD13:TIM4_CH2"     	:	2,
	"PD14:EVENT-OUT"    	:	15,
	"PD14:FMC_D0"       	:	12,
	"PD14:FMC_DA0"      	:	12,
	"PD14:SAI3_MCLK_B"  	:	6,
	"PD14:TIM4_CH3"     	:	2,
	"PD14:UART8_CTS"    	:	8,
	"PD15:EVENT-OUT"    	:	15,
	"PD15:FMC_D1"       	:	12,
	"PD15:FMC_DA1"      	:	12,
	"PD15:SAI3_MCLK_A"  	:	6,
	"PD15:TIM4_CH4"     	:	2,
	"PD15:UART8_RTS"    	:	8,
	"PE0:DCMI_D2"       	:	13,
	"PE0:EVENT-OUT"     	:	15,
    "PE0:CAN1_RX"			:	9,
	"PE0:FMC_NBL0"      	:	12,
	"PE0:HRTIM_SCIN"    	:	3,
	"PE0:LPTIM1_ETR"    	:	1,
	"PE0:LPTIM2_ETR"    	:	4,
	"PE0:SAI2_MCK_A"    	:	10,
	"PE0:TIM4_ETR"      	:	2,
	"PE0:UART8_RX"      	:	8,
	"PE1:DCMI_D3"       	:	13,
	"PE1:EVENT-OUT"     	:	15,
    "PE1:CAN1_TX"			:	9,
	"PE1:FMC_NBL1"      	:	12,
	"PE1:HRTIM_SCOUT"   	:	3,
	"PE1:LPTIM1_IN2"    	:	1,
	"PE1:UART8_TX"      	:	8,
	"PE2:ETH_MII_TXD3"  	:	11,
	"PE2:EVENT-OUT"     	:	15,
	"PE2:FMC_A23"       	:	12,
	"PE2:QUADSPI_BK1_IO2"	:	9,
	"PE2:SAI1_CK1"      	:	2,
	"PE2:SAI1_MCLK_A"   	:	6,
	"PE2:SAI4_CK1"      	:	10,
	"PE2:SAI4_MCLK_A"   	:	8,
	"PE2:SPI4_SCK"      	:	5,
	"PE2:TRACECLK"      	:	0,
	"PE3:EVENT-OUT"     	:	15,
	"PE3:FMC_A19"       	:	12,
	"PE3:SAI1_SD_B"     	:	6,
	"PE3:SAI4_SD_B"     	:	8,
	"PE3:TIM15_BKIN"    	:	4,
	"PE3:TRACED0"       	:	0,
	"PE4:DCMI_D4"       	:	13,
	"PE4:DFSDM_DATIN3"  	:	3,
	"PE4:EVENT-OUT"     	:	15,
	"PE4:FMC_A20"       	:	12,
	"PE4:LCD_B0"        	:	14,
	"PE4:SAI1_D2"       	:	2,
	"PE4:SAI1_FS_A"     	:	6,
	"PE4:SAI4_D2"       	:	10,
	"PE4:SAI4_FS_A"     	:	8,
	"PE4:SPI4_NSS"      	:	5,
	"PE4:TIM15_CH1N"    	:	4,
	"PE4:TRACED1"       	:	0,
	"PE5:DCMI_D6"       	:	13,
	"PE5:DFSDM_CKIN3"   	:	3,
	"PE5:EVENT-OUT"     	:	15,
	"PE5:FMC_A21"       	:	12,
	"PE5:LCD_G0"        	:	14,
	"PE5:SAI1_CK2"      	:	2,
	"PE5:SAI1_SCK_A"    	:	6,
	"PE5:SAI4_CK2"      	:	10,
	"PE5:SAI4_SCK_A"    	:	8,
	"PE5:SPI4_MISO"     	:	5,
	"PE5:TIM15_CH1"     	:	4,
	"PE5:TRACED2"       	:	0,
	"PE6:DCMI_D7"       	:	13,
	"PE6:EVENT-OUT"     	:	15,
	"PE6:FMC_A22"       	:	12,
	"PE6:LCD_G1"        	:	14,
	"PE6:SAI1_D1"       	:	2,
	"PE6:SAI1_SD_A"     	:	6,
	"PE6:SAI2_MCK_B"    	:	10,
	"PE6:SAI4_D1"       	:	9,
	"PE6:SAI4_SD_A"     	:	8,
	"PE6:SPI4_MOSI"     	:	5,
	"PE6:TIM15_CH2"     	:	4,
	"PE6:TIM1_BKIN2"    	:	1,
	"PE6:TIM1_BKIN2_COMP12"	:	11,
	"PE6:TRACED3"       	:	0,
	"PE7:DFSDM_DATIN2"  	:	3,
	"PE7:EVENT-OUT"     	:	15,
	"PE7:FMC_D4"        	:	12,
	"PE7:FMC_DA4"       	:	12,
	"PE7:QUADSPI_BK2_IO0"	:	10,
	"PE7:TIM1_ETR"      	:	1,
	"PE7:UART7_RX"      	:	7,
	"PE8:COMP_2_OUT"    	:	13,
	"PE8:DFSDM_CKIN2"   	:	3,
	"PE8:EVENT-OUT"     	:	15,
	"PE8:FMC_D5"        	:	12,
	"PE8:FMC_DA5"       	:	12,
	"PE8:QUADSPI_BK2_IO1"	:	10,
	"PE8:TIM1_CH1N"     	:	1,
	"PE8:UART7_TX"      	:	7,
	"PE9:DFSDM_CKOUT"   	:	3,
	"PE9:EVENT-OUT"     	:	15,
	"PE9:FMC_D6"        	:	12,
	"PE9:FMC_DA6"       	:	12,
	"PE9:QUADSPI_BK2_IO2"	:	10,
	"PE9:TIM1_CH1"      	:	1,
	"PE9:UART7_RTS"     	:	7,
	"PE10:DFSDM_DATIN4" 	:	3,
	"PE10:EVENT-OUT"    	:	15,
	"PE10:FMC_D7"       	:	12,
	"PE10:FMC_DA7"      	:	12,
	"PE10:QUADSPI_BK2_IO3"	:	10,
	"PE10:TIM1_CH2N"    	:	1,
	"PE10:UART7_CTS"    	:	7,
	"PE11:DFSDM_CKIN4"  	:	3,
	"PE11:EVENT-OUT"    	:	15,
	"PE11:FMC_D8"       	:	12,
	"PE11:FMC_DA8"      	:	12,
	"PE11:LCD_G3"       	:	14,
	"PE11:SAI2_SD_B"    	:	10,
	"PE11:SPI4_NSS"     	:	5,
	"PE11:TIM1_CH2"     	:	1,
	"PE12:COMP_1_OUT"   	:	13,
	"PE12:DFSDM_DATIN5" 	:	3,
	"PE12:EVENT-OUT"    	:	15,
	"PE12:FMC_D9"       	:	12,
	"PE12:FMC_DA9"      	:	12,
	"PE12:LCD_B4"       	:	14,
	"PE12:SAI2_SCK_B"   	:	10,
	"PE12:SPI4_SCK"     	:	5,
	"PE12:TIM1_CH3N"    	:	1,
	"PE13:COMP_2_OUT"   	:	13,
	"PE13:DFSDM_CKIN5"  	:	3,
	"PE13:EVENT-OUT"    	:	15,
	"PE13:FMC_D10"      	:	12,
	"PE13:FMC_DA10"     	:	12,
	"PE13:LCD_DE"       	:	14,
	"PE13:SAI2_FS_B"    	:	10,
	"PE13:SPI4_MISO"    	:	5,
	"PE13:TIM1_CH3"     	:	1,
	"PE14:EVENT-OUT"    	:	15,
	"PE14:FMC_D11"      	:	12,
	"PE14:FMC_DA11"     	:	12,
	"PE14:LCD_CLK"      	:	14,
	"PE14:SAI2_MCK_B"   	:	10,
	"PE14:SPI4_MOSI"    	:	5,
	"PE14:TIM1_CH4"     	:	1,
	"PE15:EVENT-OUT"    	:	15,
	"PE15:FMC_D12"      	:	12,
	"PE15:FMC_DA12"     	:	12,
	"PE15:HDMI__TIM1_BKIN"	:	5,
	"PE15:LCD_R7"       	:	14,
	"PE15:TIM1_BKIN"    	:	1,
	"PE15:TIM1_BKIN_COMP12"	:	13,
	"PF0:EVENT-OUT"     	:	15,
	"PF0:FMC_A0"        	:	12,
	"PF0:I2C2_SDA"      	:	4,
	"PF1:EVENT-OUT"     	:	15,
	"PF1:FMC_A1"        	:	12,
	"PF1:I2C2_SCL"      	:	4,
	"PF2:EVENT-OUT"     	:	15,
	"PF2:FMC_A2"        	:	12,
	"PF2:I2C2_SMBA"     	:	4,
	"PF3:EVENT-OUT"     	:	15,
	"PF3:FMC_A3"        	:	12,
	"PF4:EVENT-OUT"     	:	15,
	"PF4:FMC_A4"        	:	12,
	"PF5:EVENT-OUT"     	:	15,
	"PF5:FMC_A5"        	:	12,
	"PF6:EVENT-OUT"     	:	15,
	"PF6:QUADSPI_BK1_IO3"	:	9,
	"PF6:SAI1_SD_B"     	:	6,
	"PF6:SAI4_SD_B"     	:	8,
	"PF6:SPI5_NSS"      	:	5,
	"PF6:TIM16_CH1"     	:	1,
	"PF6:UART7_RX"      	:	7,
	"PF7:EVENT-OUT"     	:	15,
	"PF7:QUADSPI_BK1_IO2"	:	9,
	"PF7:SAI1_MCLK_B"   	:	6,
	"PF7:SAI4_MCLK_B"   	:	8,
	"PF7:SPI5_SCK"      	:	5,
	"PF7:TIM17_CH1"     	:	1,
	"PF7:UART7_TX"      	:	7,
	"PF8:EVENT-OUT"     	:	15,
	"PF8:QUADSPI_BK1_IO0"	:	10,
	"PF8:SAI1_SCK_B"    	:	6,
	"PF8:SAI4_SCK_B"    	:	8,
	"PF8:SPI5_MISO"     	:	5,
	"PF8:TIM13_CH1"     	:	9,
	"PF8:TIM16_CH1N"    	:	1,
	"PF8:UART7_RTS"     	:	7,
	"PF9:EVENT-OUT"     	:	15,
	"PF9:QUADSPI_BK1_IO1"	:	10,
	"PF9:SAI1_FS_B"     	:	6,
	"PF9:SAI4_FS_B"     	:	8,
	"PF9:SPI5_MOSI"     	:	5,
	"PF9:TIM14_CH1"     	:	9,
	"PF9:TIM17_CH1N"    	:	1,
	"PF9:UART7_CTS"     	:	7,
	"PF10:DCMI_D11"     	:	13,
	"PF10:EVENT-OUT"    	:	15,
	"PF10:LCD_DE"       	:	14,
	"PF10:QUADSPI_CLK"  	:	9,
	"PF10:SAI1_D3"      	:	2,
	"PF10:SAI4_D3"      	:	10,
	"PF10:TIM16_BKIN"   	:	1,
	"PF11:DCMI_D12"     	:	13,
	"PF11:EVENT-OUT"    	:	15,
	"PF11:FMC_SDNRAS"   	:	12,
	"PF11:SAI2_SD_B"    	:	10,
	"PF11:SPI5_MOSI"    	:	5,
	"PF12:EVENT-OUT"    	:	15,
	"PF12:FMC_A6"       	:	12,
	"PF13:DFSDM_DATIN6" 	:	3,
	"PF13:EVENT-OUT"    	:	15,
	"PF13:FMC_A7"       	:	12,
	"PF13:I2C4_SMBA"    	:	4,
	"PF14:DFSDM_CKIN6"  	:	3,
	"PF14:EVENT-OUT"    	:	15,
	"PF14:FMC_A8"       	:	12,
	"PF14:I2C4_SCL"     	:	4,
	"PF15:EVENT-OUT"    	:	15,
	"PF15:FMC_A9"       	:	12,
	"PF15:I2C4_SDA"     	:	4,
	"PG0:EVENT-OUT"     	:	15,
	"PG0:FMC_A10"       	:	12,
	"PG1:EVENT-OUT"     	:	15,
	"PG1:FMC_A11"       	:	12,
	"PG2:EVENT-OUT"     	:	15,
	"PG2:FMC_A12"       	:	12,
	"PG2:TIM8_BKIN"     	:	3,
	"PG2:TIM8_BKIN_COMP12"	:	11,
	"PG3:EVENT-OUT"     	:	15,
	"PG3:FMC_A13"       	:	12,
	"PG3:TIM8_BKIN2"    	:	3,
	"PG3:TIM8_BKIN2_COMP12"	:	11,
	"PG4:EVENT-OUT"     	:	15,
	"PG4:FMC_A14"       	:	12,
	"PG4:FMC_BA0"       	:	12,
	"PG4:TIM1_BKIN2"    	:	1,
	"PG4:TIM1_BKIN2_COMP12"	:	11,
	"PG5:EVENT-OUT"     	:	15,
	"PG5:FMC_A15"       	:	12,
	"PG5:FMC_BA1"       	:	12,
	"PG5:TIM1_ETR"      	:	1,
	"PG6:DCMI_D12"      	:	13,
	"PG6:EVENT-OUT"     	:	15,
	"PG6:FMC_NE3"       	:	12,
	"PG6:HRTIM_CHE1"    	:	2,
	"PG6:LCD_R7"        	:	14,
	"PG6:QUADSPI_BK1_NCS"	:	10,
	"PG6:TIM17_BKIN"    	:	1,
	"PG7:DCMI_D13"      	:	13,
	"PG7:EVENT-OUT"     	:	15,
	"PG7:FMC_INT"       	:	12,
	"PG7:HRTIM_CHE2"    	:	2,
	"PG7:LCD_CLK"       	:	14,
	"PG7:SAI1_MCLK_A"   	:	6,
	"PG7:USART6_CK"     	:	7,
	"PG8:ETH_PPS_OUT"   	:	11,
	"PG8:EVENT-OUT"     	:	15,
	"PG8:FMC_SDCLK"     	:	12,
	"PG8:LCD_G7"        	:	14,
	"PG8:SPDIFRX_IN2"   	:	8,
	"PG8:SPI6_NSS"      	:	5,
	"PG8:TIM8_ETR"      	:	3,
	"PG8:USART6_RTS"    	:	7,
	"PG9:DCMI_VSYNC"    	:	13,
	"PG9:EVENT-OUT"     	:	15,
	"PG9:FMC_NCE"       	:	12,
	"PG9:FMC_NE2"       	:	12,
	"PG9:I2S1_SDI"      	:	5,
	"PG9:QUADSPI_BK2_IO2"	:	9,
	"PG9:SAI2_FS_B"     	:	10,
	"PG9:SPDIFRX_IN3"   	:	8,
	"PG9:SPI1_MISO"     	:	5,
	"PG9:USART6_RX"     	:	7,
	"PG10:DCMI_D2"      	:	13,
	"PG10:EVENT-OUT"    	:	15,
	"PG10:FMC_NE3"      	:	12,
	"PG10:HRTIM_FLT5"   	:	2,
	"PG10:I2S1_WS"      	:	5,
	"PG10:LCD_B2"       	:	14,
	"PG10:LCD_G3"       	:	9,
	"PG10:SAI2_SD_B"    	:	10,
	"PG10:SPI1_NSS"     	:	5,
	"PG11:DCMI_D3"      	:	13,
	"PG11:ETH_MII_TX_EN"	:	11,
	"PG11:ETH_RMII_TX_EN"	:	11,
	"PG11:EVENT-OUT"    	:	15,
	"PG11:HRTIM_EEV4"   	:	2,
	"PG11:I2S1_CK"      	:	5,
	"PG11:LCD_B3"       	:	14,
	"PG11:SDMMC2_D2"    	:	10,
	"PG11:SPDIFRX_IN0"  	:	8,
	"PG11:SPI1_SCK"     	:	5,
	"PG12:ETH_MII_TXD1" 	:	11,
	"PG12:ETH_RMII_TXD1"	:	11,
	"PG12:EVENT-OUT"    	:	15,
	"PG12:FMC_NE4"      	:	12,
	"PG12:HRTIM_EEV5"   	:	2,
	"PG12:LCD_B1"       	:	14,
	"PG12:LCD_B4"       	:	9,
	"PG12:LPTIM1_IN1"   	:	1,
	"PG12:SPDIFRX_IN1"  	:	8,
	"PG12:SPI6_MISO"    	:	5,
	"PG12:USART6_RTS"   	:	7,
	"PG13:ETH_MII_TXD0" 	:	11,
	"PG13:ETH_RMII_TXD0"	:	11,
	"PG13:EVENT-OUT"    	:	15,
	"PG13:FMC_A24"      	:	12,
	"PG13:HRTIM_EEV10"  	:	2,
	"PG13:LCD_R0"       	:	14,
	"PG13:LPTIM1_OUT"   	:	1,
	"PG13:SPI6_SCK"     	:	5,
	"PG13:TRACED0"      	:	0,
    "PG13:USART6_CTS"		:	7,
	"PG14:ETH_MII_TXD1" 	:	11,
	"PG14:ETH_RMII_TXD1"	:	11,
	"PG14:EVENT-OUT"    	:	15,
	"PG14:FMC_A25"      	:	12,
	"PG14:LCD_B0"       	:	14,
	"PG14:LPTIM1_ETR"   	:	1,
	"PG14:QUADSPI_BK2_IO3"	:	9,
	"PG14:SPI6_MOSI"    	:	5,
	"PG14:TRACED1"      	:	0,
	"PG14:USART6_TX"    	:	7,
	"PG15:DCMI_D13"     	:	13,
	"PG15:EVENT-OUT"    	:	15,
	"PG15:FMC_SDNCAS"   	:	12,
    "PG15:USART6_CTS"		:	7,
	"PH0:EVENT-OUT"     	:	15,
	"PH1:EVENT-OUT"     	:	15,
	"PH2:ETH_MII_CRS"   	:	11,
	"PH2:EVENT-OUT"     	:	15,
	"PH2:FMC_SDCKE0"    	:	12,
	"PH2:LCD_R0"        	:	14,
	"PH2:LPTIM1_IN2"    	:	1,
	"PH2:QUADSPI_BK2_IO0"	:	9,
	"PH2:SAI2_SCK_B"    	:	10,
	"PH3:ETH_MII_COL"   	:	11,
	"PH3:EVENT-OUT"     	:	15,
	"PH3:FMC_SDNE0"     	:	12,
	"PH3:LCD_R1"        	:	14,
	"PH3:QUADSPI_BK2_IO1"	:	9,
	"PH3:SAI2_MCK_B"    	:	10,
	"PH4:EVENT-OUT"     	:	15,
	"PH4:I2C2_SCL"      	:	4,
	"PH4:LCD_G4"        	:	14,
	"PH4:LCD_G5"        	:	9,
	"PH4:OTG_HS_ULPI_NXT"	:	10,
	"PH5:EVENT-OUT"     	:	15,
	"PH5:FMC_SDNWE"     	:	12,
	"PH5:I2C2_SDA"      	:	4,
	"PH5:SPI5_NSS"      	:	5,
	"PH6:DCMI_D8"       	:	13,
	"PH6:ETH_MII_RXD2"  	:	11,
	"PH6:EVENT-OUT"     	:	15,
	"PH6:FMC_SDNE1"     	:	12,
	"PH6:I2C2_SMBA"     	:	4,
	"PH6:SPI5_SCK"      	:	5,
	"PH6:TIM12_CH1"     	:	2,
	"PH7:DCMI_D9"       	:	13,
	"PH7:ETH_MII_RXD3"  	:	11,
	"PH7:EVENT-OUT"     	:	15,
	"PH7:FMC_SDCKE1"    	:	12,
	"PH7:I2C3_SCL"      	:	4,
	"PH7:SPI5_MISO"     	:	5,
	"PH8:DCMI_HSYNC"    	:	13,
	"PH8:EVENT-OUT"     	:	15,
	"PH8:FMC_D16"       	:	12,
	"PH8:I2C3_SDA"      	:	4,
	"PH8:LCD_R2"        	:	14,
	"PH8:TIM5_ETR"      	:	2,
	"PH9:DCMI_D0"       	:	13,
	"PH9:EVENT-OUT"     	:	15,
	"PH9:FMC_D17"       	:	12,
	"PH9:I2C3_SMBA"     	:	4,
	"PH9:LCD_R3"        	:	14,
	"PH9:TIM12_CH2"     	:	2,
	"PH10:DCMI_D1"      	:	13,
	"PH10:EVENT-OUT"    	:	15,
	"PH10:FMC_D18"      	:	12,
	"PH10:I2C4_SMBA"    	:	4,
	"PH10:LCD_R4"       	:	14,
	"PH10:TIM5_CH1"     	:	2,
	"PH11:DCMI_D2"      	:	13,
	"PH11:EVENT-OUT"    	:	15,
	"PH11:FMC_D19"      	:	12,
	"PH11:I2C4_SCL"     	:	4,
	"PH11:LCD_R5"       	:	14,
	"PH11:TIM5_CH2"     	:	2,
	"PH12:DCMI_D3"      	:	13,
	"PH12:EVENT-OUT"    	:	15,
	"PH12:FMC_D20"      	:	12,
	"PH12:I2C4_SDA"     	:	4,
	"PH12:LCD_R6"       	:	14,
	"PH12:TIM5_CH3"     	:	2,
	"PH13:EVENT-OUT"    	:	15,
    "PH13:CAN1_TX"	    	:	9,
	"PH13:FMC_D21"      	:	12,
	"PH13:LCD_G2"       	:	14,
	"PH13:TIM8_CH1N"    	:	3,
	"PH13:UART4_TX"     	:	8,
	"PH14:DCMI_D4"      	:	13,
	"PH14:EVENT-OUT"    	:	15,
    "PH14:CAN1_RX"	    	:	9,
	"PH14:FMC_D22"      	:	12,
	"PH14:LCD_G3"       	:	14,
    "PH14:TIM8_UCH2N"  		:	3,
	"PH14:UART4_RX"     	:	8,
	"PH15:DCMI_D11"     	:	13,
	"PH15:EVENT-OUT"    	:	15,
    "PH15:CAN1_TX"			:	9,
	"PH15:FMC_D23"      	:	12,
	"PH15:LCD_G4"       	:	14,
	"PH15:TIM8_CH3N"    	:	3,
	"PI0:DCMI_D13"      	:	13,
	"PI0:EVENT-OUT"     	:	15,
    "PI0:CAN1_RX"			:	9,
	"PI0:FMC_D24"       	:	12,
	"PI0:I2S2_WS"       	:	5,
	"PI0:LCD_G5"        	:	14,
	"PI0:SPI2_NSS"      	:	5,
	"PI0:TIM5_CH4"      	:	2,
	"PI1:DCMI_D8"       	:	13,
	"PI1:EVENT-OUT"     	:	15,
	"PI1:FMC_D25"       	:	12,
	"PI1:I2S2_CK"       	:	5,
	"PI1:LCD_G6"        	:	14,
	"PI1:SPI2_SCK"      	:	5,
	"PI1:TIM8_BKIN2"    	:	3,
	"PI1:TIM8_BKIN2_COMP12"	:	11,
	"PI2:DCMI_D9"       	:	13,
	"PI2:EVENT-OUT"     	:	15,
	"PI2:FMC_D26"       	:	12,
	"PI2:I2S2_SDI"      	:	5,
	"PI2:LCD_G7"        	:	14,
	"PI2:SPI2_MISO"     	:	5,
	"PI2:TIM8_CH4"      	:	3,
	"PI3:DCMI_D10"      	:	13,
	"PI3:EVENT-OUT"     	:	15,
	"PI3:FMC_D27"       	:	12,
	"PI3:I2S2_SDO"      	:	5,
	"PI3:SPI2_MOSI"     	:	5,
	"PI3:TIM8_ETR"      	:	3,
	"PI4:DCMI_D5"       	:	13,
	"PI4:EVENT-OUT"     	:	15,
	"PI4:FMC_NBL2"      	:	12,
	"PI4:LCD_B4"        	:	14,
	"PI4:SAI2_MCK_A"    	:	10,
	"PI4:TIM8_BKIN"     	:	3,
	"PI4:TIM8_BKIN_COMP12"	:	11,
	"PI5:DCMI_VSYNC"    	:	13,
	"PI5:EVENT-OUT"     	:	15,
	"PI5:FMC_NBL3"      	:	12,
	"PI5:LCD_B5"        	:	14,
	"PI5:SAI2_SCK_A"    	:	10,
	"PI5:TIM8_CH1"      	:	3,
	"PI6:DCMI_D6"       	:	13,
	"PI6:EVENT-OUT"     	:	15,
	"PI6:FMC_D28"       	:	12,
	"PI6:LCD_B6"        	:	14,
	"PI6:SAI2_SD_A"     	:	10,
	"PI6:TIM8_CH2"      	:	3,
	"PI7:DCMI_D7"       	:	13,
	"PI7:EVENT-OUT"     	:	15,
	"PI7:FMC_D29"       	:	12,
	"PI7:LCD_B7"        	:	14,
	"PI7:SAI2_FS_A"     	:	10,
	"PI7:TIM8_CH3"      	:	3,
	"PI8:EVENT-OUT"     	:	15,
	"PI9:EVENT-OUT"     	:	15,
    "PI9:CAN1_RX"	     	:	9,
	"PI9:FMC_D30"       	:	12,
	"PI9:LCD_VSYNC"     	:	14,
	"PI9:UART4_RX"      	:	8,
	"PI10:ETH_MII_RX_ER"	:	11,
	"PI10:EVENT-OUT"    	:	15,
    "PI10:CAN1_RX"			:	9,
	"PI10:FMC_D31"      	:	12,
	"PI10:LCD_HSYNC"    	:	14,
	"PI11:EVENT-OUT"    	:	15,
	"PI11:LCD_G6"       	:	9,
	"PI11:OTG_HS_ULPI_DIR"	:	10,
	"PI12:EVENT-OUT"    	:	15,
	"PI12:LCD_HSYNC"    	:	14,
	"PI13:EVENT-OUT"    	:	15,
	"PI13:LCD_VSYNC"    	:	14,
	"PI14:EVENT-OUT"    	:	15,
	"PI14:LCD_CLK"      	:	14,
	"PI15:EVENT-OUT"    	:	15,
	"PI15:LCD_G2"       	:	9,
	"PI15:LCD_R0"       	:	14,
	"PJ0:EVENT-OUT"     	:	15,
	"PJ0:LCD_R1"        	:	14,
	"PJ0:LCD_R7"        	:	9,
	"PJ1:EVENT-OUT"     	:	15,
	"PJ1:LCD_R2"        	:	14,
	"PJ2:EVENT-OUT"     	:	15,
	"PJ2:LCD_R3"        	:	14,
	"PJ3:EVENT-OUT"     	:	15,
	"PJ3:LCD_R4"        	:	14,
	"PJ4:EVENT-OUT"     	:	15,
	"PJ4:LCD_R5"        	:	14,
	"PJ5:EVENT-OUT"     	:	15,
	"PJ5:LCD_R6"        	:	14,
	"PJ6:EVENT-OUT"     	:	15,
	"PJ6:LCD_R7"        	:	14,
	"PJ6:TIM8_CH2"      	:	3,
	"PJ7:EVENT-OUT"     	:	15,
	"PJ7:LCD_G0"        	:	14,
	"PJ7:TIM8_CH2N"     	:	3,
	"PJ7:TRGIN"         	:	0,
	"PJ8:EVENT-OUT"     	:	15,
	"PJ8:LCD_G1"        	:	14,
	"PJ8:TIM1_CH3N"     	:	1,
	"PJ8:TIM8_CH1"      	:	3,
	"PJ8:UART8_TX"      	:	8,
	"PJ9:EVENT-OUT"     	:	15,
	"PJ9:LCD_G2"        	:	14,
	"PJ9:TIM1_CH3"      	:	1,
	"PJ9:TIM8_CH1N"     	:	3,
	"PJ9:UART8_RX"      	:	8,
	"PJ10:EVENT-OUT"    	:	15,
	"PJ10:LCD_G3"       	:	14,
	"PJ10:SPI5_MOSI"    	:	5,
	"PJ10:TIM1_CH2N"    	:	1,
	"PJ10:TIM8_CH2"     	:	3,
	"PJ11:EVENT-OUT"    	:	15,
	"PJ11:LCD_G4"       	:	14,
	"PJ11:SPI5_MISO"    	:	5,
	"PJ11:TIM1_CH2"     	:	1,
	"PJ11:TIM8_CH2N"    	:	3,
	"PJ12:EVENT-OUT"    	:	15,
	"PJ12:LCD_B0"       	:	14,
	"PJ12:LCD_G3"       	:	9,
	"PJ12:TRGOUT"       	:	0,
	"PJ13:EVENT-OUT"    	:	15,
	"PJ13:LCD_B1"       	:	14,
	"PJ13:LCD_B4"       	:	9,
	"PJ14:EVENT-OUT"    	:	15,
	"PJ14:LCD_B2"       	:	14,
	"PJ15:EVENT-OUT"    	:	15,
	"PJ15:LCD_B3"       	:	14,
	"PK0:EVENT-OUT"     	:	15,
	"PK0:LCD_G5"        	:	14,
	"PK0:SPI5_SCK"      	:	5,
	"PK0:TIM1_CH1N"     	:	1,
	"PK0:TIM8_CH3"      	:	3,
	"PK1:EVENT-OUT"     	:	15,
	"PK1:LCD_G6"        	:	14,
	"PK1:SPI5_NSS"      	:	5,
	"PK1:TIM1_CH1"      	:	1,
	"PK1:TIM8_CH3N"     	:	3,
	"PK2:EVENT-OUT"     	:	15,
	"PK2:LCD_G7"        	:	14,
	"PK2:TIM1_BKIN"     	:	1,
	"PK2:TIM1_BKIN_COMP12"	:	11,
	"PK2:TIM8_BKIN"     	:	3,
	"PK2:TIM8_BKIN_COMP12"	:	10,
	"PK3:EVENT-OUT"     	:	15,
	"PK3:LCD_B4"        	:	14,
	"PK4:EVENT-OUT"     	:	15,
	"PK4:LCD_B5"        	:	14,
	"PK5:EVENT-OUT"     	:	15,
	"PK5:LCD_B6"        	:	14,
	"PK6:EVENT-OUT"     	:	15,
	"PK6:LCD_B7"        	:	14,
	"PK7:EVENT-OUT"     	:	15,
	"PK7:LCD_DE"        	:	14,
}

ADC1_map = {
    # format is PIN : ADC1_CHAN
    "PF11"  :   2,
    "PA6"	:	3,
    "PC4"	:	4,
    "PB1"	:	5,
    "PF12"  :   6,
    "PA7"	:	7,
    "PC5"	:	8,
    "PB0"	:	9,
	"PC0"	:	10,
	"PC1"	:	11,
	"PC2"	:	12,
    "PC3"	:	13,
    "PA2"	:	14,
    "PA3"	:	15,
    "PA0"	:	16,
    "PA1"	:	17,
    "PA4"	:	18,
    "PA5"	:	19,
}
