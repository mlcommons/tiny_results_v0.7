# Debug UART Interface
# to Pmod Header JA

# RX uart, PMOD A pin 2 (JA2), IO_L4P_T0_15, B11, BROWN cable
set_property -dict { PACKAGE_PIN B11 IOSTANDARD LVCMOS33 } [get_ports { usb_uart_rxd_debug }];

# TX uart, PMOD A pin 3 (JA3), IO_L4N_T0_15, A11, RED cable
set_property -dict { PACKAGE_PIN A11 IOSTANDARD LVCMOS33 } [get_ports { usb_uart_txd_debug }];

set_property -dict { PACKAGE_PIN D10 IOSTANDARD LVCMOS33 } [get_ports { usb_uart_txd }];
