/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_CODING_CODECS_G711_INCLUDE_AUDIO_DECODER_PCM_H_
#define WEBRTC_MODULES_AUDIO_CODING_CODECS_G711_INCLUDE_AUDIO_DECODER_PCM_H_

#include "webrtc/base/constructormagic.h"
#include "webrtc/modules/audio_coding/codecs/audio_decoder.h"


namespace webrtc {

class AudioDecoderPcm : public AudioDecoder {
 public:
	 explicit AudioDecoderPcm(size_t num_channels);
	 ~AudioDecoderPcm();
  void Reset() override;
  int PacketDuration(const uint8_t* encoded, size_t encoded_len) const override;
  size_t Channels() const override;

 protected:
  int DecodeInternal(const uint8_t* encoded,
                     size_t encoded_len,
                     int sample_rate_hz,
                     int16_t* decoded,
                     SpeechType* speech_type) override;

 private:
  const size_t num_channels_;
  RTC_DISALLOW_COPY_AND_ASSIGN(AudioDecoderPcm);

private:
	bool SpeexOpen();
	int SpeexDecode(const uint8_t* encoded,
		size_t encoded_len,
		int sample_rate_hz,
		int16_t* decoded);
	void SpeexClose();

	int SpeexEncodedDataSampleCount(const uint8_t* encoded,
		size_t encoded_len) const;

private:
//   SPEEX_HANDLE				m_pSpeex;
//   SpeexBits					m_speex_bits;
  void *   context_;
  int	   frame_bytes_;
};

class AudioDecoderPcmU final : public AudioDecoderPcm
{
public:
	AudioDecoderPcmU(size_t num_channels) : AudioDecoderPcm(num_channels) {}
};

class AudioDecoderPcmA final : public AudioDecoderPcm
{
public:
	AudioDecoderPcmA(size_t num_channels) : AudioDecoderPcm(num_channels) {}
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_AUDIO_CODING_CODECS_G722_INCLUDE_AUDIO_DECODER_G722_H_
