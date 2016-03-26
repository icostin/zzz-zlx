projects := zlx zlxstest zlxdtest

zlx_prod := slib dlib

zlx_csrc := $(sort $(wildcard *.c) ucw8.c)
zlx_chdr := zlx.h $(wildcard zlx/*.h)
zlxstest_csrc := test.c
zlxdtest_csrc := test.c

# xxx_cflags (1: prj, 2: prod, 3: cfg, 4: bld, 5: src)
zlx_cflags = -DZLX_TARGET='"$($4_target)"' -DZLX_CONFIG='"$3"' -DZLX_COMPILER='"$($4_compiler)"'
zlx_slib_cflags := -DZLX_STATIC
zlx_dlib_cflags := -DZLX_DYNAMIC

zlxstest_cflags := -DZLX_STATIC

# xxx_cflags (1: prj, 2: prod, 3: cfg, 4: bld, 5: src, 6:flags)
zlxstest_ldflags = -static -lzlx$($3_sfx)$($4_sfx)

# xxx_cflags (1: prj, 2: prod, 3: cfg, 4: bld, 5: src, 6:flags)
zlxdtest_ldflags = -lzlx$($3_sfx)$($4_sfx)

zlxstest_idep := zlx_slib
zlxdtest_idep := zlx_dlib

include icobld.mk

ucw8.c: cw.rb UnicodeData.txt EastAsianWidth.txt
	ruby cw.rb UnicodeData.txt EastAsianWidth.txt 8 > $@

UnicodeData.txt EastAsianWidth.txt:
	wget -O$@ ftp://ftp.unicode.org/Public/UNIDATA/$@
