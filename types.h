/**
 * @file types.h
 * @author m.shebanow
 * @date 11/09/2019
 * @brief generic type definitions.
 */
#pragma once
#ifndef _TYPES_H_
#define _TYPES_H_

// basic types
typedef unsigned char           bool_t;
typedef char                    int8_t;
typedef unsigned char           uint8_t;
typedef short                   int16_t;
typedef unsigned short          uint16_t;
typedef long                    int32_t;
typedef unsigned long           uint32_t;
typedef long long               int64_t;
typedef unsigned long long      uint64_t;
typedef void *                  pointer_t;
typedef float                   fp32_t;
typedef double                  fp64_t;
typedef uint32_t;               unorm32_t;

// for booleans
#define FALSE ((bool_t) 0)
#define TRUE ((bool_t) ~0)

// allocation
typedef uint64_t                size_t;

// in case NULL not defined
#ifndef NULL
#define NULL ((void *) 0)
#endif

#endif /* _TYPES_H_ */