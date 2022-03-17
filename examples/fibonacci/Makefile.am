EXTRA_DIST = create-thunk.sh

AM_CPPFLAGS = -I$(srcdir)/../../src/. -I$(builddir)/../../src/. $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

GG_LDADD = ../../src/execution/libggexecution.a \
           ../../src/storage/libggstorage.a \
           ../../src/thunk/libthunk.a \
           ../../src/sandbox/libggsandbox.a \
           ../../src/trace/libggtrace.a \
           ../../src/trace/libggsyscalltable.a \
           ../../src/protobufs/libggprotobufs.a \
           ../../src/net/libggnet.a \
           ../../src/tui/libggtui.a \
           ../../src/util/libggutil.a

BASE_LDADD = $(GG_LDADD) $(PROTOBUF_LIBS) $(HIREDIS_LIBS) $(MEMCACHED_LIBS) -lmemcached -lmemcachedutil $(CRAIL_CFLAGS) -lcrail

bin_PROGRAMS = fib add

fib_SOURCES = fib.cc
fib_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(SSL_LIBS) -ldl -lz
fib_LDFLAGS = -s

add_SOURCES = add.cc
add_LDFLAGS = -s

if BUILD_STATIC_BINS
  fib_LDFLAGS += -static -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
  add_LDFLAGS += -static
endif
