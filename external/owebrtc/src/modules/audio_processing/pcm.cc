/* pcm.c
**
** Copyright 2011, The Android Open Source Project
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of The Android Open Source Project nor the names of
**       its contributors may be used to endorse or promote products derived
**       from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY The Android Open Source Project ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL The Android Open Source Project BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
** DAMAGE.
*/

#define LOG_TAG "goc.pcm"

#include <stdio.h>
#include <android/log.h>

#include "tinyalsa/include/tinyalsa/asoundlib.h"
#include "webrtc/src/modules/audio_processing/interface/audio_processing.h"
#include "webrtc/src/modules/interface/module_common_types.h"

#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

using namespace webrtc;
extern "C" {
/************************************************************************************/
static AudioProcessing* apm = NULL;
static struct pcm *capture = NULL, *playback = NULL, *pcm1 = NULL;
static int analog_level = 0;
extern int pcm_link(struct pcm *,struct pcm*);
extern long pcm_get_delay(struct pcm *);
extern struct pcm *L_pcm_open_req(unsigned int card, unsigned int device,
                     unsigned int flags, struct pcm_config *config, int requested_rate);
extern struct pcm *L_pcm_open(unsigned int card, unsigned int device,
                     unsigned int flags, struct pcm_config *config);
extern int L_pcm_close(struct pcm *pcm);
extern int L_pcm_read(struct pcm *pcm, void *data, unsigned int count);
extern int L_pcm_read_ex(struct pcm *pcm, void *data, unsigned int count);
extern int L_pcm_write(struct pcm *pcm, const void *data, unsigned int count);

static inline void audioFrame(AudioFrame& frame,void *audio,size_t size)
{
    frame._id = 0;
    frame._frequencyInHz = 48000;
    frame._audioChannel = 2;
    memcpy(frame._payloadData, audio, size);
    frame._payloadDataLengthInSamples = 480;
}

static void try_apm_enable(void)
{
    if(apm == NULL) {
        apm = AudioProcessing::Create(0);
        apm->set_sample_rate_hz(48000);
        apm->set_num_channels(2,2);
        apm->set_num_reverse_channels(2);

        apm->level_estimator()->Enable(false);
        apm->high_pass_filter()->Enable(true);
        apm->echo_cancellation()->enable_metrics(false);
        apm->echo_cancellation()->enable_drift_compensation(false);
        apm->echo_cancellation()->set_suppression_level(EchoCancellation::kHighSuppression);
        apm->echo_cancellation()->Enable(true);

        apm->noise_suppression()->set_level(NoiseSuppression::kVeryHigh);
        apm->noise_suppression()->Enable(true);

        apm->gain_control()->set_analog_level_limits(0,255);
        apm->gain_control()->set_mode(GainControl::kAdaptiveAnalog);
        apm->gain_control()->Enable(true);
    }
}

FILE *fd = NULL, *fs = NULL;

struct pcm *W_pcm_open_req(unsigned card, unsigned device,
        unsigned flags, struct pcm_config *config, int requested_rate)
{
    struct pcm *pcm;
    try_apm_enable();

    //ALOGD("period_size : %d, %d", config->period_size, config->period_count);
    ALOGD("pcm_open_req period{%d, %d}, card{%d, %d, %x}", config->period_size, config->period_count, card, device, flags);

    if((flags & PCM_IN) && !card && !device)
        config->period_size = 480;
    if(fd == NULL) fd = fopen("/data/goc/mic1.pcm", "w");
    if(fs == NULL) fs = fopen("/data/goc/mic0.pcm", "w");
    pcm = L_pcm_open_req(card, device, flags, config, requested_rate);
    if(!card && !device) {
        ((flags & PCM_IN) ? capture : playback) = pcm;
        if(capture && playback) {
            apm->Initialize();
            //pcm_stop(capture);
            //pcm_stop(playback);
           // int res = pcm_link(capture, playback);
           // ALOGD("pcm_link : %s\n", pcm_get_error(capture));
        }
    }

    return pcm;
}

struct pcm *W_pcm_open(unsigned card, unsigned device,
        unsigned flags, struct pcm_config *config)
{
    struct pcm * pcm;
    try_apm_enable();

    ALOGD("pcm_open period{%d, %d}, card{%d, %d, %x}", config->period_size, config->period_count, card, device, flags);

    if(fd == NULL) fd = fopen("/data/goc/mic1.pcm", "w");
    if(fs == NULL) fs = fopen("/data/goc/mic0.pcm", "w");
    if((flags & PCM_IN) && !card && !device)
        config->period_size = 480;
    pcm = L_pcm_open(card, device, flags, config);
    if(!card && !device) {
        ((flags & PCM_IN) ? capture : playback) = pcm;
        if(capture && playback) {
            apm->Initialize();
            //pcm_stop(capture);
            //pcm_stop(playback);
            // int res = pcm_link(capture, playback);
            //ALOGD("pcm_link : %s\n", pcm_get_error(capture));
        }
    }

    return pcm;
}

static long playback_delay = 0, capture_delay = 0;

static void fixup(short audio[], int count)
{
    for(int i = 0;i < count;i += 2)
        audio[i + 1] = audio[i];

}

int W_pcm_read(struct pcm *pcm, void *data, unsigned count)
{
    capture_delay = pcm_get_delay(pcm) ?: 240;
    int res = L_pcm_read(pcm, data, count);
    fwrite(data, 1, count, fs);
    if(pcm == capture && playback) {
        int res = 0;
        //ALOGV("delay : {%d, %d, %d}, %d", playback_delay + capture_delay, playback_delay, capture_delay, count);
        AudioFrame frame;
        fixup((short*)data, count / 2);
        audioFrame(frame, data, count);
        apm->gain_control()->set_stream_analog_level(analog_level);
        apm->set_stream_delay_ms(30/*(playback_delay + capture_delay) / (48)*/);
        res = apm->ProcessStream(&frame);
        ALOGV("W_pcm_read : %d", res);
        analog_level = apm->gain_control()->stream_analog_level();
        memcpy(data, frame._payloadData, count);
        fwrite(data, 1, count, fd);
    }
    return res;
}

int W_pcm_write(struct pcm *pcm, const void *data, unsigned count)
{
    int res = 0;
    if(pcm == playback && capture) {
        AudioFrame frame;
        audioFrame(frame, (void*)data, (size_t)count);
        playback_delay = pcm_get_delay(playback) ?: 960;
        //ALOGV("playback %d,%d\n", playback_delay, count);
        res = apm->AnalyzeReverseStream(&frame);
        ALOGV("W_pcm_write : %d", res);
    }
    return L_pcm_write(pcm, data, count);
}

int W_pcm_close(struct pcm *pcm)
{
    if(capture == pcm) {
        capture = NULL;
    }
    if(playback == pcm) {
        playback = NULL;
    }
    return L_pcm_close(pcm);
}

}
