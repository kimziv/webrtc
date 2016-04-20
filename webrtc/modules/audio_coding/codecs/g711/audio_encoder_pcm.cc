/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


#include "webrtc/base/checks.h"
#include "webrtc/common_types.h"

#include "speex_def.h"
#include "webrtc/modules/audio_coding/codecs/g711/audio_encoder_pcm.h"
#include "webrtc/system_wrappers/include/trace.h"

#define SPEEX_FRAME_COUTN_PER_ENCODE 2

namespace webrtc {

namespace speex{
	int16_t NumSamplesPerFrame(int num_channels,
		int frame_size_ms,
		int sample_rate_hz) {
		int samples_per_frame = num_channels * frame_size_ms * sample_rate_hz / 1000;
		RTC_CHECK_LE(samples_per_frame, std::numeric_limits<int16_t>::max())
			<< "Frame size too large.";
		return static_cast<int16_t>(samples_per_frame);
	}

// 	template <typename T>
// 	typename T::Config CreateConfig(const CodecInst& codec_inst) {
// 		typename T::Config config;
// 		config.frame_size_ms = codec_inst.pacsize / 8;
// 		config.num_channels = codec_inst.channels;
// 		config.payload_type = codec_inst.pltype;
// 		return config;
// 	}

}  // namespace

// bool AudioEncoderSpeex::Config::IsOk() const {
// 	return (frame_size_ms % 10 == 0) && (num_channels >= 1);
// }

AudioEncoderSpeex::AudioEncoderSpeex(const Config& config)
	: sample_rate_hz_(16000),
	num_channels_(config.num_channels),
	payload_type_(config.payload_type),
	num_10ms_frames_per_packet_(
	static_cast<size_t>(config.frame_size_ms / 10)),
	full_frame_samples_(speex::NumSamplesPerFrame(config.num_channels,
	config.frame_size_ms,
	sample_rate_hz_)),
	first_timestamp_in_buffer_(0) {
	//RTC_CHECK_GT(sample_rate_hz, 0) << "Sample rate must be larger than 0 Hz";
	RTC_CHECK_EQ(config.frame_size_ms % 10, 0)
		<< "Frame size must be an integer multiple of 10 ms.";
	speech_buffer_.reserve(full_frame_samples_);

	context_ = new speex_context;

	WEBRTC_TRACE(kTraceInfo, kTraceVoice, 0,
		"AudioEncoderSpeex::AudioEncoderSpeex()");

// 	m_pSpeex = NULL;
// 	speex_bits_init(&m_speex_bits	
	SpeexOpen();
}

AudioEncoderSpeex::~AudioEncoderSpeex()
{
	WEBRTC_TRACE(kTraceInfo, kTraceVoice, 0,
		"AudioEncoderSpeex::~AudioEncoderSpeex()");
	SpeexClose();

	delete ((speex_context *)context_);
}

size_t AudioEncoderSpeex::MaxEncodedBytes() const {
	return full_frame_samples_ * BytesPerSample(); 
}

int AudioEncoderSpeex::SampleRateHz() const {
	return sample_rate_hz_;
}

size_t AudioEncoderSpeex::NumChannels() const {
	return num_channels_;
}

size_t AudioEncoderSpeex::Num10MsFramesInNextPacket() const {
	return num_10ms_frames_per_packet_;
}

size_t AudioEncoderSpeex::Max10MsFramesInAPacket() const {
	return num_10ms_frames_per_packet_;
}

int AudioEncoderSpeex::GetTargetBitrate() const {
	return 25000;
}
#include <stdio.h>

AudioEncoder::EncodedInfo AudioEncoderSpeex::EncodeInternal(
	uint32_t rtp_timestamp,
	rtc::ArrayView<const int16_t> audio,
	size_t max_encoded_bytes,
	uint8_t* encoded) {

	const int num_samples = SampleRateHz() / 100 * NumChannels();

// 
//  	{
//  		static FILE * f = NULL;
//  		if (f == NULL) f = fopen("/sdcard/test-cap.pcm", "wb+");
//  		if (f != NULL) {
//  			fwrite(audio, 1, 320, f);
//  		}
//  	}


	if (speech_buffer_.empty()) {
		first_timestamp_in_buffer_ = rtp_timestamp;
	}
	for (int i = 0; i < num_samples; ++i) {
		speech_buffer_.push_back(audio[i]);
	}

// 	WEBRTC_TRACE(kTraceStream, kTraceVoice, 0,
// 		"AudioEncoderSpeex::EncodeInternal(), sample_rate_hz_ %d, num_samples %d, full_frame_samples_ %d, speech_buffer_.size() %d",
// 		sample_rate_hz_, num_samples, full_frame_samples_, speech_buffer_.size());

	if (speech_buffer_.size() < full_frame_samples_ * SPEEX_FRAME_COUTN_PER_ENCODE) {
		return EncodedInfo();
	}
	//RTC_CHECK_EQ(speech_buffer_.size(), full_frame_samples_ * SPEEX_FRAME_COUTN_PER_ENCODE);

// 	WEBRTC_TRACE(kTraceStream, kTraceVoice, 0,
// 		"AudioEncoderSpeex::EncodeInternal(), RTC_CHECK_EQ(speech_buffer_.size(), full_frame_samples_ * SPEEX_FRAME_COUTN_PER_ENCODE);");

	//RTC_CHECK_GE(max_encoded_bytes, full_frame_samples_);
	EncodedInfo info;
	info.encoded_timestamp = first_timestamp_in_buffer_;
	info.payload_type = payload_type_;
	info.encoded_bytes =
		EncodeCall(&speech_buffer_[0], full_frame_samples_, encoded);

// 	WEBRTC_TRACE(kTraceStream, kTraceVoice, 0,
// 		"AudioEncoderSpeex::EncodeInternal() info.encoded_bytes %d", info.encoded_bytes);
	speech_buffer_.clear();
	return info;
}

void AudioEncoderSpeex::Reset() {
	speech_buffer_.clear();
	SpeexClose();
	SpeexOpen();
}


size_t AudioEncoderSpeex::EncodeCall(const int16_t* audio,
                                      size_t input_len,
                                      uint8_t* encoded) {
	return SpeexEncode(audio, input_len, encoded);
  //return WebRtcPcm16b_Encode(audio, input_len, encoded);
}

int AudioEncoderSpeex::BytesPerSample() const {
  return 2;
}

namespace {
AudioEncoderSpeex::Config CreateConfig(const CodecInst& codec_inst) {
  AudioEncoderSpeex::Config config;
  config.num_channels = codec_inst.channels;
  config.sample_rate_hz = codec_inst.plfreq;
  config.frame_size_ms = rtc::CheckedDivExact(
      codec_inst.pacsize, rtc::CheckedDivExact(config.sample_rate_hz, 1000));
  config.payload_type = codec_inst.pltype;
  return config;
}
}  // namespace

bool AudioEncoderSpeex::Config::IsOk() const {
  if (sample_rate_hz != 16000 || 
	  (frame_size_ms % 10 == 0) || (num_channels < 1))
    return false;
  return true;
}

AudioEncoderSpeex::AudioEncoderSpeex(const CodecInst& codec_inst)
    : AudioEncoderSpeex(CreateConfig(codec_inst)) {}



bool AudioEncoderSpeex::SpeexOpen()
{
	speex_bits_init(&((speex_context *)context_)->speex_bits);
	if (((speex_context *)context_)->pSpeex != NULL)
		SpeexClose();

	((speex_context *)context_)->pSpeex = speex_encoder_init(&speex_wb_mode);
	if (((speex_context *)context_)->pSpeex == NULL) return false;

	speex_bits_init(&((speex_context *)context_)->speex_bits);
	speex_bits_reset(&((speex_context *)context_)->speex_bits);


	spx_int32_t quality = 8;
	short complexity = 6;
	speex_encoder_ctl(((speex_context *)context_)->pSpeex, SPEEX_SET_QUALITY, &quality);
	speex_encoder_ctl(((speex_context *)context_)->pSpeex, SPEEX_SET_COMPLEXITY, &complexity);
// 	speex_encoder_ctl(m_pSpeex, SPEEX_GET_FRAME_SIZE, &m_frame_size);
// 	m_frame_size = Info.samplerate * 2 / 50;

	int bitrate = 22000;
	speex_encoder_ctl(((speex_context *)context_)->pSpeex, SPEEX_SET_BITRATE, &bitrate);

	return true;
}

void AudioEncoderSpeex::SpeexClose()
{
	if (((speex_context *)context_)->pSpeex)
	{
		speex_encoder_destroy(((speex_context *)context_)->pSpeex);
	}
	speex_bits_destroy(&((speex_context *)context_)->speex_bits);
	((speex_context *)context_)->pSpeex = NULL;
}

int AudioEncoderSpeex::SpeexEncode(const int16_t* audio,
	size_t input_len,
	uint8_t* encoded)
{
	if (((speex_context *)context_)->pSpeex == NULL) return 0;

	//outBuffer.AssureSize(1024, false);

	//char *pDest = (char *)outBuffer.Buffer();
	//int & iDest = outBuffer.BufferLen();
	int encodedBytes = 0;



	spx_int16_t* src = (spx_int16_t*)audio;
	for (int i = 0; i < SPEEX_FRAME_COUTN_PER_ENCODE; i++)
	{
		speex_encode_int(((speex_context *)context_)->pSpeex, src, &((speex_context *)context_)->speex_bits);
		src += full_frame_samples_;
	}

	speex_bits_insert_terminator(&((speex_context *)context_)->speex_bits);
	encodedBytes = speex_bits_write(&((speex_context *)context_)->speex_bits, (char*)encoded, 300);
	speex_bits_reset(&((speex_context *)context_)->speex_bits);

	return encodedBytes;
}


#include <limits>

#include "webrtc/base/checks.h"
#include "webrtc/common_types.h"
#include "webrtc/modules/audio_coding/codecs/g711/g711_interface.h"


namespace {

	template <typename T>
	typename T::Config CreateConfig(const CodecInst& codec_inst) {
		typename T::Config config;
		config.frame_size_ms = codec_inst.pacsize / 8;
		config.num_channels = codec_inst.channels;
		config.payload_type = codec_inst.pltype;
		return config;
	}

}  // namespace

bool AudioEncoderPcm::Config::IsOk() const {
	return (frame_size_ms % 10 == 0) && (num_channels >= 1);
}

AudioEncoderPcm::AudioEncoderPcm(const Config& config, int sample_rate_hz)
	: sample_rate_hz_(sample_rate_hz),
	num_channels_(config.num_channels),
	payload_type_(config.payload_type),
	num_10ms_frames_per_packet_(
	static_cast<size_t>(config.frame_size_ms / 10)),
	full_frame_samples_(
	config.num_channels * config.frame_size_ms * sample_rate_hz / 1000),
	first_timestamp_in_buffer_(0) {
	RTC_CHECK_GT(sample_rate_hz, 0) << "Sample rate must be larger than 0 Hz";
	RTC_CHECK_EQ(config.frame_size_ms % 10, 0)
		<< "Frame size must be an integer multiple of 10 ms.";
	speech_buffer_.reserve(full_frame_samples_);
}

AudioEncoderPcm::~AudioEncoderPcm() = default;

size_t AudioEncoderPcm::MaxEncodedBytes() const {
	return full_frame_samples_ * BytesPerSample();
}

int AudioEncoderPcm::SampleRateHz() const {
	return sample_rate_hz_;
}

size_t AudioEncoderPcm::NumChannels() const {
	return num_channels_;
}

size_t AudioEncoderPcm::Num10MsFramesInNextPacket() const {
	return num_10ms_frames_per_packet_;
}

size_t AudioEncoderPcm::Max10MsFramesInAPacket() const {
	return num_10ms_frames_per_packet_;
}

int AudioEncoderPcm::GetTargetBitrate() const {
	return static_cast<int>(
		8 * BytesPerSample() * SampleRateHz() * NumChannels());
}

AudioEncoder::EncodedInfo AudioEncoderPcm::EncodeInternal(
	uint32_t rtp_timestamp,
	rtc::ArrayView<const int16_t> audio,
	size_t max_encoded_bytes,
	uint8_t* encoded) {
	if (speech_buffer_.empty()) {
		first_timestamp_in_buffer_ = rtp_timestamp;
	}
	speech_buffer_.insert(speech_buffer_.end(), audio.begin(), audio.end());
	if (speech_buffer_.size() < full_frame_samples_) {
		return EncodedInfo();
	}
	RTC_CHECK_EQ(speech_buffer_.size(), full_frame_samples_);
	RTC_CHECK_GE(max_encoded_bytes, full_frame_samples_);
	EncodedInfo info;
	info.encoded_timestamp = first_timestamp_in_buffer_;
	info.payload_type = payload_type_;
	info.encoded_bytes =
		EncodeCall(&speech_buffer_[0], full_frame_samples_, encoded);
	speech_buffer_.clear();
	return info;
}

void AudioEncoderPcm::Reset() {
	speech_buffer_.clear();
}

}  // namespace webrtc
