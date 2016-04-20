/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_CODING_CODECS_G711_INCLUDE_AUDIO_ENCODER_PCM_H_
#define WEBRTC_MODULES_AUDIO_CODING_CODECS_G711_INCLUDE_AUDIO_ENCODER_PCM_H_

#include "webrtc/base/scoped_ptr.h"
//#include "webrtc/modules/audio_coding/codecs/speex/include/audio_encoder_pcm.h"
#include "webrtc/modules/audio_coding/codecs/audio_encoder.h"

namespace webrtc {

struct CodecInst;

class AudioEncoderSpeex : public AudioEncoder{
public:
	struct Config {
	public:
		bool IsOk() const;

		int frame_size_ms;
		int num_channels;
		int payload_type;
		int sample_rate_hz;

		Config()
			: frame_size_ms(20), num_channels(1), payload_type(97), sample_rate_hz(16000) {}
	};

	~AudioEncoderSpeex() override;

	size_t MaxEncodedBytes() const override;
	int SampleRateHz() const override;
	size_t NumChannels() const override;
  size_t Num10MsFramesInNextPacket() const override;
  size_t Max10MsFramesInAPacket() const override;
  int GetTargetBitrate() const override;
  EncodedInfo EncodeInternal(uint32_t rtp_timestamp,
                             rtc::ArrayView<const int16_t> audio,
                             size_t max_encoded_bytes,
                             uint8_t* encoded) override;
  void Reset() override;

 private:
	const int sample_rate_hz_;
	const int num_channels_;
	const int payload_type_;
	const size_t num_10ms_frames_per_packet_;
	const size_t full_frame_samples_;
	std::vector<int16_t> speech_buffer_;
	uint32_t first_timestamp_in_buffer_;

//////////////////////////////////////////////////////////////////////////
	//107 pt
 public:
	 AudioEncoderSpeex(const Config& config);
	 AudioEncoderSpeex(const CodecInst& codec_inst);

 protected:
  size_t EncodeCall(const int16_t* audio,
                    size_t input_len,
                    uint8_t* encoded);

  int BytesPerSample() const;


private:
	bool SpeexOpen();
	void SpeexClose();

	int SpeexEncode(const int16_t* audio,
		size_t input_len,
		uint8_t* encoded);



private:
	void * context_;

private:
	RTC_DISALLOW_COPY_AND_ASSIGN(AudioEncoderSpeex);
};


class AudioEncoderPcmA : public AudioEncoderSpeex
{
public:
	AudioEncoderPcmA(const CodecInst& codec_inst) : AudioEncoderSpeex(codec_inst)
	{
	}
};
class AudioEncoderPcmU : public AudioEncoderSpeex
{
public:
	AudioEncoderPcmU(const CodecInst& codec_inst) : AudioEncoderSpeex(codec_inst)
	{
	}
};




#include <vector>

#include "webrtc/base/constructormagic.h"
#include "webrtc/modules/audio_coding/codecs/audio_encoder.h"


class AudioEncoderPcm : public AudioEncoder {
public:
	struct Config {
	public:
		bool IsOk() const;

		int frame_size_ms;
		size_t num_channels;
		int payload_type;

	protected:
		explicit Config(int pt)
			: frame_size_ms(20), num_channels(1), payload_type(pt) {}
	};

	~AudioEncoderPcm() override;

	size_t MaxEncodedBytes() const override;
	int SampleRateHz() const override;
	size_t NumChannels() const override;
	size_t Num10MsFramesInNextPacket() const override;
	size_t Max10MsFramesInAPacket() const override;
	int GetTargetBitrate() const override;
	EncodedInfo EncodeInternal(uint32_t rtp_timestamp,
		rtc::ArrayView<const int16_t> audio,
		size_t max_encoded_bytes,
		uint8_t* encoded) override;
	void Reset() override;

protected:
	AudioEncoderPcm(const Config& config, int sample_rate_hz);

	virtual size_t EncodeCall(const int16_t* audio,
		size_t input_len,
		uint8_t* encoded) = 0;

	virtual size_t BytesPerSample() const = 0;

private:
	const int sample_rate_hz_;
	const size_t num_channels_;
	const int payload_type_;
	const size_t num_10ms_frames_per_packet_;
	const size_t full_frame_samples_;
	std::vector<int16_t> speech_buffer_;
	uint32_t first_timestamp_in_buffer_;
};

}  // namespace webrtc
#endif  // WEBRTC_MODULES_AUDIO_CODING_CODECS_G722_INCLUDE_AUDIO_ENCODER_G722_H_
