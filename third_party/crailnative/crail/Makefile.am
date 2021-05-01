AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. -I$(top_srcdir)/third_party/crailnative/. $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS) $(SSL_CFLAGS) $(SSL_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libcrail.a

libcrail_a_SOURCES =  client/common/block_cache.cc client/common/block_cache.h \
						client/common/crail_constants.h \
						client/common/file_type.h \
						client/common/future.h \
						client/common/serializable.h \
						client/crail_directory.cc client/crail_directory.h \
						client/crail_file.cc client/crail_file.h \
						client/crail_inputstream.cc client/crail_inputstream.h \
						client/crail_node.cc client/crail_node.h \
						client/crail_outputstream.cc client/crail_outputstream.h \
						client/crail_store.cc client/crail_store.h \
						client/directory_record.cc client/directory_record.h \
						client/metadata/block_info.cc client/metadata/block_info.h \
						client/metadata/datanode_info.cc client/metadata/datanode_info.h \
						client/metadata/file_info.cc client/metadata/file_info.h \
						client/metadata/filename.cc client/metadata/filename.h \
						client/namenode/create_request.cc client/namenode/create_request.h \
						client/namenode/create_response.cc client/namenode/create_response.h \
						client/namenode/getblock_request.cc client/namenode/getblock_request.h \
						client/namenode/getblock_response.cc client/namenode/getblock_response.h \
						client/namenode/ioctl_request.cc client/namenode/ioctl_request.h \
						client/namenode/ioctl_response.cc client/namenode/ioctl_response.h \
						client/namenode/lookup_request.cc client/namenode/lookup_request.h \
						client/namenode/lookup_response.cc client/namenode/lookup_response.h \
						client/namenode/namenode_client.h \
						client/namenode/namenode_request.cc client/namenode/namenode_request.h \
						client/namenode/namenode_response.cc client/namenode/namenode_response.h \
						client/namenode/narpc/narpc_create_response.h \
						client/namenode/narpc/narpc_getblock_response.h \
						client/namenode/narpc/narpc_ioctl_response.h \
						client/namenode/narpc/narpc_lookup_response.h \
						client/namenode/narpc/narpc_namenode_client.cc client/namenode/narpc/narpc_namenode_client.h \
						client/namenode/narpc/narpc_remove_response.h \
						client/namenode/narpc/narpc_void_response.h \
						client/namenode/remove_request.cc client/namenode/remove_request.h \
						client/namenode/remove_response.cc client/namenode/remove_response.h \
						client/namenode/setfile_request.cc client/namenode/setfile_request.h \
						client/namenode/void_response.cc client/namenode/void_response.h \
						client/storage/narpc/narpc_read_request.cc client/storage/narpc/narpc_read_request.h \
						client/storage/narpc/narpc_read_response.cc client/storage/narpc/narpc_read_response.h \
						client/storage/narpc/narpc_storage_client.cc client/storage/narpc/narpc_storage_client.h \
						client/storage/narpc/narpc_storage_request.cc client/storage/narpc/narpc_storage_request.h \
						client/storage/narpc/narpc_storage_response.cc client/storage/narpc/narpc_storage_response.h \
						client/storage/narpc/narpc_write_request.cc client/storage/narpc/narpc_write_request.h \
						client/storage/narpc/narpc_write_response.cc client/storage/narpc/narpc_write_response.h \
						client/storage/storage_cache.cc client/storage/storage_cache.h \
						client/storage/storage_client.h \
						client/utils/crail_hash.cc client/utils/crail_hash.h \
						client/utils/crail_networking.cc client/utils/crail_networking.h \
						client/utils/micro_clock.cc client/utils/micro_clock.h
							

