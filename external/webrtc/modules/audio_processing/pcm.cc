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

#include "webrtc/modules/audio_processing/include/audio_processing.h"
#include "tinyalsa/include/tinyalsa/asoundlib.h"
#include "webrtc/modules/include/module_common_types.h"
#include "webrtc/common_audio/include/audio_util.h"
#include <android/log.h>

#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

using namespace webrtc;
extern "C" {
/************************************************************************************/
static AudioProcessing* apm = NULL;
static volatile bool is_read = false, is_write = false;
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
    frame.id_ = 0;
    frame.sample_rate_hz_ = 48000;
    frame.num_channels_ = 2;
    memcpy(frame.data_, audio, size);
    frame.samples_per_channel_ = 480;
}

static void try_apm_enable(void)
{
    if(apm == NULL && is_read && is_write) {
        apm = AudioProcessing::Create();
        apm->Initialize(48000, 48000, 48000,
                AudioProcessing::ChannelLayout::kStereo,
                AudioProcessing::ChannelLayout::kStereo,
                AudioProcessing::ChannelLayout::kStereo
                );

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

static void try_apm_disable(void)
{
    if(apm != NULL) {
        delete apm;
        apm = NULL;
    }
}

#if 0
FILE *fr = NULL, *fw = NULL, *fe = NULL;
#define OPEN(f, path) do {if(f == NULL) f = fopen(path, "w");}while(0)
#define WRITE(f, buff, size) do{if(f != NULL) fwrite(buff, 1, size, f);}while(0)
#else
#define OPEN(f, path)
#define WRITE(f, buff, size) 
#endif

struct pcm *W_pcm_open_req(unsigned card, unsigned device,
        unsigned flags, struct pcm_config *config, int requested_rate)
{
    struct pcm *pcm;
    //try_apm_enable();

    //ALOGD("period_size : %d, %d", config->period_size, config->period_count);
    ALOGD("pcm_open_req period{%d, %d}, card{%d, %d, %x}", config->period_size, config->period_count, card, device, flags);

    //if((flags & PCM_IN) && !card && !device)
    //    config->period_size = 480;
    config->period_size = 1024;
    pcm = L_pcm_open_req(card, device, flags, config, requested_rate);
    config->period_size = 480;
    if(!card && !device) {
        ((flags & PCM_IN) ? capture : playback) = pcm;
        if(capture && playback) {
            OPEN(fw, "/data/goc/pcm/write.pcm");
            OPEN(fr, "/data/goc/pcm/read.pcm");
            OPEN(fe, "/data/goc/pcm/echo.pcm");
            //apm->Initialize();
        }
    }

    return pcm;
}

struct pcm *W_pcm_open(unsigned card, unsigned device,
        unsigned flags, struct pcm_config *config)
{
    struct pcm * pcm;
    //try_apm_enable();

    ALOGD("pcm_open period{%d, %d}, card{%d, %d, %x}", config->period_size, config->period_count, card, device, flags);

    //if((flags & PCM_IN) && !card && !device)
    config->period_size = 1024;
    pcm = L_pcm_open(card, device, flags, config);
    config->period_size = 480;
    if(!card && !device) {
        ((flags & PCM_IN) ? capture : playback) = pcm;
        if(capture && playback) {
            OPEN(fw, "/data/goc/pcm/write.pcm");
            OPEN(fr, "/data/goc/pcm/read.pcm");
            OPEN(fe, "/data/goc/pcm/echo.pcm");
            //apm->Initialize();
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
    if(pcm == capture && playback) {
        is_read = true;
        if(is_read && is_write) {
            try_apm_enable();
            //ALOGV("delay : {%d, %d, %d}, %d", playback_delay + capture_delay, playback_delay, capture_delay, count);
            AudioFrame frame;
            fixup((short*)data, count / 2);
            WRITE(fr, data, count);
            audioFrame(frame, data, count);
            apm->gain_control()->set_stream_analog_level(analog_level);
            apm->set_stream_delay_ms(40/*(playback_delay + capture_delay) / (48)*/);
            apm->ProcessStream(&frame);
            analog_level = apm->gain_control()->stream_analog_level();
            memcpy(data, frame.data_, count);
            WRITE(fe, data, count);
        }
    }
    return res;
}

int W_pcm_write(struct pcm *pcm, const void *data, unsigned count)
{
    if(pcm == playback && capture) {
        is_write = true;
        if(is_write && is_read) {
            try_apm_enable();
            AudioFrame frame;
            audioFrame(frame, (void*)data, (size_t)count);
            playback_delay = pcm_get_delay(playback) ?: 960;
            //ALOGV("playback %d,%d\n", playback_delay, count);
            apm->AnalyzeReverseStream(&frame);
            WRITE(fw, data, count);
        }
    }
    return L_pcm_write(pcm, data, count);
}

int W_pcm_close(struct pcm *pcm)
{
    if(capture == pcm) {
        capture = NULL;
        is_read = false;
        try_apm_disable();
    }
    if(playback == pcm) {
        capture = NULL;
        is_write = false;
        try_apm_disable();
    }
    return L_pcm_close(pcm);
}

}
