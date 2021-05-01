AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS) $(SSL_CFLAGS) $(SSL_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libcrailclient.a

libcrailclient_a_SOURCES =  common/block_cache.cc
							common/block_cache.h
							common/crail_constants.h
							common/file_type.h
							common/future.h
							common/serializable.h
							crail_directory.cc
							crail_directory.h
							crail_file.cc
							crail_file.h
							crail_inputstream.cc
							crail_inputstream.h
							crail_node.cc
							crail_node.h
							crail_outputstream.cc
							crail_outputstream.h
							crail_store.cc
							crail_store.h
							directory_record.cc
							directory_record.h
							metadata/block_info.cc
							metadata/block_info.h
							metadata/datanode_info.cc
							metadata/datanode_info.h
							metadata/file_info.cc
							metadata/file_info.h
							metadata/filename.cc
							metadata/filename.h
							namenode/create_request.cc
							namenode/create_request.h
							namenode/create_response.cc
							namenode/create_response.h
							namenode/getblock_request.cc
							namenode/getblock_request.h
							namenode/getblock_response.cc
							namenode/getblock_response.h
							namenode/ioctl_request.cc
							namenode/ioctl_request.h
							namenode/ioctl_response.cc
							namenode/ioctl_response.h
							namenode/lookup_request.cc
							namenode/lookup_request.h
							namenode/lookup_response.cc
							namenode/lookup_response.h
							namenode/namenode_client.h
							namenode/namenode_request.cc
							namenode/namenode_request.h
							namenode/namenode_response.cc
							namenode/namenode_response.h
							namenode/narpc/narpc_create_response.h
							namenode/narpc/narpc_getblock_response.h
							namenode/narpc/narpc_ioctl_response.h
							namenode/narpc/narpc_lookup_response.h
							namenode/narpc/narpc_namenode_client.cc
							namenode/narpc/narpc_namenode_client.h
							namenode/narpc/narpc_remove_response.h
							namenode/narpc/narpc_void_response.h
							namenode/remove_request.cc
							namenode/remove_request.h
							namenode/remove_response.cc
							namenode/remove_response.h
							namenode/setfile_request.cc
							namenode/setfile_request.h
							namenode/void_response.cc
							namenode/void_response.h
							storage/narpc/narpc_read_request.cc
							storage/narpc/narpc_read_request.h
							storage/narpc/narpc_read_response.cc
							storage/narpc/narpc_read_response.h
							storage/narpc/narpc_storage_client.cc
							storage/narpc/narpc_storage_client.h
							storage/narpc/narpc_storage_request.cc
							storage/narpc/narpc_storage_request.h
							storage/narpc/narpc_storage_response.cc
							storage/narpc/narpc_storage_response.h
							storage/narpc/narpc_write_request.cc
							storage/narpc/narpc_write_request.h
							storage/narpc/narpc_write_response.cc
							storage/narpc/narpc_write_response.h
							storage/storage_cache.cc
							storage/storage_cache.h
							storage/storage_client.h
							utils/crail_hash.cc
							utils/crail_hash.h
							utils/crail_networking.cc
							utils/crail_networking.h
							utils/micro_clock.cc
							utils/micro_clock.h

