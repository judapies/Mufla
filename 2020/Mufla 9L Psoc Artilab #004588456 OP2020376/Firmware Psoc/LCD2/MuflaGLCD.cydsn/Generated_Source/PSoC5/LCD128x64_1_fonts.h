#ifndef _FONTS_H
#define _FONTS_H

#include <device.h>

/* select desired fonts. (Simply comment out those not needed) */
#define EN_FIVE_DOT
#define EN_SIX_DOT
#define EN_SEVEN_DOT
#define EN_NINE_DOT
#define EN_TEN_DOT
#define EN_FIFTEEN_DOT
#define EN_EIGHTEEN_DOT
//#define EN_FONT_MT1

/* define number labels for the font selections */
typedef enum
{
#ifdef EN_FIVE_DOT
	FONT_FIVE_DOT,
#endif

#ifdef EN_SIX_DOT
	FONT_SIX_DOT,
#endif

#ifdef EN_SEVEN_DOT
	FONT_SEVEN_DOT,
#endif

#ifdef EN_NINE_DOT
	FONT_NINE_DOT,
#endif

#ifdef EN_TEN_DOT
	FONT_TEN_DOT,
#endif

#ifdef EN_FIFTEEN_DOT
	FONT_FIFTEEN_DOT,
#endif

#ifdef EN_EIGHTEEN_DOT
	FONT_EIGHTEEN_DOT,
#endif

#ifdef EN_FONT_MT1
	FONT_MT1,
#endif

	FONT_COUNT
} FONT_BASE;

struct FONT_DEF 
{
   uint8 store_width;            /* glyph storage width in bytes */
   uint8 glyph_height;  		 /* glyph height for storage */
   const uint8 *glyph_table;      /* font table start address in memory */
   uint8 fixed_width;            /* fixed width of glyphs. If zero */
                                         /* then use the width table. */
   const uint8 *width_table; 	 /* variable width table start adress */
   uint8 glyph_beg;			 	 /* start ascii offset in table */
   uint8 glyph_end;				 /* end ascii offset in table */
   uint8 glyph_def;				 /*  for undefined glyph  */
};

/* font definition tables for the fonts */
extern const struct FONT_DEF fonts[FONT_COUNT];

/* glyph bitmap and width tables for the fonts */ 
#ifdef EN_FIVE_DOT
  extern const uint8 five_dot_glyph_table[];
  extern const uint8 five_dot_width_table[];
#endif

#ifdef EN_SIX_DOT
  extern const uint8 six_dot_glyph_table[];
  extern const uint8 six_dot_width_table[];
#endif

#ifdef EN_SEVEN_DOT
#define DEG_CHAR ('~'+1)
  extern const uint8 seven_dot_glyph_table[];
  extern const uint8 seven_dot_width_table[];
#endif

#ifdef EN_NINE_DOT
  extern const uint8  nine_dot_glyph_table[];
#endif

#ifdef EN_TEN_DOT
  extern const uint8  ten_dot_glyph_table[];
#endif

#ifdef EN_FIFTEEN_DOT
  extern const uint8  fifteen_dot_glyph_table[];
  extern const uint8  fifteen_dot_width_table[];
#endif

#ifdef EN_EIGHTEEN_DOT
  extern const uint8  eighteen_dot_glyph_table[];
  extern const uint8  eighteen_dot_width_table[];
#endif

#ifdef EN_FONT_MT1
  extern const uint8  font_mt1_glyph_table[];
  extern const uint8  font_mt1_width_table[];
#endif

#endif
