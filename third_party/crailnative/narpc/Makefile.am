AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. -I$(top_srcdir)/third_party/crailnative/. \
              $(PROTOBUF_CFLAGS) $(CXX14_FLAGS) $(SSL_CFLAGS) $(SSL_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libnarpc.a

libnarpc_a_SOURCES = narpc_staging.cc narpc_staging.h \
				 	 network_utils.cc network_utils.h \
					 rpc_client.cc rpc_client.h \
					 rpc_message.h
