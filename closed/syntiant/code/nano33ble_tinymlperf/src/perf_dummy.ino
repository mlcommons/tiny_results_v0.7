
#include <inttypes.h>
#include <stddef.h>

#include <SPI.h>
#include <Wire.h>


#include "syntiant_ilib/syntiant_ndp.h"
#include "syntiant_ilib/syntiant_ndp_error.h"
#include "syntiant_ilib/ndp120_regs.h"
#include "syntiant_ilib/ndp120_spi_regs.h"
#include "syntiant_ilib/syntiant_ndp120.h"

#include "mlperf_internal.h"
#include "mlperf_submitter.h"
#include "ndp.h"

//#define SYNSERIAL Serial
#define SYNSERIAL Serial1

// define exactly one of these
// #define CLK_EXT32K_FLL30M  // ext 32k oscillator, 30MHz sys clock, use PLL
#define CLK_EXT32K_PLL30M    // int 32k oscillator, 30MHz sys clock, use PLL

// these debug print routines mainly meant for 
// inserting debug statements in syntiant ilib code if necessary
extern "C" {
  void debugstr(char *str);
  void debugint(int i);
}

void debugstr(char *str) 
{
    SYNSERIAL.println(str);
}
void debugint(int i) 
{
    SYNSERIAL.print("0x");
    SYNSERIAL.println(i, HEX);
}


/* structure to hold state */
struct ndp120_app_pi_s {
    int init_mode;
    int mbwait_timeout;
    uint8_t device_type;
  // syntiant_evb_io_handle_t io_handle;
    struct syntiant_ndp_device_s *ndp;
};


static int
check_status(const char *message, int s, int do_exit)
{
    if (s) {
        SYNSERIAL.print(message);
        SYNSERIAL.print(" failed: ");
        SYNSERIAL.println(syntiant_ndp_error_name(s));
        while(do_exit);
    }    
    return s;
}

enum NRF_GPIO_Pins
{
    PORSTB = NRF_GPIO_PIN_MAP(1, 14),   // NDP PORSTB             (1.14 = D6)
    SPI_CS = NRF_GPIO_PIN_MAP(1, 12),   // NDP SPI chip select    (1.12 = D3)
};

struct syntiant_ndp_device_s *ndp = NULL;
struct syntiant_ndp_integration_interfaces_s iif;
char **labels;

/* NRF GPIO for clk generator */
#define CLK_GEN_ARGS 1, 13 

#define LABELS_LEN 2048
#define STRING_LEN 256
#define MAX_LABELS 64

//TODO: Replace with new DNN pkg
extern "C" unsigned char dnn_ndp120_params[];
extern "C" unsigned int dnn_ndp120_params_len;

extern "C" unsigned char dsp_firmware[];
extern "C" unsigned int dsp_firmware_len;


// JHDBG reducing SPI speed to try to work with level shifters
// SPI speed
// uint32_t spi_speed_general = 12000000;
uint32_t spi_speed_general = 1200000; // jhdbg

// Use slower SPI speed when writing samples @ 1MHz
// uint32_t spi_speed_spi_sample_reg_write = 2000000;
uint32_t spi_speed_spi_sample_reg_write = 200000; // used when pull-ups slowed SPI lines


extern char g_cmd_buf[];
extern size_t volatile g_cmd_pos;

// TODO delete this after debug is done
extern void clear_dbg_buf();

/* iif functions */
int
ndp120_example_transfer(
    void *d, int mcu, uint32_t addr, void *out, void *in, unsigned int count)
{
    return spi_ndp_transfer(d, mcu, addr, out, in, count);
}

void *
ndp120_example_malloc(int size)
{
    return malloc(size);
}

void
ndp120_example_free(void *p)
{
    free(p);
}

int
ndp120_example_sync(void *d)
{
    return SYNTIANT_NDP_ERROR_NONE;
}

int
ndp120_example_unsync(void *d)
{
    return SYNTIANT_NDP_ERROR_NONE;
}

/* iif mailbox-exchange wait. this implementation just polls for mailbox
 * completion */
int
ndp120_example_mbwait(void *d)
{
    int s;
    uint32_t notifications = 0;
    
    /* bail out prematurely here, not iusing stock MCU & DSP code */
    return 0;

    int timeout = 1000;

    SYNSERIAL.println("mbwait called");

    do {
        s = syntiant_ndp_poll(ndp, &notifications, 1);
        check_status("Error syntiant_ndp_poll", s, 0);
        timeout--;
    } while (timeout && (notifications & SYNTIANT_NDP_NOTIFICATION_MAILBOX_IN) == 0);    
    return !!timeout;
}


int
ndp120_example_get_type(void *d, unsigned int *type)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    uint8_t type_byte = 0x34;
    uint8_t data;
    s = ndp120_example_transfer(d, 0, NDP120_SPI_ID0, NULL, &data, 1);
    if (!s) {
        if (data) {
            type_byte = data;
            SYNSERIAL.print("SPI ID read is 0x");
            SYNSERIAL.println(type_byte, HEX);
        } else {
            SYNSERIAL.print("SPI ID read as 0, assuming 0x");
            SYNSERIAL.println(type_byte, HEX);
        }
    }

    if (!s) {
        *type = type_byte;
    }
    return s;
}


/* function to use to communicate to NDP over SPI */
/* note that the Arduino functions used here will replace the bytes sent with 0xff 
   [when used with NDP1xx devices]. It's not currently an issue, just something to watch out for...
 */
int spi_ndp_transfer(void *d, int mcu, uint32_t address, void *_out,
                            void *_in, unsigned int count)
{
    uint8_t *out = (uint8_t *)_out;
    uint8_t *in = (uint8_t *)_in;
    int s = SYNTIANT_NDP_ERROR_NONE;
    uint8_t dummy[4] = {0};
    unsigned int i;

    if (in && out) {
        return SYNTIANT_NDP_ERROR_ARG;
    }

    if (mcu) {
        if ((count & 0x3) != 0) {
            return SYNTIANT_NDP_ERROR_ARG;
        }
        if (out) {
            nrf_gpio_pin_clear(SPI_CS);
            SPI.transfer(NDP120_SPI_MADDR(0));
            SPI.transfer(address & 0xff);
            SPI.transfer((address >> 8) & 0xff);
            SPI.transfer((address >> 16) & 0xff);
            SPI.transfer((address >> 24) & 0xff);
            SPI.transfer(out, count); // NB: overwrites out
            nrf_gpio_pin_set(SPI_CS);
        } else {
            nrf_gpio_pin_clear(SPI_CS);            
            SPI.transfer(NDP120_SPI_MADDR(0));
            SPI.transfer(address & 0xff);
            SPI.transfer((address >> 8) & 0xff);
            SPI.transfer((address >> 16) & 0xff);
            SPI.transfer((address >> 24) & 0xff);
            nrf_gpio_pin_set(SPI_CS);
            for(int i = 0; i < 4; i++); // short delay
            nrf_gpio_pin_clear(SPI_CS);
            // Reading 4 dummy bytes allows for running SPI faster
            // as this gives HW state maching time to fetch result
            SPI.transfer(0x80 | NDP120_SPI_MADDR(0));            
            SPI.transfer(dummy, 4);
            for (i = 0; i < count; i += 4) {
                SPI.transfer(&in[i], 4); 
            }
            nrf_gpio_pin_set(SPI_CS);
        }
    } else {
        if (out) {
            if (address == NDP120_SPI_SAMPLE) {
                SPI.beginTransaction(SPISettings(spi_speed_spi_sample_reg_write,
                                                 MSBFIRST, SPI_MODE0));
            }
            nrf_gpio_pin_clear(SPI_CS);
            SPI.transfer(address);
            SPI.transfer(out, count);  // NB: overwrites out
            nrf_gpio_pin_set(SPI_CS);
            if (address == NDP120_SPI_SAMPLE) {
                SPI.beginTransaction(SPISettings(spi_speed_general, MSBFIRST,
                                                 SPI_MODE0));
            }
        } else {
            nrf_gpio_pin_clear(SPI_CS);
            SPI.transfer(0x80 | address);
            SPI.transfer(in, count);
            nrf_gpio_pin_set(SPI_CS);
        }
    }
    return s;
}

void
get_config(void)
{
    char *label_data;    
    char fwver[STRING_LEN] = { 0 };
    char paramver[STRING_LEN] = { 0 };
    char pkgver[STRING_LEN] = { 0 };
    struct syntiant_ndp_config_s config;    
    int s;
    unsigned int i, j;
    
    label_data = (char *) malloc(STRING_LEN);
    if (!label_data) {
        fprintf(stderr, "unable to allocate label_data\n");
        exit(1);
    }

    labels = (char **) malloc(MAX_LABELS * sizeof(char *));
    if (!labels) {
        fprintf(stderr, "unable to allocate labels\n");
        exit(1);
    }

    memset(&config, 0, sizeof(struct syntiant_ndp_config_s));
    config.firmware_version = fwver;
    config.firmware_version_len = STRING_LEN;
    config.parameters_version = paramver;
    config.parameters_version_len = STRING_LEN;
    config.pkg_version = pkgver;
    config.pkg_version_len = STRING_LEN;
    config.labels = label_data;
    config.labels_len = STRING_LEN;

    s = syntiant_ndp_get_config(ndp, &config);
    check_status("get_config", s, 0);
    
    /* get pointers to the labels */
    j = 0;
    for (i = 0; i < config.classes; i++) {
        labels[i] = &label_data[j];
        for (; label_data[j]; j++)
            ;
        j++;
    }
    
    /* print status information obtained from package */
    SYNSERIAL.println("labels:");
    for (i = 0; i < config.classes; i++) {
        SYNSERIAL.print(" "); SYNSERIAL.println(labels[i]);
    }
}

int
spi_read_reg(void *d, uint32_t address, uint8_t *v)
{
    return spi_ndp_transfer(d, 0, address, NULL, v, 1);
}

int
spi_write_reg(void *d, uint32_t address, uint8_t v)
{
    return spi_ndp_transfer(d, 0, address, &v, NULL, 1);
}

int
mcu_read_reg(void *d, uint32_t address, uint32_t *v)
{
    return spi_ndp_transfer(d, 1, address, NULL, v, 4);
}

int
mcu_write_reg(void *d, uint32_t address, uint32_t v)
{
    return spi_ndp_transfer(d, 1, address, &v, NULL, 4);
}


int
config_fll(unsigned int ref_clk, unsigned int tar_clk)
{
    int trace = 1;
    char temp[200];
    uint32_t fllctl, fllsts;

    sprintf(temp, "Configuring FLL");
    SYNSERIAL.println(temp);

    tar_clk = tar_clk / 2;
    float freqMult = (float)tar_clk / (float)ref_clk;
    float freqDiv = (float)ref_clk / (float)tar_clk;

    /* Quantize to Q11.3 and Q0.14 */
    freqMult = (int)(freqMult * (1 << 3));
    freqDiv = (int)(freqDiv * (1 << 14));

    if (trace > 1) {
        sprintf(temp, "  FreqMult = %f FreqDiv = %f", freqMult / (1 << 3),
            freqDiv / (1 << 14));
        SYNSERIAL.println(temp);
    }

    /* set multipliers */
    mcu_read_reg(NULL, NDP120_CHIP_CONFIG_FLLCTL1, &fllctl);
    fllctl = NDP120_CHIP_CONFIG_FLLCTL1_WARMUPLIMIT_MASK_INSERT(fllctl, 4);
    fllctl = NDP120_CHIP_CONFIG_FLLCTL1_FREQMULT_MASK_INSERT(
        fllctl, (int)freqMult);
    fllctl
        = NDP120_CHIP_CONFIG_FLLCTL1_FREQDIV_MASK_INSERT(fllctl, (int)freqDiv);
    if (trace) {
        sprintf(temp, "Writing 0x%08lx to fllctl1. ", fllctl);
		SYNSERIAL.print("Writing to FLLCTL1" );
        SYNSERIAL.println(temp);
    }
    mcu_write_reg(NULL, NDP120_CHIP_CONFIG_FLLCTL1, fllctl);

    /* turn on clk */
    mcu_read_reg(NULL, NDP120_CHIP_CONFIG_FLLCTL0, &fllctl);
    fllctl = NDP120_CHIP_CONFIG_FLLCTL0_CLK_ENABLE_MASK_INSERT(fllctl, 1);
    fllctl = NDP120_CHIP_CONFIG_FLLCTL0_ENABLE_MASK_INSERT(fllctl, 1);
    fllctl = NDP120_CHIP_CONFIG_FLLCTL0_ENABLETRACKING_MASK_INSERT(fllctl, 1);
    mcu_write_reg(NULL, NDP120_CHIP_CONFIG_FLLCTL0, fllctl);

    int locked = 0;
    int loop_count = 0;
    do {
        mcu_read_reg(NULL, NDP120_CHIP_CONFIG_FLLCTL0, &fllsts);
        locked = NDP120_CHIP_CONFIG_FLLSTS0_LOCKED(fllsts);
        if (!locked) {
            sprintf(temp, "Still waiting for FLL lock after %3d ms", loop_count);
            SYNSERIAL.println(temp);
            delay(1);
            loop_count++;
        }
    } while (!locked);

    if (trace) {
        sprintf(temp, "FLL Locked after %d ms", loop_count);
        SYNSERIAL.println(temp);
    }
    return 0;
}


#ifdef CLK_EXT32K_PLL30M
static int
ndp120_config_clock(struct ndp120_app_pi_s *ndp120, options_t *options)
{
    int s = 0;
    syntiant_ndp120_config_clk_src_t config_clk_src;
    syntiant_ndp120_config_clk_pll_t config_clk_pll;
    syntiant_ndp120_config_clk_fll_t config_clk_fll;
    syntiant_ndp120_config_clk_xtal_t config_clk_xtal;
    struct syntiant_ndp_device_s *ndp = ndp120->ndp;
    uint8_t clock_option = options->clock_option;

    if (clock_option <= 13) { /* use PLL */
        if (options->use_xtal) {
            /* enable xtal */
            config_clk_xtal = (syntiant_ndp120_config_clk_xtal_t)
                    { 0, NDP120_CONFIG_SET_CLK_XTAL_OUT |
                         NDP120_CONFIG_SET_CLK_XTAL_OSC, 1, 1 };
            
            s = syntiant_ndp120_config_clk_xtal(ndp, &config_clk_xtal);
            if (s) goto error;
            
            delayMicroseconds(100000);

            /* Switch to XTAL pad, enable refclk only, leave clksel at fll */
            config_clk_src = (syntiant_ndp120_config_clk_src_t)
                    { 0, NDP120_CONFIG_SET_CLK_SRC_REFSEL,
                         1, 0, 0, 0, 0, 0, 0 };

            s = syntiant_ndp120_config_clk_src(ndp, &config_clk_src);
            if (s) goto error;

            config_clk_pll = (syntiant_ndp120_config_clk_pll_t) {
                0, 1, clock_option, 0, 0};

            /* lock then switch to pll without touching refclk field */
            s = syntiant_ndp120_config_clk_pll(ndp, &config_clk_pll);
            if (s) goto error;
        }
        else {
            config_clk_pll = (syntiant_ndp120_config_clk_pll_t) {
                0, 1, clock_option, 0, 0};

            th_printf("jhdbg: about to call syntiant_ndp120_config_clk_pll\r\n");
            s = syntiant_ndp120_config_clk_pll(ndp, &config_clk_pll);
            th_printf("jhdbg: syntiant_ndp120_config_clk_pll returned %d\r\n", s);
            if (s) goto error;

            config_clk_src = (syntiant_ndp120_config_clk_src_t)
                    { 0, NDP120_CONFIG_SET_CLK_SRC_CLKSEL |
                         NDP120_CONFIG_SET_CLK_SRC_EXTSEL,
                         0, 1, 0, 0, 0, 0, 0};
            th_printf("jhdbg: about to call syntiant_ndp120_config_clk_src\r\n");
            s = syntiant_ndp120_config_clk_src(ndp, &config_clk_src);
            th_printf("jhdbg: syntiant_ndp120_config_clk_src returned %d\r\n", s);
            if (s) goto error;
        }

        delayMicroseconds(500000);

        config_clk_pll = (syntiant_ndp120_config_clk_pll_t) {
            1, 0, 0, 0, 0};
        th_printf("jhdbg: about to call syntiant_ndp120_config_clk_pll\r\n");
        s = syntiant_ndp120_config_clk_pll(ndp, &config_clk_pll);
        th_printf("jhdbg: syntiant_ndp120_config_clk_pll returned %d\r\n", s);
        if (s) goto error;
    } else if ((clock_option <= 15) && (clock_option >= 14)) { /* use FLL */
        if (options->use_xtal) {
            /* enable xtal */
            config_clk_xtal = (syntiant_ndp120_config_clk_xtal_t)
                    { 0, NDP120_CONFIG_SET_CLK_XTAL_OUT |
                         NDP120_CONFIG_SET_CLK_XTAL_OSC, 1, 1 };
            s = syntiant_ndp120_config_clk_xtal(ndp, &config_clk_xtal);
            if (s) goto error;

            delayMicroseconds(100000);

            /* Switch to XTAL pad, enable refclk only, leave clksel at fll */
            config_clk_src = (syntiant_ndp120_config_clk_src_t)
                    { 0, NDP120_CONFIG_SET_CLK_SRC_REFSEL,
                         1, 0, 0, 0, 0, 0, 0 };
            s = syntiant_ndp120_config_clk_src(ndp, &config_clk_src);
            if (s) goto error;

            config_clk_fll = (syntiant_ndp120_config_clk_fll_t) {
                0, 1, clock_option - 14, 0};
            /* lock then switch to fll without touching refclk field */
            s = syntiant_ndp120_config_clk_fll(ndp, &config_clk_fll);
            if (s) goto error;
        }
        else {
            config_clk_fll = (syntiant_ndp120_config_clk_fll_t) {
                0, 1, clock_option - 14, 0};
            s = syntiant_ndp120_config_clk_fll(ndp, &config_clk_fll);
            if (s) goto error;

            config_clk_src = (syntiant_ndp120_config_clk_src_t)
                    { 0, NDP120_CONFIG_SET_CLK_SRC_CLKSEL |
                         NDP120_CONFIG_SET_CLK_SRC_EXTSEL,
                         0, 0, 0, 0, 0, 0, 0 };
            s = syntiant_ndp120_config_clk_src(ndp, &config_clk_src);
            if (s) goto error;
        }

        delayMicroseconds(500000);
        config_clk_fll = (syntiant_ndp120_config_clk_fll_t) {
            1, 0, 0, 0};
        s = syntiant_ndp120_config_clk_fll(ndp, &config_clk_fll);
        if (s) goto error;
    } else { /* external clock */
        config_clk_src = (syntiant_ndp120_config_clk_src_t)
              { 0, NDP120_CONFIG_SET_CLK_SRC_EXTSEL,
                0, 0, 1, 0, 0, 0, 0 };
        s = syntiant_ndp120_config_clk_src(ndp, &config_clk_src);
        if (s) goto error;
    }

error:
     return s;
}
#endif

int ndp_main(void)
{
#ifdef CLK_EXT32K_FLL30M
    uint32_t input_freq = 15996000;
#endif
    syntiant_ndp120_config_clk_src_t config_clk_src;
    
    int s;
    int on;
    unsigned int device_type;

    SYNSERIAL.println("NDP main");
    
    /* setup the integration interface functions */
    iif.d = ndp;
    iif.malloc = ndp120_example_malloc;
    iif.free = ndp120_example_free;
    iif.mbwait = ndp120_example_mbwait;
    iif.get_type = ndp120_example_get_type;
    iif.sync = ndp120_example_sync;
    iif.unsync = ndp120_example_unsync;
    iif.transfer = ndp120_example_transfer;

    /* initialize the ndp based on the interface functions */
    s = syntiant_ndp_init(&ndp, &iif, SYNTIANT_NDP_INIT_MODE_RESET);
    check_status("Error syntiant_ndp_init", s, 1);
    SYNSERIAL.println("NDP Initialized OK");

    options_t options;
    static struct ndp120_app_pi_s ndp120_app_state;
    memset(&ndp120_app_state, 0, sizeof(struct ndp120_app_pi_s));
    th_printf("Done with memset\r\n");
    ndp120_app_state.ndp = ndp;
    // f_in=32kHz, use PLL for all.  clock_option==2 =>0.9V/30MHz, ==11=>1.1V/98.3MHz
    options.clock_option = 11; 
    options.use_xtal = 0; // using external oscillator
    options.ndp120 = &ndp120_app_state;
    
    th_printf("About to configure clock\r\n");
    s = ndp120_config_clock(&ndp120_app_state, &options);
    th_printf("ndp120_config_clock returned %d\r\n", s);
    check_status("ndp_clocks_setup", s, 1);  

    uint32_t mcudata;

    spi_ndp_transfer(NULL, 1, 0x40009000, NULL, &mcudata, 4);
    SYNSERIAL.print("MCU Register 0x40009000: 0x");
    SYNSERIAL.println(mcudata, HEX);

    // read MCU register 0x40009004
    spi_ndp_transfer(NULL, 1, 0x40009004, NULL, &mcudata, 4);
    SYNSERIAL.print("MCU Register 0x40009004: 0x");
    SYNSERIAL.println(mcudata, HEX);

    on = SYNTIANT_NDP_INTERRUPT_DEFAULT;
    s = syntiant_ndp_interrupts(ndp, &on);
    check_status("Error syntiant_ndp_interrupts", s, 0);
    // packages to load
    struct {            
            uint8_t *package;
            unsigned int package_len;
            const char *package_str;
    } packages[] = {
            { dsp_firmware, dsp_firmware_len, "DSP" },
            { dnn_ndp120_params, dnn_ndp120_params_len, "DNN" }
    };
    SYNSERIAL.println("jhdbg: (2) ");    
    for (unsigned int j = 0; j < sizeof(packages) / sizeof(packages[0]); j++) { 
        char tmp[100];
        SYNSERIAL.println("jhdbg: (3): In for pkgs loop");
        // Reset package loading
        s = syntiant_ndp_load(ndp, NULL, 0);
        if (s != SYNTIANT_NDP_ERROR_MORE) {
            strcpy(tmp, "Error resetting package load state: ");
            strcat(tmp, packages[j].package_str);
            check_status(tmp, s, 0);
        }
        
        SYNSERIAL.print("jhdbg: (3): ");
        SYNSERIAL.println(packages[j].package_str);

        // Load package
        s = syntiant_ndp_load(ndp, packages[j].package, packages[j].package_len);
        if (s != SYNTIANT_NDP_ERROR_NONE && s != SYNTIANT_NDP_ERROR_MORE) {            
            strcpy(tmp, "Error loading ");
            strcat(tmp, packages[j].package_str);
            check_status(tmp, s, 0);
        } else {        
            SYNSERIAL.print(packages[j].package_str);
            SYNSERIAL.println(" loaded OK");
        }
    }

    // -- Check/Wait the MBOX out interupt
    // -- Clear the Interrupt
    // -- The value of the status should be RESP_RUNNING = 0x10 and the handhsake value = 0x80
    //      -- The handshake bit then gets reset to 0x0 
    //      -- The full value output form the MBOX out should be 0x90
    uint32_t data,cnt=50;

    for (cnt = 0; cnt < 50; cnt++) {
        // Check/Wait for MBOX out interrupt at boot
        syntiant_ndp120_read(ndp, 0, NDP120_SPI_INTSTS, &data);
        if (data & 0x4) {
            break;
        }
        delay(10);
    }
    
    syntiant_ndp120_write(ndp, 0, NDP120_SPI_INTSTS, data);
    syntiant_ndp120_read(ndp, 0, NDP120_SPI_MBOUT, &data);
    if (!(data & 0x90)) {
        SYNSERIAL.println("ERROR: Did not get running Response from DSP\n");
    } else {
        SYNSERIAL.println("DSP: BOOT CONFIRM\n");
    }
    
    syntiant_ndp120_write(ndp, 0, NDP120_SPI_MBIN, 0x0c | 0x00);
for (cnt = 0; cnt < 50; cnt++) {
    syntiant_ndp120_read(ndp, 0, NDP120_SPI_INTSTS, &data);
    if (data & 0x4) { // got interrupt
        break;
    }
    delay(10);
}
syntiant_ndp120_write(ndp, 0, NDP120_SPI_INTSTS, data); // clear interrupt status
if (!(data & 0x4)) {
        th_printf("error: didn't get interrupt\n");
        //goto error;
    }
    syntiant_ndp120_read(ndp, 0, NDP120_SPI_MBOUT, &data); // read mbout register for ack
    if (!(data & 0x7f)) {
        th_printf("got ack from dsp\n");
    } else {
        th_printf("error: error ack from dsp\n");
        //goto error;
}
    SYNSERIAL.println("jhdbg: (6): Past put_ext_clk_freq");
    get_config();

#if 0
    // configures external clock
    config_clk_src = (syntiant_ndp120_config_clk_src_t)
              { 0, NDP120_CONFIG_SET_CLK_SRC_EXTSEL,
                0, 0, 1, 0, 0, 0, 0 };
    s = syntiant_ndp120_config_clk_src(ndp, &config_clk_src);
        check_status("syntiant_ndp120_config_clk_src", s, 1);
    SYNSERIAL.print( "CLK src configured");
#endif

    SYNSERIAL.println("ndp_main all done!");
    return 0;
}

void setup()
{   
    // Initialize SYNSERIAL Port, speed value is arbitrary and doesn't affect
    // data transfer speed.
    // Update: It looks like this does set the port baud rate. 18FEB2022JHH
    delay(250);
# if EE_CFG_ENERGY_MODE==1
    SYNSERIAL.begin(9600);
# else  
    SYNSERIAL.begin(115200);
# endif
    SYNSERIAL.setTimeout(10000);
    // stream.setTimeout(2000);
    
    delay(250);
    SYNSERIAL.println("Mock-up Perf Target on Arduino BLE33");

    Wire.begin(); // i2c setup

    // Prepare for SPI
    nrf_gpio_cfg_output(SPI_CS);
    nrf_gpio_pin_set(SPI_CS);

    SPI.begin();
    SPI.beginTransaction(SPISettings(spi_speed_general, MSBFIRST, SPI_MODE0));   

    delay(100);

    // reset the NDP
    nrf_gpio_cfg_output(PORSTB);
    nrf_gpio_pin_clear(PORSTB);
    delay(300);
    nrf_gpio_pin_set(PORSTB);
    delay(300);

    for (int i = 0; i < 1; i++){
        uint8_t data;
        spi_ndp_transfer(NULL, 0, NDP120_SPI_ID0, NULL, &data, 1);
        SYNSERIAL.print("Data ");
        SYNSERIAL.println(data, HEX);
        delay(100); // was 5000 ??
    }

    ndp_main();
    ee_benchmark_initialize();  // setup for Tiny MLPerf benchmark
    // SYNSERIAL.println("Modified setup ending");
    // SYNSERIAL.println("Finished ee_benchmark_initialize");
    // delay(1000);  // added a delay
}


void loop()
{    
    int ch_perf_command;
    
    // this is the main loop from the Tiny-MLPerf runner
    if (SYNSERIAL.available()) {
      ch_perf_command = th_getchar();
      ee_serial_callback(ch_perf_command);
    }
}
