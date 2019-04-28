/**
 * constants.h - holds various constants that are generally global, such as
 * megabyte/kilobyte sizes, etc.
 */
#ifndef _CONSTANTS_H
#define _CONSTANTS_H

/* defines for different byte sizes */
#define BYTE_SIZE     8
#define FOUR_KB       0x00001000
#define EIGHT_KB      0x00002000
#define FOUR_MB       0x00400000
#define EIGHT_MB      0x00800000
#define TWELVE_MB     0x00C00000
#define MB_128        0x08000000
#define MB_132        0x08400000

/* byte masks */
#define LO_BYTE_MASK  0x00FF
#define HI_BYTE_MASK  0xFF00

/* text colors for each terminal */
#define CURSOR1			  0xe
#define CURSOR2			  0xc
#define CURSOR3			  0xb

/* numbers of each terminal */
#define FIRST_TERM		0
#define SECOND_TERM		1
#define THIRD_TERM		2

/* various maximums */
#define MAXUINT32     0xFFFFFFFF

#endif
