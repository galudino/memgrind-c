CC = cc 
CSTD = -std=c11 
PEDANTIC = -pedantic-errors 
WALL = -Wall 
WERROR = -Werror
DEBUG = -g
OPTIMIZED = -Os

INCLUDE_ULOG = libs/cgcs_ulog
INCLUDE_VECTOR = libs/cgcs_vector
INCLUDE_MALLOC = libs/cgcs_malloc

OBJECT_ULOG_DEBUG = build/cgcs_ulog_debug.o
OBJECT_VECTOR_DEBUG = build/cgcs_vector_debug.o
OBJECT_MALLOC_DEBUG = build/cgcs_malloc_debug.o
OBJECT_DEBUG = $(OBJECT_ULOG_DEBUG) $(OBJECT_VECTOR_DEBUG) $(OBJECT_MALLOC_DEBUG)

OBJECT_ULOG = build/cgcs_ulog.o
OBJECT_VECTOR = build/cgcs_vector.o
OBJECT_MALLOC = build/cgcs_malloc.o
OBJECT = $(OBJECT_ULOG) $(OBJECT_VECTOR) $(OBJECT_MALLOC)

all: debug release

debug:	memgrind_c_debug

memgrind_c_debug:	memgrind_c.c $(OBJECT_DEBUG)
		$(CC) $(CSTD) $(PEDANTIC) $(WALL) $(WERROR) $(DEBUG) -I $(INCLUDE_ULOG) -I $(INCLUDE_VECTOR) -I $(INCLUDE_MALLOC) -o memgrind_c_debug memgrind_c.c $(OBJECT_DEBUG)

build/cgcs_ulog_debug.o:	libs/cgcs_ulog/cgcs_ulog.c
		$(CC) -c libs/cgcs_ulog/cgcs_ulog.c $(CSTD) $(PEDANTIC) $(WALL) $(WERROR) -I $(INCLUDE_ULOG) -o $(OBJECT_ULOG_DEBUG)

build/cgcs_vector_debug.o:	libs/cgcs_vector/cgcs_vector.c
		$(CC) -c libs/cgcs_vector/cgcs_vector.c $(CSTD) $(PEDANTIC) $(WALL) $(WERROR) -I $(INCLUDE_VECTOR) -o $(OBJECT_VECTOR_DEBUG)

build/cgcs_malloc_debug.o:	libs/cgcs_malloc/cgcs_malloc.c
		$(CC) -c libs/cgcs_malloc/cgcs_malloc.c $(CSTD) $(PEDANTIC) $(WALL) $(WERROR) -I $(INCLUDE_MALLOC) -o $(OBJECT_MALLOC_DEBUG)

release:	memgrind_c

memgrind_c:	memgrind_c.c $(OBJECT)
		$(CC) $(CSTD) $(PEDANTIC) $(WALL) $(WERROR) $(OPTIMIZED) -I $(INCLUDE_ULOG) -I $(INCLUDE_VECTOR) -I $(INCLUDE_MALLOC) -o memgrind_c memgrind_c.c $(OBJECT)

build/cgcs_ulog.o:	libs/cgcs_ulog/cgcs_ulog.c
		$(CC) -c libs/cgcs_ulog/cgcs_ulog.c $(CSTD) $(PEDANTIC) $(WALL) $(WERROR) -I $(INCLUDE_ULOG) -o $(OBJECT_ULOG)

build/cgcs_vector.o:	libs/cgcs_vector/cgcs_vector.c
		$(CC) -c libs/cgcs_vector/cgcs_vector.c $(CSTD) $(PEDANTIC) $(WALL) $(WERROR) -I $(INCLUDE_VECTOR) -o $(OBJECT_VECTOR)

build/cgcs_malloc.o:	libs/cgcs_malloc/cgcs_malloc.c
		$(CC) -c libs/cgcs_malloc/cgcs_malloc.c $(CSTD) $(PEDANTIC) $(WALL) $(WERROR) -I $(INCLUDE_MALLOC) -o $(OBJECT_MALLOC)

clean:
	rm -rf build/*.o memgrind_c_debug memgrind_c

