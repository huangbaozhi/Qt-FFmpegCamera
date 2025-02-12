/*
 * Copyright (C) 2001-2011 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef SWSCALE_SWSCALE_H
#define SWSCALE_SWSCALE_H

/**
 * @file
 * @ingroup libsws
 * external API header
 */

#include <stdint.h>

#include "libavutil/avutil.h"
#include "libavutil/frame.h"
#include "libavutil/log.h"
#include "libavutil/pixfmt.h"
#include "version_major.h"
#ifndef HAVE_AV_CONFIG_H
/* When included as part of the ffmpeg build, only include the major version
 * to avoid unnecessary rebuilds. When included externally, keep including
 * the full version information. */
#include "version.h"
#endif

/**
 * @defgroup libsws libswscale
 * Color conversion and scaling library.
 *
 * @{
 *
 * Return the LIBSWSCALE_VERSION_INT constant.
 */
unsigned swscale_version(void);

/**
 * Return the libswscale build-time configuration.
 */
const char *swscale_configuration(void);

/**
 * Return the libswscale license.
 */
const char *swscale_license(void);

/* values for the flags, the stuff on the command line is different */
#define SWS_FAST_BILINEAR     1
#define SWS_BILINEAR          2
#define SWS_BICUBIC           4
#define SWS_X                 8
#define SWS_POINT          0x10
#define SWS_AREA           0x20
#define SWS_BICUBLIN       0x40
#define SWS_GAUSS          0x80
#define SWS_SINC          0x100
#define SWS_LANCZOS       0x200
#define SWS_SPLINE        0x400

#define SWS_SRC_V_CHR_DROP_MASK     0x30000
#define SWS_SRC_V_CHR_DROP_SHIFT    16

#define SWS_PARAM_DEFAULT           123456

#define SWS_PRINT_INFO              0x1000

//the following 3 flags are not completely implemented
//internal chrominance subsampling info
#define SWS_FULL_CHR_H_INT    0x2000
//input subsampling info
#define SWS_FULL_CHR_H_INP    0x4000
#define SWS_DIRECT_BGR        0x8000
#define SWS_ACCURATE_RND      0x40000
#define SWS_BITEXACT          0x80000
#define SWS_ERROR_DIFFUSION  0x800000

#define SWS_MAX_REDUCE_CUTOFF 0.002

#define SWS_CS_ITU709         1
#define SWS_CS_FCC            4
#define SWS_CS_ITU601         5
#define SWS_CS_ITU624         5
#define SWS_CS_SMPTE170M      5
#define SWS_CS_SMPTE240M      7
#define SWS_CS_DEFAULT        5
#define SWS_CS_BT2020         9

/**
 * Return a pointer to yuv<->rgb coefficients for the given colorspace
 * suitable for sws_setColorspaceDetails().
 *
 * @param colorspace One of the SWS_CS_* macros. If invalid,
 * SWS_CS_DEFAULT is used.
 */
const int *sws_getCoefficients(int colorspace);

// when used for filters they must have an odd number of elements
// coeffs cannot be shared between vectors
typedef struct SwsVector {
    double *coeff;              ///< pointer to the list of coefficients
    int length;                 ///< number of coefficients in the vector
} SwsVector;

// vectors can be shared
typedef struct SwsFilter {
    SwsVector *lumH;
    SwsVector *lumV;
    SwsVector *chrH;
    SwsVector *chrV;
} SwsFilter;

struct SwsContext;

/**
 * Return a positive value if pix_fmt is a supported input format, 0
 * otherwise.
 */
int sws_isSupportedInput(enum AVPixelFormat pix_fmt);

/**
 * Return a positive value if pix_fmt is a supported output format, 0
 * otherwise.
 */
int sws_isSupportedOutput(enum AVPixelFormat pix_fmt);

/**
 * @param[in]  pix_fmt the pixel format
 * @return a positive value if an endianness conversion for pix_fmt is
 * supported, 0 otherwise.
 */
int sws_isSupportedEndiannessConversion(enum AVPixelFormat pix_fmt);

/**
 * Allocate an empty SwsContext. This must be filled and passed to
 * sws_init_context(). For filling see AVOptions, options.c and
 * sws_setColorspaceDetails().
 */
struct SwsContext *sws_alloc_context(void);

/**
 * Initialize the swscaler context sws_context.
 *
 * @return zero or positive value on success, a negative value on
 * error
 */
av_warn_unused_result
int sws_init_context(struct SwsContext *sws_context, SwsFilter *srcFilter, SwsFilter *dstFilter);

/**
 * 释放swscaler上下文swsContext。
 * 如果swsContext为NULL，则不执行任何操作。
 */
void sws_freeContext(struct SwsContext *swsContext);

/**
 * 分配并返回一个SwsContext。您需要它来使用sws_scale()执行缩放/转换操作。
 *
 * @param srcW the width of the source image
 * @param srcH the height of the source image
 * @param srcFormat the source image format
 * @param dstW the width of the destination image
 * @param dstH the height of the destination image
 * @param dstFormat the destination image format
 * @param flags specify which algorithm and options to use for rescaling
 * @param param extra parameters to tune the used scaler
 *              For SWS_BICUBIC param[0] and [1] tune the shape of the basis
 *              function, param[0] tunes f(1) and param[1] f´(1)
 *              For SWS_GAUSS param[0] tunes the exponent and thus cutoff
 *              frequency
 *              For SWS_LANCZOS param[0] tunes the width of the window function
 * @return a pointer to an allocated context, or NULL in case of error
 * @note this function is to be removed after a saner alternative is
 *       written
 */
struct SwsContext *sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
                                  int dstW, int dstH, enum AVPixelFormat dstFormat,
                                  int flags, SwsFilter *srcFilter,
                                  SwsFilter *dstFilter, const double *param);

/**
 * 在srcSlice中缩放图像切片，并将生成的缩放切片放入dst中的图像中。
 * 切片是图像中连续行的序列。
 *
 * 切片必须按顺序提供，可以是自上而下，也可以是自下而上。
 * 如果切片是按非顺序提供的，则函数的行为是未定义的。
 *
 * @param c         先前使用sws_getContext（）创建的缩放上下文
 * @param srcSlice  包含指向源切片平面的指针的数组
 * @param srcStride 包含源图像每个平面步幅的数组
 * @param srcSliceY 要处理的切片在源图像中的位置，即切片第一行图像中的数字（从零开始计数）
 * @param srcSliceH 源切片的高度，即切片中的行数
 * @param dst       包含指向目标图像平面的指针的数组
 * @param dstStride 包含目标图像每个平面的步幅的数组
 * @return          输出切片的高度
 */
int sws_scale(struct SwsContext *c, const uint8_t *const srcSlice[],
              const int srcStride[], int srcSliceY, int srcSliceH,
              uint8_t *const dst[], const int dstStride[]);

/**
 * Scale source data from src and write the output to dst.
 *
 * This is merely a convenience wrapper around
 * - sws_frame_start()
 * - sws_send_slice(0, src->height)
 * - sws_receive_slice(0, dst->height)
 * - sws_frame_end()
 *
 * @param dst The destination frame. See documentation for sws_frame_start() for
 *            more details.
 * @param src The source frame.
 *
 * @return 0 on success, a negative AVERROR code on failure
 */
int sws_scale_frame(struct SwsContext *c, AVFrame *dst, const AVFrame *src);

/**
 * Initialize the scaling process for a given pair of source/destination frames.
 * Must be called before any calls to sws_send_slice() and sws_receive_slice().
 *
 * This function will retain references to src and dst, so they must both use
 * refcounted buffers (if allocated by the caller, in case of dst).
 *
 * @param dst The destination frame.
 *
 *            The data buffers may either be already allocated by the caller or
 *            left clear, in which case they will be allocated by the scaler.
 *            The latter may have performance advantages - e.g. in certain cases
 *            some output planes may be references to input planes, rather than
 *            copies.
 *
 *            Output data will be written into this frame in successful
 *            sws_receive_slice() calls.
 * @param src The source frame. The data buffers must be allocated, but the
 *            frame data does not have to be ready at this point. Data
 *            availability is then signalled by sws_send_slice().
 * @return 0 on success, a negative AVERROR code on failure
 *
 * @see sws_frame_end()
 */
int sws_frame_start(struct SwsContext *c, AVFrame *dst, const AVFrame *src);

/**
 * Finish the scaling process for a pair of source/destination frames previously
 * submitted with sws_frame_start(). Must be called after all sws_send_slice()
 * and sws_receive_slice() calls are done, before any new sws_frame_start()
 * calls.
 */
void sws_frame_end(struct SwsContext *c);

/**
 * Indicate that a horizontal slice of input data is available in the source
 * frame previously provided to sws_frame_start(). The slices may be provided in
 * any order, but may not overlap. For vertically subsampled pixel formats, the
 * slices must be aligned according to subsampling.
 *
 * @param slice_start first row of the slice
 * @param slice_height number of rows in the slice
 *
 * @return a non-negative number on success, a negative AVERROR code on failure.
 */
int sws_send_slice(struct SwsContext *c, unsigned int slice_start,
                   unsigned int slice_height);

/**
 * Request a horizontal slice of the output data to be written into the frame
 * previously provided to sws_frame_start().
 *
 * @param slice_start first row of the slice; must be a multiple of
 *                    sws_receive_slice_alignment()
 * @param slice_height number of rows in the slice; must be a multiple of
 *                     sws_receive_slice_alignment(), except for the last slice
 *                     (i.e. when slice_start+slice_height is equal to output
 *                     frame height)
 *
 * @return a non-negative number if the data was successfully written into the output
 *         AVERROR(EAGAIN) if more input data needs to be provided before the
 *                         output can be produced
 *         another negative AVERROR code on other kinds of scaling failure
 */
int sws_receive_slice(struct SwsContext *c, unsigned int slice_start,
                      unsigned int slice_height);

/**
 * @return alignment required for output slices requested with sws_receive_slice().
 *         Slice offsets and sizes passed to sws_receive_slice() must be
 *         multiples of the value returned from this function.
 */
unsigned int sws_receive_slice_alignment(const struct SwsContext *c);

/**
 * @param dstRange flag indicating the while-black range of the output (1=jpeg / 0=mpeg)
 * @param srcRange flag indicating the while-black range of the input (1=jpeg / 0=mpeg)
 * @param table the yuv2rgb coefficients describing the output yuv space, normally ff_yuv2rgb_coeffs[x]
 * @param inv_table the yuv2rgb coefficients describing the input yuv space, normally ff_yuv2rgb_coeffs[x]
 * @param brightness 16.16 fixed point brightness correction
 * @param contrast 16.16 fixed point contrast correction
 * @param saturation 16.16 fixed point saturation correction
#if LIBSWSCALE_VERSION_MAJOR > 6
 * @return negative error code on error, non negative otherwise
#else
 * @return -1 if not supported
#endif
 */
int sws_setColorspaceDetails(struct SwsContext *c, const int inv_table[4],
                             int srcRange, const int table[4], int dstRange,
                             int brightness, int contrast, int saturation);

/**
#if LIBSWSCALE_VERSION_MAJOR > 6
 * @return negative error code on error, non negative otherwise
#else
 * @return -1 if not supported
#endif
 */
int sws_getColorspaceDetails(struct SwsContext *c, int **inv_table,
                             int *srcRange, int **table, int *dstRange,
                             int *brightness, int *contrast, int *saturation);

/**
 * Allocate and return an uninitialized vector with length coefficients.
 */
SwsVector *sws_allocVec(int length);

/**
 * Return a normalized Gaussian curve used to filter stuff
 * quality = 3 is high quality, lower is lower quality.
 */
SwsVector *sws_getGaussianVec(double variance, double quality);

/**
 * Scale all the coefficients of a by the scalar value.
 */
void sws_scaleVec(SwsVector *a, double scalar);

/**
 * Scale all the coefficients of a so that their sum equals height.
 */
void sws_normalizeVec(SwsVector *a, double height);

void sws_freeVec(SwsVector *a);

SwsFilter *sws_getDefaultFilter(float lumaGBlur, float chromaGBlur,
                                float lumaSharpen, float chromaSharpen,
                                float chromaHShift, float chromaVShift,
                                int verbose);
void sws_freeFilter(SwsFilter *filter);

/**
 * 获取缓存的图像转换上下文。首先校验参数是否一致，如果校验不通过就释放资源；
 * 然后判断上下文是否存在，如果存在直接复用，如不存在进行分配、初始化操作
 *
 * 如果context为NULL，只需调用sws_getContext（）来获取新上下文。
 * 否则，检查参数是否已保存在上下文中。
 * 如果是这样，则返回当前上下文。
 * 否则，释放上下文并使用新参数获取新上下文。
 *
 * 请注意，未选中srcFilter和dstFilter，它们假定保持不变。
 */
struct SwsContext *sws_getCachedContext(struct SwsContext *context,
                                        int srcW, int srcH, enum AVPixelFormat srcFormat,
                                        int dstW, int dstH, enum AVPixelFormat dstFormat,
                                        int flags, SwsFilter *srcFilter,
                                        SwsFilter *dstFilter, const double *param);

/**
 * Convert an 8-bit paletted frame into a frame with a color depth of 32 bits.
 *
 * The output frame will have the same packed format as the palette.
 *
 * @param src        source frame buffer
 * @param dst        destination frame buffer
 * @param num_pixels number of pixels to convert
 * @param palette    array with [256] entries, which must match color arrangement (RGB or BGR) of src
 */
void sws_convertPalette8ToPacked32(const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette);

/**
 * Convert an 8-bit paletted frame into a frame with a color depth of 24 bits.
 *
 * With the palette format "ABCD", the destination frame ends up with the format "ABC".
 *
 * @param src        source frame buffer
 * @param dst        destination frame buffer
 * @param num_pixels number of pixels to convert
 * @param palette    array with [256] entries, which must match color arrangement (RGB or BGR) of src
 */
void sws_convertPalette8ToPacked24(const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette);

/**
 * Get the AVClass for swsContext. It can be used in combination with
 * AV_OPT_SEARCH_FAKE_OBJ for examining options.
 *
 * @see av_opt_find().
 */
const AVClass *sws_get_class(void);

/**
 * @}
 */

#endif /* SWSCALE_SWSCALE_H */
