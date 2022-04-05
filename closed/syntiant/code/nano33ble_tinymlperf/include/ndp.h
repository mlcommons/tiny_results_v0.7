typedef struct options_s {
    struct ndp120_app_pi_s *ndp120;
    uint8_t clock_option;
    int agc_shift_dir;
    int agc_shift_cnt;
    int spi_read_delay;
    int spi_speed;
    int pdm_freq;
    uint32_t input_freq;
    int gpio;
#ifndef NO_I2C
    char *i2c_file;
    int i2c_addr;
#endif
    int dnn_input;
    int pdm_ndp;
    int pdm_in_shift;
    int pdm_out_shift;
    char *package_names;
    char *spi_dev;
    int spi_split;

    int aud2_src_type;
    int aud2_src_param;
    int aud2_enable;

    int aud2_out_mode;

    int i2s_framesize;
    int i2s_samplesize;
    int i2s_msb;
    int i2s_right_enable;
    int i2s_left_enable;
    int i2s_mode;
    int i2s_pack;
    int i2s_negedgeenable;
    int delayed_flop_sensitivity;

    /* int i2s_numdelay; */
    int i2s_zc;
    int i2s_shift_dir;
    int i2s_shift_cnt;

    int audio_sync_enable;
    int audio_sync_ref_chan;
    int audio_sync_adj_chan;
    int audio_sync_offset;
    int record_duration;

    int watch_enable;
    int watch_count;

    /* dsp tank size related */
    int32_t pcm_tank_size_in_msec;
    uint32_t aud_samp_size_bytes;
    uint32_t func_samp_size_bytes;
    uint32_t aud_samp_cap;
    uint32_t func_samp_cap;

    int dump_registers;

    char *play_filename;
    char *record_filename;
    int init_only;
    int use_xtal;
    enum syntiant_ndp_init_mode_e init_mode;
    int use_interrupt;
    char *mcu_test_package;
    uint8_t device_type;
} options_t;



enum {
};




int syntiant_ndp120_config_clk_xtal(struct syntiant_ndp_device_s *ndp, syntiant_ndp120_config_clk_xtal_t *config);




