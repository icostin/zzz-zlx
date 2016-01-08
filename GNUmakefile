projects := zlx zlxstest zlxdtest

zlx_prod := slib dlib

zlx_csrc := stdarray.c misc.c
zlx_chdr := zlx.h zlx/base.h zlx/array.h zlx/stdarray.h
zlxstest_csrc := test.c
zlxdtest_csrc := test.c

# xxx_cflags (1: prj, 2: prod, 3: cfg, 4: bld, 5: src, 6:flags='cflags')
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

