/*
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

/**
 * @file
 * Public dictionary API.
 * @deprecated
 *  AVDictionary is provided for compatibility with libav. It is both in
 *  implementation as well as API inefficient. It does not scale and is
 *  extremely slow with large dictionaries.
 *  It is recommended that new code uses our tree container from tree.c/h
 *  where applicable, which uses AVL trees to achieve O(log n) performance.
 */

#ifndef AVUTIL_DICT_H
#define AVUTIL_DICT_H

#include <stdint.h>

/**
 * @addtogroup lavu_dict AVDictionary
 * @ingroup lavu_data
 *
 * @brief Simple key:value store
 *
 * @{
 * Dictionaries are used for storing key:value pairs. To create
 * an AVDictionary, simply pass an address of a NULL pointer to
 * av_dict_set(). NULL can be used as an empty dictionary wherever
 * a pointer to an AVDictionary is required.
 * Use av_dict_get() to retrieve an entry or iterate over all
 * entries and finally av_dict_free() to free the dictionary
 * and all its contents.
 *
 @code
   AVDictionary *d = NULL;           // "create" an empty dictionary
   AVDictionaryEntry *t = NULL;

   av_dict_set(&d, "foo", "bar", 0); // add an entry

   char *k = av_strdup("key");       // if your strings are already allocated,
   char *v = av_strdup("value");     // you can avoid copying them like this
   av_dict_set(&d, k, v, AV_DICT_DONT_STRDUP_KEY | AV_DICT_DONT_STRDUP_VAL);

   while (t = av_dict_get(d, "", t, AV_DICT_IGNORE_SUFFIX)) {
       <....>                             // iterate over all entries in d
   }
   av_dict_free(&d);
 @endcode
 */

#define AV_DICT_MATCH_CASE      1   /**< 只获取具有精确大小写键匹配的条目。只与av_dict_get()相关。 */
#define AV_DICT_IGNORE_SUFFIX   2   /**< 返回字典中第一部分与搜索键对应的第一个条目，
                                         忽略找到的键字符串的后缀。只与av_dict_get()相关。 */
#define AV_DICT_DONT_STRDUP_KEY 4   /**< Take ownership of a key that's been 获取用av_malloc()或其他内存分配函数分配的键的所有权。
                                         allocated with av_malloc() or another memory allocation function. */
#define AV_DICT_DONT_STRDUP_VAL 8   /**< Take ownership of a value that's been 获取用av_malloc()或其他内存分配函数分配的值的所有权。
                                         allocated with av_malloc() or another memory allocation function. */
#define AV_DICT_DONT_OVERWRITE 16   ///< 不要覆盖现有的条目。
#define AV_DICT_APPEND         32   /**< 如果条目已经存在，则向其追加。注意，没有添加分隔符，字符串只是连接在一起。 */
#define AV_DICT_MULTIKEY       64   /**< 允许在字典中存储几个相等的键 */

typedef struct AVDictionaryEntry {
    char *key;
    char *value;
} AVDictionaryEntry;

typedef struct AVDictionary AVDictionary;

/**
 * 获取具有匹配键的字典条目。
 *
 * The returned entry key or value must not be changed, or it will
 * cause undefined behavior.
 *
 * To iterate through all the dictionary entries, you can set the matching key
 * to the null string "" and set the AV_DICT_IGNORE_SUFFIX flag.
 *
 * @param prev Set to the previous matching element to find the next.
 *             If set to NULL the first matching element is returned.
 * @param key matching key
 * @param flags a collection of AV_DICT_* flags controlling how the entry is retrieved
 * @return found entry or NULL in case no matching entry was found in the dictionary
 */
AVDictionaryEntry *av_dict_get(const AVDictionary *m, const char *key,
                               const AVDictionaryEntry *prev, int flags);

/**
 * Get number of entries in dictionary.
 *
 * @param m dictionary
 * @return  number of entries in dictionary
 */
int av_dict_count(const AVDictionary *m);

/**
 * 在*pm中设置给定的条目，覆盖现有的条目。
 *
 * 注意:如果AV_DICT_DONT_STRDUP_KEY或AV_DICT_DONT_STRDUP_VAL被设置，这些参数将在错误时被释放。
 *
 * 警告：向字典中添加新条目会使以前使用av_dict_get返回的所有现有条目无效。
 *
 * @param pm    指向指向字典结构的指针的指针。如果*pm为NULL，则分配字典结构并将其放入*pm。
 * @param key   要添加到*pm的输入键(根据标志的不同，可以是av_strduped，也可以作为新键添加)
 * @param value 要添加到*pm的条目值（将被av_strduped或作为新键添加，具体取决于标志）.
 *        传递NULL值将导致删除现有条目。
 * @return >成功时=0，否则错误代码<0
 */
int av_dict_set(AVDictionary **pm, const char *key, const char *value, int flags);

/**
 * Convenience wrapper for av_dict_set that converts the value to a string
 * and stores it.
 *
 * Note: If AV_DICT_DONT_STRDUP_KEY is set, key will be freed on error.
 */
int av_dict_set_int(AVDictionary **pm, const char *key, int64_t value, int flags);

/**
 * Parse the key/value pairs list and add the parsed entries to a dictionary.
 *
 * In case of failure, all the successfully set entries are stored in
 * *pm. You may need to manually free the created dictionary.
 *
 * @param key_val_sep  a 0-terminated list of characters used to separate
 *                     key from value
 * @param pairs_sep    a 0-terminated list of characters used to separate
 *                     two pairs from each other
 * @param flags        flags to use when adding to dictionary.
 *                     AV_DICT_DONT_STRDUP_KEY and AV_DICT_DONT_STRDUP_VAL
 *                     are ignored since the key/value tokens will always
 *                     be duplicated.
 * @return             0 on success, negative AVERROR code on failure
 */
int av_dict_parse_string(AVDictionary **pm, const char *str,
                         const char *key_val_sep, const char *pairs_sep,
                         int flags);

/**
 * Copy entries from one AVDictionary struct into another.
 * @param dst pointer to a pointer to a AVDictionary struct. If *dst is NULL,
 *            this function will allocate a struct for you and put it in *dst
 * @param src pointer to source AVDictionary struct
 * @param flags flags to use when setting entries in *dst
 * @note metadata is read using the AV_DICT_IGNORE_SUFFIX flag
 * @return 0 on success, negative AVERROR code on failure. If dst was allocated
 *           by this function, callers should free the associated memory.
 */
int av_dict_copy(AVDictionary **dst, const AVDictionary *src, int flags);

/**
 * 释放分配给AVDictionary结构以及所有键和值的所有内存。
 */
void av_dict_free(AVDictionary **m);

/**
 * Get dictionary entries as a string.
 *
 * Create a string containing dictionary's entries.
 * Such string may be passed back to av_dict_parse_string().
 * @note String is escaped with backslashes ('\').
 *
 * @param[in]  m             dictionary
 * @param[out] buffer        Pointer to buffer that will be allocated with string containg entries.
 *                           Buffer must be freed by the caller when is no longer needed.
 * @param[in]  key_val_sep   character used to separate key from value
 * @param[in]  pairs_sep     character used to separate two pairs from each other
 * @return                   >= 0 on success, negative on error
 * @warning Separators cannot be neither '\\' nor '\0'. They also cannot be the same.
 */
int av_dict_get_string(const AVDictionary *m, char **buffer,
                       const char key_val_sep, const char pairs_sep);

/**
 * @}
 */

#endif /* AVUTIL_DICT_H */
