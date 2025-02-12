/*
 * AVCodec public API
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVCODEC_CODEC_H
#define AVCODEC_CODEC_H

#include <stdint.h>

#include "libavutil/avutil.h"
#include "libavutil/hwcontext.h"
#include "libavutil/log.h"
#include "libavutil/pixfmt.h"
#include "libavutil/rational.h"
#include "libavutil/samplefmt.h"

#include "libavcodec/codec_id.h"
#include "libavcodec/version_major.h"

/**
 * @addtogroup lavc_core
 * @{
 */

/**
 * Decoder can use draw_horiz_band callback.
 */
#define AV_CODEC_CAP_DRAW_HORIZ_BAND     (1 <<  0)
/**
 * Codec uses get_buffer() or get_encode_buffer() for allocating buffers and
 * supports custom allocators.
 * If not set, it might not use get_buffer() or get_encode_buffer() at all, or
 * use operations that assume the buffer was allocated by
 * avcodec_default_get_buffer2 or avcodec_default_get_encode_buffer.
 */
#define AV_CODEC_CAP_DR1                 (1 <<  1)
#if FF_API_FLAG_TRUNCATED
/**
 * @deprecated Use parsers to always send proper frames.
 */
#define AV_CODEC_CAP_TRUNCATED           (1 <<  3)
#endif
/**
 * Encoder or decoder requires flushing with NULL input at the end in order to
 * give the complete and correct output.
 *
 * NOTE: If this flag is not set, the codec is guaranteed to never be fed with
 *       with NULL data. The user can still send NULL data to the public encode
 *       or decode function, but libavcodec will not pass it along to the codec
 *       unless this flag is set.
 *
 * Decoders:
 * The decoder has a non-zero delay and needs to be fed with avpkt->data=NULL,
 * avpkt->size=0 at the end to get the delayed data until the decoder no longer
 * returns frames.
 *
 * Encoders:
 * The encoder needs to be fed with NULL data at the end of encoding until the
 * encoder no longer returns data.
 *
 * NOTE: For encoders implementing the AVCodec.encode2() function, setting this
 *       flag also means that the encoder must set the pts and duration for
 *       each output packet. If this flag is not set, the pts and duration will
 *       be determined by libavcodec from the input frame.
 */
#define AV_CODEC_CAP_DELAY               (1 <<  5)
/**
 * Codec can be fed a final frame with a smaller size.
 * This can be used to prevent truncation of the last audio samples.
 */
#define AV_CODEC_CAP_SMALL_LAST_FRAME    (1 <<  6)

/**
 * Codec can output multiple frames per AVPacket
 * Normally demuxers return one frame at a time, demuxers which do not do
 * are connected to a parser to split what they return into proper frames.
 * This flag is reserved to the very rare category of codecs which have a
 * bitstream that cannot be split into frames without timeconsuming
 * operations like full decoding. Demuxers carrying such bitstreams thus
 * may return multiple frames in a packet. This has many disadvantages like
 * prohibiting stream copy in many cases thus it should only be considered
 * as a last resort.
 */
#define AV_CODEC_CAP_SUBFRAMES           (1 <<  8)
/**
 * Codec is experimental and is thus avoided in favor of non experimental
 * encoders
 */
#define AV_CODEC_CAP_EXPERIMENTAL        (1 <<  9)
/**
 * Codec should fill in channel configuration and samplerate instead of container
 */
#define AV_CODEC_CAP_CHANNEL_CONF        (1 << 10)
/**
 * Codec supports frame-level multithreading.
 */
#define AV_CODEC_CAP_FRAME_THREADS       (1 << 12)
/**
 * Codec supports slice-based (or partition-based) multithreading.
 */
#define AV_CODEC_CAP_SLICE_THREADS       (1 << 13)
/**
 * Codec supports changed parameters at any point.
 */
#define AV_CODEC_CAP_PARAM_CHANGE        (1 << 14)
/**
 * Codec supports multithreading through a method other than slice- or
 * frame-level multithreading. Typically this marks wrappers around
 * multithreading-capable external libraries.
 */
#define AV_CODEC_CAP_OTHER_THREADS       (1 << 15)
#if FF_API_AUTO_THREADS
#define AV_CODEC_CAP_AUTO_THREADS        AV_CODEC_CAP_OTHER_THREADS
#endif
/**
 * Audio encoder supports receiving a different number of samples in each call.
 */
#define AV_CODEC_CAP_VARIABLE_FRAME_SIZE (1 << 16)
/**
 * Decoder is not a preferred choice for probing.
 * This indicates that the decoder is not a good choice for probing.
 * It could for example be an expensive to spin up hardware decoder,
 * or it could simply not provide a lot of useful information about
 * the stream.
 * A decoder marked with this flag should only be used as last resort
 * choice for probing.
 */
#define AV_CODEC_CAP_AVOID_PROBING       (1 << 17)

#if FF_API_UNUSED_CODEC_CAPS
/**
 * Deprecated and unused. Use AVCodecDescriptor.props instead
 */
#define AV_CODEC_CAP_INTRA_ONLY       0x40000000
/**
 * Deprecated and unused. Use AVCodecDescriptor.props instead
 */
#define AV_CODEC_CAP_LOSSLESS         0x80000000
#endif

/**
 * Codec is backed by a hardware implementation. Typically used to
 * identify a non-hwaccel hardware decoder. For information about hwaccels, use
 * avcodec_get_hw_config() instead.
 */
#define AV_CODEC_CAP_HARDWARE            (1 << 18)

/**
 * Codec is potentially backed by a hardware implementation, but not
 * necessarily. This is used instead of AV_CODEC_CAP_HARDWARE, if the
 * implementation provides some sort of internal fallback.
 */
#define AV_CODEC_CAP_HYBRID              (1 << 19)

/**
 * This codec takes the reordered_opaque field from input AVFrames
 * and returns it in the corresponding field in AVCodecContext after
 * encoding.
 */
#define AV_CODEC_CAP_ENCODER_REORDERED_OPAQUE (1 << 20)

/**
 * This encoder can be flushed using avcodec_flush_buffers(). If this flag is
 * not set, the encoder must be closed and reopened to ensure that no frames
 * remain pending.
 */
#define AV_CODEC_CAP_ENCODER_FLUSH   (1 << 21)

/**
 * AVProfile.
 */
typedef struct AVProfile {
    int profile;
    const char *name; ///< short name for the profile
} AVProfile;

/**
 * AVCodec.
 */
typedef struct AVCodec {
    /**
     * 编解码器实现的名称。
     * 该名称在编码器和解码器之间是全局唯一的(但编码器和解码器可以共享相同的名称)。
     * 这是从用户角度查找编解码器的主要方法。
     */
    const char *name;
    /**
     * 编解码器的描述性名称，意味着比名称更易于阅读。
     * 您应该使用NULL_IF_CONFIG_SMALL（）宏来定义它。
     */
    const char *long_name;
    enum AVMediaType type;
    enum AVCodecID id;
    /**
     * 编解码器功能。
     * see AV_CODEC_CAP_*
     */
    int capabilities;
    uint8_t max_lowres;                     ///< maximum value for lowres supported by the decoder
    const AVRational *supported_framerates; ///< array of supported framerates, or NULL if any, array is terminated by {0,0}
    const enum AVPixelFormat *pix_fmts;     ///< array of supported pixel formats, or NULL if unknown, array is terminated by -1
    const int *supported_samplerates;       ///< array of supported audio samplerates, or NULL if unknown, array is terminated by 0
    const enum AVSampleFormat *sample_fmts; ///< array of supported sample formats, or NULL if unknown, array is terminated by -1
#if FF_API_OLD_CHANNEL_LAYOUT
    /**
     * @deprecated use ch_layouts instead
     */
    attribute_deprecated
    const uint64_t *channel_layouts;         ///< array of support channel layouts, or NULL if unknown. array is terminated by 0
#endif
    const AVClass *priv_class;              ///< AVClass for the private context
    const AVProfile *profiles;              ///< array of recognized profiles, or NULL if unknown, array is terminated by {FF_PROFILE_UNKNOWN}

    /**
     * Group name of the codec implementation.
     * This is a short symbolic name of the wrapper backing this codec. A
     * wrapper uses some kind of external implementation for the codec, such
     * as an external library, or a codec implementation provided by the OS or
     * the hardware.
     * If this field is NULL, this is a builtin, libavcodec native codec.
     * If non-NULL, this will be the suffix in AVCodec.name in most cases
     * (usually AVCodec.name will be of the form "<codec_name>_<wrapper_name>").
     */
    const char *wrapper_name;

    /**
     * Array of supported channel layouts, terminated with a zeroed layout.
     */
    const AVChannelLayout *ch_layouts;
} AVCodec;

/**
 * Iterate over all registered codecs.
 *
 * @param opaque a pointer where libavcodec will store the iteration state. Must
 *               point to NULL to start the iteration.
 *
 * @return the next registered codec or NULL when the iteration is
 *         finished
 */
const AVCodec *av_codec_iterate(void **opaque);

/**
 * 查找具有匹配编解码器ID的注册解码器。
 *
 * @param id 请求解码器的AVCodecID
 * @return A 解码器（如果找到），否则为NULL。
 */
const AVCodec *avcodec_find_decoder(enum AVCodecID id);

/**
 * Find a registered decoder with the specified name.
 *
 * @param name name of the requested decoder
 * @return A decoder if one was found, NULL otherwise.
 */
const AVCodec *avcodec_find_decoder_by_name(const char *name);

/**
 * Find a registered encoder with a matching codec ID.
 *
 * @param id AVCodecID of the requested encoder
 * @return An encoder if one was found, NULL otherwise.
 */
const AVCodec *avcodec_find_encoder(enum AVCodecID id);

/**
 * Find a registered encoder with the specified name.
 *
 * @param name name of the requested encoder
 * @return An encoder if one was found, NULL otherwise.
 */
const AVCodec *avcodec_find_encoder_by_name(const char *name);
/**
 * @return a non-zero number if codec is an encoder, zero otherwise
 */
int av_codec_is_encoder(const AVCodec *codec);

/**
 * @return a non-zero number if codec is a decoder, zero otherwise
 */
int av_codec_is_decoder(const AVCodec *codec);

/**
 * Return a name for the specified profile, if available.
 *
 * @param codec the codec that is searched for the given profile
 * @param profile the profile value for which a name is requested
 * @return A name for the profile if found, NULL otherwise.
 */
const char *av_get_profile_name(const AVCodec *codec, int profile);

enum {
    /**
     * 编解码器通过hw_device_ctx接口支持此格式。
     *
     * 选择此格式时，AVCodecContext。在调用avcodec_open2（）之前，应将hw_device_ctx设置为指定类型的设备。
     */
    AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX = 0x01,
    /**
     * 编解码器通过hw_frames_ctx接口支持这种格式。
     *
     * 当为解码器选择这种格式时，AVCodecContext。Hw_frames_ctx应该在get_format()回调函数中设置一个合适的帧上下文。
     * 帧上下文必须已经在指定类型的设备上创建。
     *
     * 为编码器选择此格式时，AVCodecContext。在调用avcodec_open2（）之前，应将hw_frames_ctx设置为将用于输入帧的上下文。
     */
    AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX = 0x02,
    /**
     * 编解码器通过一些内部方法支持这种格式。
     *
     * 这种格式的选择不需要任何额外的配置—不需要设备或帧上下文。
     */
    AV_CODEC_HW_CONFIG_METHOD_INTERNAL      = 0x04,
    /**
     * 编解码器通过一些特殊方法支持这种格式。
     *
     * 需要额外的设置和/或函数调用。
     * 有关详细信息，请参阅特定于编解码器的文档。(不推荐使用要求这种配置的方法，应该优先使用其他方法。)
     */
    AV_CODEC_HW_CONFIG_METHOD_AD_HOC        = 0x08,
};

typedef struct AVCodecHWConfig {
    /**
     * 对于解码器而言，一种硬件像素格式，如果有合适的硬件可用，解码器可以解码到该格式。
     *
     * 对于编码器，编码器可以接受的像素格式。
     * 如果设置为AV_PIX_FMT_NONE，则这适用于编解码器支持的所有像素格式。
     */
    enum AVPixelFormat pix_fmt;
    /**
     * AV_CODEC_HW_CONFIG_METHOD_*标志的位集，描述可用于此配置的可能设置方法。
     */
    int methods;
    /**
     * 与配置相关联的设备类型。
     *
     * 必须为AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX和AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX设置，否则不使用。
     */
    enum AVHWDeviceType device_type;
} AVCodecHWConfig;

/**
 * 检索编解码器支持的硬件配置。
 *
 * 索引值从零到某个最大值返回索引的配置描述符；
 * 所有其他值都返回NULL。如果编解码器不支持任何硬件配置，则它将始终返回NULL。
 */
const AVCodecHWConfig *avcodec_get_hw_config(const AVCodec *codec, int index);

/**
 * @}
 */

#endif /* AVCODEC_CODEC_H */
