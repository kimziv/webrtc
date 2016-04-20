#ifndef _WEBRTC_AUDIO_SPEEX_DEF_H_
#define _WEBRTC_AUDIO_SPEEX_DEF_H_

#include<stdio.h>

extern "C" {
#include "webrtc/modules/audio_coding/codecs/g711/speex/libspeex/config.h"
#include "webrtc/modules/audio_coding/codecs/g711/speex/include/speex/speex.h"
#include "webrtc/modules/audio_coding/codecs/g711/speex/include/speex/speex_bits.h"
#include "webrtc/modules/audio_coding/codecs/g711/speex/include/speex/speex_preprocess.h"

#ifndef SPEEX_HANDLE
	typedef void*		SPEEX_HANDLE;
#endif
};


struct speex_context
{
	SPEEX_HANDLE				pSpeex;
	SpeexMode					speex_mode;
	SpeexBits					speex_bits;
	SpeexPreprocessState *		preprocess;
public:
	speex_context() {
		pSpeex = NULL;
		preprocess = NULL;
	 }
};


#endif
