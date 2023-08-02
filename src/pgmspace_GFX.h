/*
  pgmspace_GFX.h - Includes file pgmspace.h if it exists, and also defines
  pgm_read_* macros and inline functions if they don't exist. Designed for
  compatibility with the Adafruit_GFX.cpp file, and this code was copied from
  that file, hence the Adafruit copyright is retained here.

  Created by Ted Toal, July 30, 2023 from Adafruit_GFX.cpp.


  Copyright (c) 2013 Adafruit Industries.  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.


  Details:

  The pgm _read_* macros and inline functions defined in Adafruit_GFX.cpp are
  needed elsewhere in this library, hence they have been extracted from that
  file into this one, where they can be #included into any file that needs them.

  In an ideal world this file would have been included with the
  Adafruit_GFX_Library and #included by Adafruit_GFX.cpp.
*/
#ifndef pgmspace_GFX_h
#define pgmspace_GFX_h

#include <Arduino.h>
#include <gfxfont.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#endif

// Many (but maybe not all) non-AVR board installs define macros for
// compatibility with existing PROGMEM-reading AVR code. Do our own checks and
// defines here for good measure...

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char*)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short*)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long*)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards, 32 bits
// elsewhere. Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
#define pgm_read_pointer(addr) ((void*)pgm_read_dword(addr))
#else
#define pgm_read_pointer(addr) ((void*)pgm_read_word(addr))
#endif

inline GFXglyph* pgm_read_glyph_ptr(const GFXfont* gfxFont, uint8_t c) {
#ifdef __AVR__
  return &(((GFXglyph*)pgm_read_pointer(&gfxFont->glyph))[c]);
#else
  // expression in __AVR__ section may generate "dereferencing type-punned
  // pointer will break strict-aliasing rules" warning In fact, on other
  // platforms (such as STM32) there is no need to do this pointer magic as
  // program memory may be read in a usual way So expression may be simplified
  return gfxFont->glyph + c;
#endif //__AVR__
}

inline uint8_t* pgm_read_bitmap_ptr(const GFXfont* gfxFont) {
#ifdef __AVR__
  return (uint8_t*)pgm_read_pointer(&gfxFont->bitmap);
#else
  // expression in __AVR__ section generates "dereferencing type-punned pointer
  // will break strict-aliasing rules" warning In fact, on other platforms (such
  // as STM32) there is no need to do this pointer magic as program memory may
  // be read in a usual way So expression may be simplified
  return gfxFont->bitmap;
#endif //__AVR__
}

#endif // pgmspace_GFX_h
