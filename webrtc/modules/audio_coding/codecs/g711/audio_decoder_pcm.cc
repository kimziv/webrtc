/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "speex_def.h"
#include "webrtc/modules/audio_coding/codecs/g711/audio_decoder_pcm.h"

#include "webrtc/base/checks.h"

#include "webrtc/system_wrappers/include/trace.h"

#include <stdio.h>

namespace webrtc {

	int g_frame_bytes_ = 0;

AudioDecoderPcm::AudioDecoderPcm(size_t num_channels)
    : num_channels_(num_channels) {
	context_ = new speex_context;
	((speex_context *)context_)->pSpeex = nullptr;
	frame_bytes_ = 0;
	//SpeexBits					m_speex_bits;
  RTC_DCHECK_GE(num_channels, 1u);
  WEBRTC_TRACE(kTraceInfo, kTraceVoice, 0,
	  "AudioDecoderPcm::AudioDecoderPcm() ");
}

AudioDecoderPcm::~AudioDecoderPcm()
{
	WEBRTC_TRACE(kTraceInfo, kTraceVoice, 0,
		"AudioDecoderPcm::~AudioDecoderPcm() ");
	SpeexClose();
	delete ((speex_context *)context_);
}

void AudioDecoderPcm::Reset() {
	SpeexClose();
	SpeexOpen();
}

size_t AudioDecoderPcm::Channels() const {
  return num_channels_;
}

int AudioDecoderPcm::DecodeInternal(const uint8_t* encoded,
                                       size_t encoded_len,
                                       int sample_rate_hz,
                                       int16_t* decoded,
                                       SpeechType* speech_type) {


 	WEBRTC_TRACE(kTraceInfo, kTraceVoice, 0,
 		"AudioDecoderPcm::DecodeInternal(%d bytes buffer) ", encoded_len);
  RTC_DCHECK(sample_rate_hz == 16000)
      << "Unsupported sample rate " << sample_rate_hz;

  //size_t ret = WebRtcPcm16b_Decode(encoded, encoded_len, decoded);
  if (((speex_context *)context_)->pSpeex == NULL)
	  SpeexOpen();
  int sample_count = SpeexDecode(encoded, encoded_len, sample_rate_hz, decoded);

//  	{
//  		static FILE * f = NULL;
//  		if (f == NULL) f = fopen("/sdcard/test-render.pcm", "wb+");
//  		if (f != NULL) {
//  			fwrite(decoded, 2, sample_count, f);
//  		}
//  	}
  frame_bytes_ = sample_count / 320;

  *speech_type = ConvertSpeechType(1);
  return sample_count;
}

int AudioDecoderPcm::PacketDuration(const uint8_t* encoded,
                                       size_t encoded_len) const {
  // Two encoded byte per sample per channel.

	return -1;

	int frame_bytes = frame_bytes_;
	if (frame_bytes == 0)
	{
		frame_bytes = g_frame_bytes_;
	}

	if (frame_bytes == 0 || (encoded_len % frame_bytes != 0 && encoded_len % frame_bytes != 1))
	{
		int frame_count = SpeexEncodedDataSampleCount(encoded, encoded_len) / 320;
		if (g_frame_bytes_ == 0)
			g_frame_bytes_ = encoded_len / frame_count;
	}

	return static_cast<int>(encoded_len / frame_bytes * 320);
}


bool AudioDecoderPcm::SpeexOpen()
{
	((speex_context *)context_)->pSpeex = speex_decoder_init(&speex_wb_mode);
	int enh_enabled = 1;
	speex_decoder_ctl(((speex_context *)context_)->pSpeex, SPEEX_SET_ENH, &enh_enabled);
	return true;
}

int AudioDecoderPcm::SpeexDecode(const uint8_t* encoded,
	size_t encoded_len,
	int sample_rate_hz,
	int16_t* decoded)
{
	if (((speex_context *)context_)->pSpeex == nullptr) return 0;

	speex_bits_set_bit_buffer(&((speex_context *)context_)->speex_bits, (void *)encoded, encoded_len);

	int size = 0;
	int iRemain = 1;
	while (iRemain)
	{
		int iRet = speex_decode_int(((speex_context *)context_)->pSpeex, &((speex_context *)context_)->speex_bits,
			(spx_int16_t*)(decoded + size));
		if (0 != iRet)
		{
			break;
		}

		size += 320;

		iRemain = speex_bits_remaining(&((speex_context *)context_)->speex_bits);
	}
//static FILE * f = NULL;
//if (f == NULL) f = fopen("/sdcard/test.pcm", "wb+");
//if (f != NULL) fwrite(decoded, 1, size, f);
	return size;
}

void AudioDecoderPcm::SpeexClose()
{
	if (((speex_context *)context_)->pSpeex)
		speex_decoder_destroy(((speex_context *)context_)->pSpeex);
	((speex_context *)context_)->pSpeex = NULL;
}

int AudioDecoderPcm::SpeexEncodedDataSampleCount(const uint8_t* encoded,
	size_t encoded_len) const
{
	SpeexBits					speex_bits;
	SPEEX_HANDLE	pSpeex = speex_decoder_init(&speex_wb_mode);
	int enh_enabled = 1;
	speex_decoder_ctl(pSpeex, SPEEX_SET_ENH, &enh_enabled);

	speex_bits_set_bit_buffer(&speex_bits, (void *)encoded, encoded_len);

	spx_int16_t decoded[400];
	int ret = 0;
	int iRemain = 1;
	while (iRemain)
	{
		int iRet = speex_decode_int(pSpeex, &speex_bits,
			decoded);
		if (0 != iRet)
		{
			break;
		}
		iRemain = speex_bits_remaining(&speex_bits);
		ret += 320;
	}
	if (pSpeex)
		speex_decoder_destroy(pSpeex);

	return ret;
}

}  // namespace webrtc
