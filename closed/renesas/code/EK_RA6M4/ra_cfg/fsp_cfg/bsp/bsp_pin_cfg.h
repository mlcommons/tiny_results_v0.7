/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"
#define ARDUINO_A0_MIKROBUS_AN (IOPORT_PORT_00_PIN_00)
#define ARDUINO_A1 (IOPORT_PORT_00_PIN_01)
#define ETH_INT (IOPORT_PORT_00_PIN_02)
#define ARDUINO_A2 (IOPORT_PORT_00_PIN_03)
#define OSPI_RST (IOPORT_PORT_00_PIN_04)
#define SW1 (IOPORT_PORT_00_PIN_05)
#define SW2 (IOPORT_PORT_00_PIN_06)
#define ARDUINO_A3 (IOPORT_PORT_00_PIN_07)
#define PMOD1_INT (IOPORT_PORT_00_PIN_08)
#define ARDUINO_D2 (IOPORT_PORT_00_PIN_09)
#define ARDUINO_A4 (IOPORT_PORT_00_PIN_14)
#define ARDUINO_A5 (IOPORT_PORT_00_PIN_15)
#define OSPI_CLK (IOPORT_PORT_01_PIN_00)
#define OSPI_SIO7 (IOPORT_PORT_01_PIN_01)
#define OSPI_SIO1 (IOPORT_PORT_01_PIN_02)
#define OSPI_SIO6 (IOPORT_PORT_01_PIN_03)
#define OSPI_DQS (IOPORT_PORT_01_PIN_04)
#define OSPI_SIO5 (IOPORT_PORT_01_PIN_05)
#define OSPI_SIO0 (IOPORT_PORT_01_PIN_06)
#define OSPI_SIO3 (IOPORT_PORT_01_PIN_07)
#define ARDUINO_D3 (IOPORT_PORT_01_PIN_11)
#define MIKROBUS_RST (IOPORT_PORT_01_PIN_15)
#define ARDUILD_MISO_MIKROBUS_MISO (IOPORT_PORT_02_PIN_02)
#define ARDUILD_MOSI_MIKROBUS_MOSI (IOPORT_PORT_02_PIN_03)
#define ARDUILD_CLK_MIKROBUS_CLK (IOPORT_PORT_02_PIN_04)
#define ARDUILD_SS_MIKROBUS_SS (IOPORT_PORT_02_PIN_05)
#define PMOD1_SS (IOPORT_PORT_03_PIN_01)
#define ARDUINO_D9 (IOPORT_PORT_03_PIN_03)
#define ARDUINO_D7 (IOPORT_PORT_03_PIN_04)
#define QSPI_CLK (IOPORT_PORT_03_PIN_05)
#define QSPI_SSL (IOPORT_PORT_03_PIN_06)
#define QSPI_IO0 (IOPORT_PORT_03_PIN_07)
#define QSPI_IO1 (IOPORT_PORT_03_PIN_08)
#define QSPI_IO2 (IOPORT_PORT_03_PIN_09)
#define QSPI_IO3 (IOPORT_PORT_03_PIN_10)
#define PMOD1_IO1 (IOPORT_PORT_03_PIN_11)
#define PMOD1_IO2 (IOPORT_PORT_03_PIN_12)
#define PMOD1_IO3 (IOPORT_PORT_03_PIN_13)
#define LED3 (IOPORT_PORT_04_PIN_00)
#define ETH_MDC (IOPORT_PORT_04_PIN_01)
#define ETH_MDIO (IOPORT_PORT_04_PIN_02)
#define ETH_RST (IOPORT_PORT_04_PIN_03)
#define LED2 (IOPORT_PORT_04_PIN_04)
#define ETH_TXD_EN (IOPORT_PORT_04_PIN_05)
#define ETH_TXD1 (IOPORT_PORT_04_PIN_06)
#define USB_VBUS_DETECT (IOPORT_PORT_04_PIN_07)
#define ARDUINO_D6_MIKROBUS_PWM (IOPORT_PORT_04_PIN_08)
#define MIKROBUS_INT (IOPORT_PORT_04_PIN_09)
#define PMOD2_MISO (IOPORT_PORT_04_PIN_10)
#define PMOD2_MOSI (IOPORT_PORT_04_PIN_11)
#define PMOD2_CLK (IOPORT_PORT_04_PIN_12)
#define PMOD2_SS (IOPORT_PORT_04_PIN_13)
#define PMOD2_INT (IOPORT_PORT_04_PIN_14)
#define LED1 (IOPORT_PORT_04_PIN_15)
#define USB_VBUS_EN (IOPORT_PORT_05_PIN_00)
#define USB_OC (IOPORT_PORT_05_PIN_01)
#define GROVE2_SCL (IOPORT_PORT_05_PIN_05)
#define GROVE2_SDA (IOPORT_PORT_05_PIN_06)
#define ARDUINO_SDA_MIKROBUS_SDA_GROVE1_SDA_QWIIC_SDA (IOPORT_PORT_05_PIN_11)
#define ARDUINO_SCL_MIKROBUS_SCL_GROVE1_SCL_QWIIC_SCL (IOPORT_PORT_05_PIN_12)
#define OSPI_SIO4 (IOPORT_PORT_06_PIN_00)
#define OSPI_SIO2 (IOPORT_PORT_06_PIN_01)
#define OSPI_CS (IOPORT_PORT_06_PIN_09)
#define OSPI_CS0 (IOPORT_PORT_06_PIN_10)
#define ARDUINO_D8 (IOPORT_PORT_06_PIN_11)
#define ARDUINO_RST (IOPORT_PORT_06_PIN_12)
#define ARDUINO_TX_MIKROBUS_TX (IOPORT_PORT_06_PIN_13)
#define ARDUINO_RX_MIKROBUS_RX (IOPORT_PORT_06_PIN_14)
#define ETH_TXD0 (IOPORT_PORT_07_PIN_00)
#define ETH_REF50 (IOPORT_PORT_07_PIN_01)
#define ETH_RXD0 (IOPORT_PORT_07_PIN_02)
#define ETH_RXD1 (IOPORT_PORT_07_PIN_03)
#define ETH_RX_ERR (IOPORT_PORT_07_PIN_04)
#define ETH_CRS_DV (IOPORT_PORT_07_PIN_05)
#define PMOD2_IO1 (IOPORT_PORT_07_PIN_08)
#define PMOD2_IO2 (IOPORT_PORT_07_PIN_09)
#define PMOD2_IO3 (IOPORT_PORT_07_PIN_10)
#define ARDUINO_D5 (IOPORT_PORT_07_PIN_12)
#define ARDUINO_D4 (IOPORT_PORT_07_PIN_13)
extern const ioport_cfg_t g_bsp_pin_cfg; /* R7FA6M4AF3CFB.pincfg */

void BSP_PinConfigSecurityInit();
#endif /* BSP_PIN_CFG_H_ */
