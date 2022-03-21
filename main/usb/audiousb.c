/**
 * @file audiousb.c
 * 
 */

#include <stdint.h>
#include <stdbool.h>
#include "arm_math.h"
#include "arm_const_structs.h"

#include "audiousb.h"
#include "amk_printf.h"

#ifndef AUDIO_DEBUG
#define AUDIO_DEBUG 1
#endif

#if AUDIO_DEBUG
#define audio_debug  amk_printf
#else
#define audio_debug(...)
#endif

#ifdef TINYUSB_ENABLE

#include "tusb.h"

// Unit numbers are arbitrary selected
#define UAC2_ENTITY_CLOCK               0x04

// Speaker path
#define UAC2_ENTITY_SPK_INPUT_TERMINAL  0x01
#define UAC2_ENTITY_SPK_FEATURE_UNIT    0x02
#define UAC2_ENTITY_SPK_OUTPUT_TERMINAL 0x03

#define N_SAMPLE_RATES                  1

enum {
    VOLUME_CTRL_0_DB = 0,
    VOLUME_CTRL_10_DB = 2560,
    VOLUME_CTRL_20_DB = 5120,
    VOLUME_CTRL_30_DB = 7680,
    VOLUME_CTRL_40_DB = 10240,
    VOLUME_CTRL_50_DB = 12800,
    VOLUME_CTRL_60_DB = 15360,
    VOLUME_CTRL_70_DB = 17920,
    VOLUME_CTRL_80_DB = 20480,
    VOLUME_CTRL_90_DB = 23040,
    VOLUME_CTRL_100_DB = 25600,
    VOLUME_CTRL_SILENCE = 0x8000,
};

//#define AUDIO_BUFFER_SIZE   (48*8)
//#define AUDIO_FFT_SIZE      128
//#define ARM_CFFT            arm_cfft_sR_q15_len128

#define AUDIO_BUFFER_SIZE   (48*16)
#define AUDIO_FFT_SIZE      256
//#define ARM_CFFT            arm_cfft_sR_q15_len256
#define ARM_CFFT            arm_cfft_sR_f32_len256

arm_cfft_radix4_instance_f32 arm_cfft;

typedef struct {
    uint32_t sample_rate;
    int16_t volume[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX+1];   // +1 for master channel 0
    int8_t mute[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX+1];      // +1 for master channel 0
    // the pcm data were 1 channel, 16bits, 48000hz, thus one frame take 96 bytes, 
    // to use the 128 points FFT, the 48*2*8=768 bytes buffer were chosed
    uint16_t offset;    // offset from current buffer
    uint16_t valid;     // valid data in buffer
    uint16_t buffer[AUDIO_BUFFER_SIZE];
} audio_state_t;

static audio_state_t audio_state = {CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, {0}, {0}, 0, 0, {0}};

void audio_init(void)
{
}

void output_mag_data_q15(q15_t* data, uint32_t size)
{
    amk_printf("====================================\r\n");
    for (int i = 0; i < size/16; i++) {
        for (int j = 0; j < 16; j++) {
            amk_printf("\t%d", data[i*16+j]);
        }
        amk_printf("\r\n");
    }
    amk_printf("------------------------------------r\n");
}

void output_mag_data_f32(float32_t* data, uint32_t size)
{
    amk_printf("====================================\r\n");
    for (int i = 0; i < size/8; i++) {
        for (int j = 0; j < 8; j++) {
            amk_printf("%+16.4f", data[i*8+j]);
        }
        amk_printf("\r\n");
    }
    amk_printf("------------------------------------\r\n");
}

#if 0

void audio_task(void)
{
    static q15_t work_buf[AUDIO_FFT_SIZE*2];
    static q15_t results[AUDIO_FFT_SIZE];
    if (audio_state.valid >= AUDIO_FFT_SIZE) {
        audio_debug("Audio data: valid=%d, offset=%d\r\n", audio_state.valid, audio_state.offset);
        for(int i = 0; i < AUDIO_FFT_SIZE; i++) {
            work_buf[2*i] = audio_state.buffer[audio_state.offset+i] >> 1;
            work_buf[2*i+1] = 0;
        }
        audio_state.valid -= AUDIO_FFT_SIZE;
        audio_state.offset = (audio_state.offset + AUDIO_FFT_SIZE) % AUDIO_BUFFER_SIZE;
#if 1
    /* Process the data through the CFFT/CIFFT module */
    arm_cfft_q15(&ARM_CFFT, work_buf, 0, 1);

    /* Process the data through the Complex Magnitude Module for
    calculating the magnitude at each bin */
    arm_cmplx_mag_q15(work_buf, results, AUDIO_FFT_SIZE);
    output_mag_data(results, AUDIO_FFT_SIZE/2);
#endif
    }


}

#endif

void fft_test(float32_t *work_buf, float32_t *results)
{
    #define PI2 6.28318530717959
    #define FS 1024
    for (int i = 0; i < AUDIO_FFT_SIZE; i++) {
        work_buf[2*i]=15 + 10*arm_sin_f32(PI2*i*100/FS) + 5.5*arm_sin_f32(PI2*i*150/FS);
        //work_buf[2*i] = 1500 * sin(PI2 * i * 350.0 / FS) + 2700 * sin(PI2 * i * 8400.0 / FS) + 4000 * sin(PI2 * i * 18725.0 / FS);
        work_buf[2*i+1] = 0.0;
    }

    arm_cfft_radix4_init_f32(&arm_cfft, AUDIO_FFT_SIZE, 0, 1);
    arm_cfft_radix4_f32(&arm_cfft, work_buf);

#if 0
    arm_cfft_f32(&ARM_CFFT, work_buf, 0, 0);
#endif

    /* Process the data through the Complex Magnitude Module for
    calculating the magnitude at each bin */
    arm_cmplx_mag_f32(work_buf, results, AUDIO_FFT_SIZE);
    output_mag_data_f32(results, AUDIO_FFT_SIZE);
}

void audio_task(void)
{
    static float32_t work_buf[AUDIO_FFT_SIZE*2];
    static float32_t results[AUDIO_FFT_SIZE];
    if (audio_state.valid >= AUDIO_FFT_SIZE) {
        audio_debug("Audio data: valid=%d, offset=%d\r\n", audio_state.valid, audio_state.offset);
        for(int i = 0; i < AUDIO_FFT_SIZE; i++) {
            work_buf[2*i] = (float32_t)(audio_state.buffer[audio_state.offset+i]/65536.0);
            work_buf[2*i+1] = 0;
        }
        audio_state.valid -= AUDIO_FFT_SIZE;
        audio_state.offset = (audio_state.offset + AUDIO_FFT_SIZE) % AUDIO_BUFFER_SIZE;

#if 1
        /* Process the data through the CFFT/CIFFT module */
        arm_cfft_radix4_init_f32(&arm_cfft, AUDIO_FFT_SIZE, 0, 1);
        arm_cfft_radix4_f32(&arm_cfft, work_buf);
        //arm_cfft_f32(&ARM_CFFT, work_buf, 0, 0);

        /* Process the data through the Complex Magnitude Module for
        calculating the magnitude at each bin */
        arm_cmplx_mag_f32(work_buf, results, AUDIO_FFT_SIZE);
        //arm_cmplx_mag_f32(work_buf, results, AUDIO_FFT_SIZE);
        output_mag_data_f32(results, AUDIO_FFT_SIZE);
#else
        fft_test(work_buf, results);
#endif
    }
}

// Helper for clock get requests
static bool tud_audio_clock_get_request(uint8_t rhport, audio_control_request_t const *request)
{
    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ) {
        if (request->bRequest == AUDIO_CS_REQ_CUR) {
            audio_debug("Clock get current freq %lu\r\n", audio_state.sample_rate);

            audio_control_cur_4_t curf = { tu_htole32(audio_state.sample_rate) };
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &curf, sizeof(curf));
        } else if (request->bRequest == AUDIO_CS_REQ_RANGE) {
            audio_control_range_4_n_t(N_SAMPLE_RATES) rangef =
            {
                .wNumSubRanges = tu_htole16(N_SAMPLE_RATES)
            };
            audio_debug("Clock get %d freq ranges\r\n", N_SAMPLE_RATES);
            for(uint8_t i = 0; i < N_SAMPLE_RATES; i++)
            {
                rangef.subrange[i].bMin = audio_state.sample_rate;
                rangef.subrange[i].bMax = audio_state.sample_rate;
                rangef.subrange[i].bRes = 0;
                audio_debug("Range %d (%d, %d, %d)\r\n", i, (int)rangef.subrange[i].bMin, (int)rangef.subrange[i].bMax, (int)rangef.subrange[i].bRes);
            }
            
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &rangef, sizeof(rangef));
        }
    } else if (request->bControlSelector == AUDIO_CS_CTRL_CLK_VALID &&
            request->bRequest == AUDIO_CS_REQ_CUR) {
        audio_control_cur_1_t cur_valid = { .bCur = 1 };
        audio_debug("Clock get is valid %u\r\n", cur_valid.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_valid, sizeof(cur_valid));
    }

    audio_debug("Clock get request not supported, entity = %u, selector = %u, request = %u\r\n",
                request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
}

// Helper for clock set requests
static bool tud_audio_clock_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
    (void)rhport;

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ) {

        audio_state.sample_rate = ((audio_control_cur_4_t const *)buf)->bCur;

        audio_debug("Clock set current freq: %ld\r\n", audio_state.sample_rate);

        return true;
    } else {
        audio_debug("Clock set request not supported, entity = %u, selector = %u, request = %u\r\n",
                request->bEntityID, request->bControlSelector, request->bRequest);
        return false;
    }
}

// Helper for feature unit get requests
static bool tud_audio_feature_unit_get_request(uint8_t rhport, audio_control_request_t const *request)
{
    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE && request->bRequest == AUDIO_CS_REQ_CUR) {
        audio_control_cur_1_t mute1 = { .bCur = audio_state.mute[request->bChannelNumber] };
        audio_debug("Get channel %u mute %d\r\n", request->bChannelNumber, mute1.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &mute1, sizeof(mute1));
    } else if (UAC2_ENTITY_SPK_FEATURE_UNIT && request->bControlSelector == AUDIO_FU_CTRL_VOLUME) {
        if (request->bRequest == AUDIO_CS_REQ_RANGE) {
            audio_control_range_2_n_t(1) range_vol = {
                .wNumSubRanges = tu_htole16(1),
                .subrange[0] = { .bMin = tu_htole16(-VOLUME_CTRL_50_DB), tu_htole16(VOLUME_CTRL_0_DB), tu_htole16(256) }
            };
            audio_debug("Get channel %u volume range (%d, %d, %u) dB\r\n", request->bChannelNumber,
                    range_vol.subrange[0].bMin / 256, range_vol.subrange[0].bMax / 256, range_vol.subrange[0].bRes / 256);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &range_vol, sizeof(range_vol));
        } else if (request->bRequest == AUDIO_CS_REQ_CUR) {
            audio_control_cur_2_t cur_vol = { .bCur = tu_htole16(audio_state.volume[request->bChannelNumber]) };
            audio_debug("Get channel %u volume %d dB\r\n", request->bChannelNumber, cur_vol.bCur / 256);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_vol, sizeof(cur_vol));
        }
    }

    audio_debug("Feature unit get request not supported, entity = %u, selector = %u, request = %u\r\n",
                request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

// Helper for feature unit set requests
static bool tud_audio_feature_unit_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
    (void)rhport;

    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE) {
        audio_state.mute[request->bChannelNumber] = ((audio_control_cur_1_t const *)buf)->bCur;

        audio_debug("Set channel %d Mute: %d\r\n", request->bChannelNumber, audio_state.mute[request->bChannelNumber]);
        return true;
    } else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME) {
        audio_state.volume[request->bChannelNumber] = ((audio_control_cur_2_t const *)buf)->bCur;
        audio_debug("Set channel %d volume: %d dB\r\n", request->bChannelNumber, audio_state.volume[request->bChannelNumber] / 256);
        return true;
    } else {
        audio_debug("Feature unit set request not supported, entity = %u, selector = %u, request = %u\r\n",
                    request->bEntityID, request->bControlSelector, request->bRequest);
        return false;
    }
}

//--------------------------------------------------------------------+
// Application Callback API Implementations
//--------------------------------------------------------------------+

// Invoked when audio class specific get request received for an entity
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    audio_control_request_t const *request = (audio_control_request_t const *)p_request;

    if (request->bEntityID == UAC2_ENTITY_CLOCK)
        return tud_audio_clock_get_request(rhport, request);
    if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT)
        return tud_audio_feature_unit_get_request(rhport, request);
    else
    {
        audio_debug("Get request not handled, entity = %d, selector = %d, request = %d\r\n",
                    request->bEntityID, request->bControlSelector, request->bRequest);
    }
    return false;
}

// Invoked when audio class specific set request received for an entity
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *buf)
{
    audio_control_request_t const *request = (audio_control_request_t const *)p_request;

    if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT)
        return tud_audio_feature_unit_set_request(rhport, request, buf);

    if (request->bEntityID == UAC2_ENTITY_CLOCK)
        return tud_audio_clock_set_request(rhport, request, buf);

    audio_debug("Set request not handled, entity = %d, selector = %d, request = %d\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

bool tud_audio_set_itf_close_EP_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
    (void)rhport;

    uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    audio_debug("Audio set itf close ep: itf=%d, alt=%d\r\n", itf, alt);

    return true;
}

bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
    (void)rhport;
    uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    audio_debug("Audio set interface: itf=%d, alt=%d\r\n", itf, alt);

    return true;
}

bool tud_audio_rx_done_pre_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)func_id;
    (void)cur_alt_setting;

    int32_t readed = tud_audio_read(&audio_state.buffer[audio_state.offset], n_bytes_received);
    audio_state.valid += readed / 2;
    audio_debug("Audio data readed: ep=%d, received=%d, readed=%ld\r\n", ep_out, n_bytes_received, readed);
    return true;
}

bool tud_audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)itf;
    (void)ep_in;
    (void)cur_alt_setting;

    audio_debug("Audio data tx prepare, should not be here\r\n");
    return true;
}

#endif