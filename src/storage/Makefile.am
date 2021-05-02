AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. -I$(top_srcdir)/third_party/crailnative/. $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS) $(SSL_CFLAGS) $(SSL_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggstorage.a

libggstorage_a_SOURCES = backend.hh backend.cc \
                         backend_local.hh \
                         backend_s3.hh backend_s3.cc \
                         backend_redis.hh backend_redis.cc \
                         backend_gs.hh backend_gs.cc \
                         backend_crail.hh backend_crail.cc
