/*
 * CppCrail: Native Crail
 *
 * Author: Patrick Stuedi  <pstuedi@gmail.com>
 *
 * Copyright (C) 2015-2018
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "crail/client/crail_directory.h"

#include <iostream>
#include <memory>

#include "crail/client/crail_inputstream.h"
#include "crail/client/directory_record.h"

CrailDirectory::CrailDirectory(FileInfo file_info,
                               shared_ptr<NamenodeClient> namenode_client,
                               shared_ptr<StorageCache> storage_cache,
                               shared_ptr<BlockCache> block_cache)
    : CrailNode(file_info, namenode_client, storage_cache, block_cache) {}

CrailDirectory::~CrailDirectory() {}

int CrailDirectory::Enumerate() {
  int records = file_info_.capacity() / 512;
  unique_ptr<CrailInputstream> input_stream = make_unique<CrailInputstream>(
      namenode_client_, storage_cache_, block_cache_, file_info_, 0);
  shared_ptr<ByteBuffer> buf = make_shared<ByteBuffer>(512);
  DirectoryRecord record;
  for (int i = 0; i < records; i++) {
    buf->Clear();
    input_stream->Read(buf).get();
    buf->Flip();
    record.Update(*buf);
    if (record.valid()) {
      cout << record.name().c_str() << endl;
    }
  }

  return 0;
}
