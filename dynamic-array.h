/**
 * @file dynamic-array.h
 * @author 오기준 (kijunking@pusan.ac.kr)
 * @brief dynamic array에 대한 선언적 내용이 들어간다.
 * @version 0.1
 * @date 2020-05-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef _DYNAMIC_ARRAY_H
#define _DYNAMIC_ARRAY_H

#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#define SIZE_T_FORMAT "%I64d"
#define KEY_FORMAT SIZE_T_FORMAT
typedef size_t key_t;
#endif

#ifdef linux
#define SIZE_T_FORMAT "%ld"
#define KEY_FORMAT "%d"
#include <sys/types.h> // for key_t
#endif

typedef unsigned long bitmap_t;

#define BITS (8)
#define NR_LINES (1000)
#define BITS_PER_BITMAP (sizeof(bitmap_t) * BITS)
#define BITMAP_SIZE ((NR_LINES / BITS_PER_BITMAP) + 1)

#define INDEX_EMPTY ((size_t)(~0UL))

/**
 * @brief debug용 출력 함수
 * 
 */
#define pr_info(msg, ...)                                                      \
        fprintf(stderr, "[{%lfs} %s(%s):%d] " msg,                             \
                ((double)clock() / CLOCKS_PER_SEC), __FILE__, __func__,        \
                __LINE__, ##__VA_ARGS__)

/**
 * @brief bitmap에 대한 bit 연산을 수행한다.
 * 
 */
#define dynamic_array_set_bit(bitmap, index)                                   \
        (bitmap[(bitmap_t)((index) / BITS_PER_BITMAP)] |=                      \
         (1 << ((index) % BITS_PER_BITMAP)))
#define dynamic_array_clear_bit(bitmap, index)                                 \
        (bitmap[(bitmap_t)((index) / BITS_PER_BITMAP)] &=                      \
         ~(1 << ((index) % BITS_PER_BITMAP)))
#define dynamic_array_test_bit(bitmap, index)                                  \
        ((bitmap[(bitmap_t)((index) / BITS_PER_BITMAP)] &                      \
          (1 << ((index) % BITS_PER_BITMAP))) != 0)

/**
 * @brief dynamic array의 line에 들어가는 항목에 대한 구조체.
 * 
 */
struct item {
        key_t key; /**< unique한 item을 찾는 값에 해당한다. */
};

/**
 * @brief 2^index의 item을 가지는 line 구조체이다.
 * 
 */
struct line {
        key_t min; /**< 현재 line에서의 최솟값에 해당한다. */
        key_t max; /**< 현재 line에서의 최댓값에 해당한다. */

        size_t size; /**< 현재 line의 크기를 이야기한다.(2^index) */
        struct item *items; /**< 2^index 만큼의 item이 할당된다. */
};

/**
 * @brief dynamic array에 대한 전체 메타 데이터를 관리하는 배열이다.
 * 
 */
struct dynamic_array {
        size_t size; /**< dynamic array의 현재 크기를 지칭한다. */
        bitmap_t bitmap[BITMAP_SIZE]; /**< 값이 있는 위치 정보를 가지는 비트맵 */
        struct line lines[NR_LINES]; /**< 라인 구조체 */
};

struct dynamic_array *dynamic_array_init();
struct item *dynamic_array_search(struct dynamic_array *array, key_t key);
int dynamic_array_insert(struct dynamic_array *array, const struct item item);
void dynamic_array_free(struct dynamic_array *array);

/**
 * @brief dynamic array의 비트맵을 초기화 한다.
 * 
 * @param bitmap 초기화하려고 하는 비트맵의 포인터를 지칭한다.
 */
static inline void dynamic_array_bitmap_init(bitmap_t *bitmap)
{
        int i;
        for (i = 0; i < BITMAP_SIZE; i++) {
                bitmap[i] &= 0x0;
        }
}

/**
 * @brief dynamic array의 bitmap_idx위치의 bitmap에서 최초로 0이 나오는 비트를 반환한다.
 * 
 * @param bitmap 찾고자 하는 비트맵
 * @param bitmap_idx 현재 탐색 중인 비트맵 인덱스
 * @return size_t 비트맵의 위치. 못 찾은 경우 INDEX_EMPTY가 반환된다.
 */
static inline size_t __dynamic_array_find_first_zero_bit(bitmap_t *bitmap,
                                                         size_t bitmap_idx)
{
        size_t i, pos;
        if (bitmap[bitmap_idx] == (size_t)(~0UL)) {
                return INDEX_EMPTY;
        }

        for (i = 0; i < BITS_PER_BITMAP; i++) {
                pos = bitmap_idx * BITS_PER_BITMAP + i;
                if (!dynamic_array_test_bit(bitmap, pos)) {
                        return pos;
                }
        }

        return INDEX_EMPTY;
}

/**
 * @brief dynamic array의 bitmap에서 최초로 0이 나오는 비트를 반환한다.
 * 
 * @param bitmap 찾고자 하는 비트맵
 * @return size_t 비트맵의 위치. 못 찾은 경우 INDEX_EMPTY가 반환된다.
 */
static inline size_t dynamic_array_find_first_zero_bit(bitmap_t *bitmap)
{
        size_t i, pos;
        for (i = 0; i < BITMAP_SIZE; i++) {
                pos = __dynamic_array_find_first_zero_bit(bitmap, i);
                if (pos != INDEX_EMPTY) {
                        return pos;
                }
        }

        return INDEX_EMPTY;
}

/**
 * @brief dynamic array의 bitmap_idx 위치의 bitmap에서 최초로 1이 나오는 비트를 반환한다.
 * 
 * @param bitmap 찾고자 하는 비트맵
 * @param bitmap_idx 현재 탐색 중인 비트맵 인덱스
 * @return size_t 비트맵의 위치. 못 찾은 경우 INDEX_EMPTY가 반환된다.
 */
static inline size_t __dynamic_array_find_first_bit(bitmap_t *bitmap,
                                                    size_t bitmap_idx)
{
        size_t i, pos;
        if (bitmap[bitmap_idx] == (size_t)(~0UL)) {
                return INDEX_EMPTY;
        }

        for (i = 0; i < BITS_PER_BITMAP; i++) {
                pos = bitmap_idx * BITS_PER_BITMAP + i;
                if (dynamic_array_test_bit(bitmap, pos)) {
                        return pos;
                }
        }

        return INDEX_EMPTY;
}

/**
 * @brief dynamic array의 bitmap에서 최초로 1이 나오는 비트를 반환한다.
 * 
 * @param bitmap 찾고자 하는 비트맵
 * @return size_t 비트맵의 위치. 못 찾은 경우 INDEX_EMPTY가 반환된다.
 */
static inline size_t dynamic_array_find_first_bit(bitmap_t *bitmap)
{
        size_t i, pos;
        for (i = 0; i < BITMAP_SIZE; i++) {
                pos = __dynamic_array_find_first_bit(bitmap, i);
                if (pos != INDEX_EMPTY) {
                        return pos;
                }
        }

        return INDEX_EMPTY;
}

#endif