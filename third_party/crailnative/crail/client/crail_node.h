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

#ifndef CRAIL_NODE_H
#define CRAIL_NODE_H

#include "crail/client/common/block_cache.h"
#include "crail/client/common/file_type.h"
#include "crail/client/crail_store.h"
#include "crail/client/metadata/file_info.h"
#include "crail/client/namenode/namenode_client.h"
#include "crail/client/storage/storage_cache.h"

using namespace std;
using namespace crail;

class CrailNode {
public:
  CrailNode() = default;
  CrailNode(FileInfo file_info, shared_ptr<NamenodeClient> namenode_client,
            shared_ptr<StorageCache> storage_cache,
            shared_ptr<BlockCache> block_cache);
  CrailNode(const CrailNode &other) : file_info_(other.file_info_) {
    this->namenode_client_ = other.namenode_client_;
    this->storage_cache_ = other.storage_cache_;
    this->block_cache_ = other.block_cache_;
  }
  virtual ~CrailNode() = default;

  CrailNode &operator=(CrailNode other) {
    if (&other == this) {
      return *this;
    }

    this->file_info_ = other.file_info_;
    this->namenode_client_ = other.namenode_client_;
    this->storage_cache_ = other.storage_cache_;
    this->block_cache_ = other.block_cache_;

    return *this;
  }

  bool valid() const { return file_info_.fd() >= 0; }
  int type() const { return file_info_.type(); }
  unsigned long long fd() const { return file_info_.fd(); }
  unsigned long long capacity() const { return file_info_.capacity(); }

protected:
  FileInfo file_info_;
  shared_ptr<NamenodeClient> namenode_client_;
  shared_ptr<StorageCache> storage_cache_;
  shared_ptr<BlockCache> block_cache_;
};

#endif /* CRAIL_NODE_H */
