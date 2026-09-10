# Credits

## Team

- **Ernesto De Felice** - Project Lead, Core GBA programming (using libtonc), Fixed-Point Math, Audio (SFX & Music)
- **Pierluca De Felice** - Programming, Graphics, Audio (SFX & Music)
- **Raffaele Colamarino** - Lore, Programming, Game Manual
- **Raffaele Aucelli** - Programming


All the team contributed to the Game Logic, Game Design and Testing.

## Third-party assets

<!-- List any external assets used (graphics, audio, fonts) with author, license and link -->

- **Visual Assets (Cockpit HUD, Lunar Surface, Gauges, UI Digits)**:
  - Created for *Lander Advance* — Licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/)
- **Audio Assets (SFX, Engine Tones)**:
  - Created for *Lander Advance* — Licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/)

# Third-Party Code, Libraries & Toolchain

## Tonc & Tonc Text Engine (TTE)
- **Purpose:** Game Boy Advance support library (headers, core utilities, fixed-point math LUTs, affine transformations, default 8x8 font & text engine)
- **Author:** J. Vijn (Cearn)
- **Original Project:** http://www.coranac.com/projects/#tonc
- **Source (reference):** https://github.com/gbadev-org/libtonc
- **Distribution:** devkitPro / gba-dev toolchain (`libtonc`)
- **License:** MIT License

  Copyright (c) 2001-2008 J. Vijn

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

## Maxmod
- **Purpose:** Sound and music playback engine
- **Author:** Mukunda Johnson (mukunda@maxmod.org)
- **Source:** https://github.com/devkitPro/maxmod
- **License:** Permissive License — Copyright (c) 2008, Mukunda Johnson

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

## libgba
- **Purpose:** Low-level GBA register definitions, interrupts, and BIOS call wrappers
- **Author:** Dave Murphy (WinterMute) / devkitPro
- **Source:** https://github.com/devkitPro/libgba
- **License:** Permissive / LGPL

## devkitARM & Build Tools
- **devkitARM (GCC for ARM):** GNU Compiler Collection targeting `arm-none-eabi` (GPL)
- **Grit (GBA Raster Image Transmogrifier):** Jasper Vijn (Cearn) — Image to GBA graphics converter (MIT)
- **mmutil:** Mukunda Johnson / devkitPro — Maxmod soundbank compiler (Permissive)
- **gbafix:** devkitPro — GBA ROM header patching and checksum tool (GPL)
- **bin2s:** devkitPro — Binary to GNU Assembly conversion utility (GPL)