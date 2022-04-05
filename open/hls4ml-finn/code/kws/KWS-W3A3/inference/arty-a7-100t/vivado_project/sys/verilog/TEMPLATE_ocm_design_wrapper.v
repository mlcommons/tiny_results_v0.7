`timescale 1 ps / 1 ps

module finn_kws_mlp_w3a3_ocm_design_wrapper
   (
    reset,
    sys_clock,
    usb_uart_rxd,
    usb_uart_txd,
    usb_uart_rxd_debug,
    usb_uart_txd_debug    
    );
  input reset;
  input sys_clock;
  input usb_uart_rxd;
  output usb_uart_txd;
  output usb_uart_rxd_debug;
  output usb_uart_txd_debug;

  wire reset;
  wire sys_clock;
  wire usb_uart_rxd;
  wire usb_uart_txd;
  wire usb_uart_rxd_debug;
  wire usb_uart_txd_debug;
  
  assign usb_uart_rxd_debug = usb_uart_rxd;
  assign usb_uart_txd = usb_uart_txd_debug;
  
  finn_kws_mlp_w3a3_ocm_design finn_kws_mlp_w3a3_ocm_design_i
       (
        .reset(reset),
        .sys_clock(sys_clock),
        .usb_uart_rxd(usb_uart_rxd),
        .usb_uart_txd_debug(usb_uart_txd_debug));
endmodule
