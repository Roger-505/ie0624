/*
 * This is the core graphics library for all our displays, providing a common
 * set of graphics primitives (points, lines, circles, etc.).  It needs to be
 * paired with a hardware-specific library for each display device we carry
 * (to handle the lower-level functions).
 *
 * Adafruit invests time and resources providing this open source code, please
 * support Adafruit & open-source hardware by purchasing products from Adafruit!
 *
 * Copyright (c) 2013 Adafruit Industries.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Modified the AdaFruit library to be a C library, changed the font and
 * generally munged it in a variety of ways, creating a reasonably quick
 * and dirty way to put something "interesting" on the LCD display.
 * --Chuck McManis (2013, 2014)
 *
 */

#include "gfx.h"
#include "font.c"

#define pgm_read_double_word(addr) (*(const uint8_t *)(addr))
#define FONT_CHAR_WIDTH 8

struct gfx_state __gfx_state;

void
gfx_drawPixel(int x, int y, uint8_t color)
{
	if ((x < 0) || (x >= __gfx_state._width) ||
	    (y < 0) || (y >= __gfx_state._height)) {
		return; /* off screen so don't draw it */
	}
	(__gfx_state.drawpixel)(x, y, color);
}
#define true 1

void
gfx_init(void (*pixel_func)(int, int, uint8_t), int width, int height)
{
	__gfx_state._width    = width;
	__gfx_state._height   = height;
	__gfx_state.rotation  = 0;
	__gfx_state.cursor_y  = __gfx_state.cursor_x    = 0;
	__gfx_state.textsize  = 1;
	__gfx_state.textcolor = 0;
	__gfx_state.textbgcolor = 0xff;
	__gfx_state.wrap      = true;
	__gfx_state.drawpixel = pixel_func;
}


/* Bresenham's algorithm - thx wikpedia */
void gfx_drawLine(int16_t x0, int16_t y0,
			    int16_t x1, int16_t y1,
			    uint8_t color)
{
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			gfx_drawPixel(y0, x0, color);
		} else {
			gfx_drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void gfx_drawFastVLine(int16_t x, int16_t y,
		       int16_t h, uint8_t color)
{
	/* Update in subclasses if desired! */
	gfx_drawLine(x, y, x, y + h - 1, color);
}

void gfx_fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
		  uint8_t color)
{
	/* Update in subclasses if desired! */
	int16_t i;
	for (i = x; i < x + w; i++) {
		gfx_drawFastVLine(i, y, h, color);
	}
}

void gfx_fillScreen(uint8_t color)
{
	gfx_fillRect(0, 0, __gfx_state._width, __gfx_state._height, color);
}

void gfx_drawBitmap(int16_t x, int16_t y, tImage bmp)
{
	for (int16_t j = 0; j < bmp.height; j++) {
		for (int16_t i = 0; i < bmp.width; i++) {
				gfx_drawPixel(x + i, y + j, pgm_read_double_word(&bmp.data[j * bmp.width + i])); // ojo con estos indices
			}
		}
}

void gfx_fillCircle(int16_t x0, int16_t y0, int16_t r,
		    uint8_t color)
{
	gfx_drawFastVLine(x0, y0 - r, 2*r+1, color);
	gfx_fillCircleHelper(x0, y0, r, 3, 0, color);
}

/* Used to do circles and roundrects */
void gfx_fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
			  uint8_t cornername, int16_t delta, uint8_t color)
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (cornername & 0x1) {
			gfx_drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
			gfx_drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
		}
		if (cornername & 0x2) {
			gfx_drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
			gfx_drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
		}
	}
}

/* Fill a rounded rectangle */
void gfx_fillRoundRect(int16_t x, int16_t y, int16_t w,
		       int16_t h, int16_t r, uint8_t color) {
	/* smarter version */
	gfx_fillRect(x + r, y, w - 2 * r, h, color);

	/* draw four corners */
	gfx_fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	gfx_fillCircleHelper(x + r        , y + r, r, 2, h - 2 * r - 1, color);
}

void gfx_box(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, int16_t line_w)
{
    gfx_fillRoundRect(x,y,w,h,r,GFX_WHITE);
    gfx_fillRoundRect(x + line_w, y + line_w, w - 2 * line_w, h - 2 * line_w, r, GFX_BLACK);
}

/* Draw a character */
void gfx_drawChar(int16_t x, int16_t y, unsigned char c,
		  uint32_t color, uint8_t bg, uint8_t size)
{
	int8_t i, j, column;
	const char (*glyph)[8] = char_addr[c - 32];

	// descender = (*glyph & 0x80) != 0;

    for (i = 0; i < 8; i++) {
        column = (*glyph)[i];
               
		for (j = 0; j < 8; j++) {
			if (column & 0x01) {
				if (size == 1) /* default size */
					gfx_drawPixel(x+i, y+j, color);
				else {  /* big size */
					gfx_fillRect(x+(i*size), y+(j*size),
						     size, size, color);
				}
			} else if (bg != color) {
				if (size == 1) /* default size */
					gfx_drawPixel(x+i, y+j, bg);
				else {  /* big size */
					gfx_fillRect(x+i*size, y+j*size,
						     size, size, bg);
				}
			}
			column >>= 1;
		}
	}
}

void gfx_write(uint8_t c)
{
	if (c == '\n') {
		__gfx_state.cursor_y += __gfx_state.textsize * 12;
		__gfx_state.cursor_x  = 0;
	} else if (c == '\r') {
		/* skip em */
	} else {
		gfx_drawChar(__gfx_state.cursor_x, __gfx_state.cursor_y,
			     c, __gfx_state.textcolor, __gfx_state.textbgcolor,
			     __gfx_state.textsize);
		__gfx_state.cursor_x += __gfx_state.textsize * 8;
		if (__gfx_state.wrap &&
		    (__gfx_state.cursor_x > (__gfx_state._width -
					     __gfx_state.textsize*8))) {
			__gfx_state.cursor_y += __gfx_state.textsize * 12;
			__gfx_state.cursor_x = 0;
		}
	}
}

void gfx_puts(char *s)
{
	while (*s) {
		gfx_write(*s);
		s++;
	}
}

void gfx_setCursor(int16_t x, int16_t y)
{
	__gfx_state.cursor_x = x;
	__gfx_state.cursor_y = y;
}

void gfx_setTextSize(uint8_t s)
{
	__gfx_state.textsize = (s > 0) ? s : 1;
}

void gfx_setTextColor(uint8_t color, uint8_t bg)
{
	__gfx_state.textcolor   = color;
	__gfx_state.textbgcolor = bg;
}

void gfx_setTextWrap(uint8_t w)
{
	__gfx_state.wrap = w;
}

uint8_t gfx_getRotation(void)
{
	return __gfx_state.rotation;
}

void gfx_setRotation(uint8_t x)
{
	__gfx_state.rotation = (x & 3);
	switch (__gfx_state.rotation) {
	case 0:
	case 2:
		__gfx_state._width  = GFX_WIDTH;
		__gfx_state._height = GFX_HEIGHT;
		break;
	case 1:
	case 3:
		__gfx_state._width  = GFX_HEIGHT;
		__gfx_state._height = GFX_WIDTH;
		break;
	}
}

/* Return the size of the display (per current rotation) */
uint16_t gfx_width(void)
{
	return __gfx_state._width;
}

uint16_t gfx_height(void)
{
	return __gfx_state._height;
}

uint8_t gfx_size(void)
{
	return __gfx_state.textsize;
}

/* Place a string in the middle of the screen by calculating the
 * position of the first char of the string so the whole
 * string is centered */
void gfx_puts_centered(char* s, int16_t y)
{
    int16_t sPos = (GFX_WIDTH - gfx_size() * FONT_CHAR_WIDTH * (strlen(s) + 1))/2;  // account for '\0' by adding 1 for ascciz
    gfx_setCursor(sPos, y);
    gfx_puts(s);
}
