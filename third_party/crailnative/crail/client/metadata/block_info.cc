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

#include "crail/client/metadata/block_info.h"

#include <iostream>

BlockInfo::BlockInfo() {}

BlockInfo::~BlockInfo() {}

int BlockInfo::Write(ByteBuffer &buffer) {
  datanode_info_.Write(buffer);
  buffer.PutLong(lba_);
  buffer.PutLong(addr_);
  buffer.PutInt(length_);
  buffer.PutInt(lkey_);

  return 0;
}

int BlockInfo::Update(ByteBuffer &buffer) {
  datanode_info_.Update(buffer);
  lba_ = buffer.GetLong();
  addr_ = buffer.GetLong();
  length_ = buffer.GetInt();
  lkey_ = buffer.GetInt();

  return 0;
}

int BlockInfo::Dump() const {
  datanode_info_.Dump();
  cout << "lba " << lba_ << ", addr " << addr_ << ", length_ " << length_
       << ", lkey " << lkey_ << endl;
  return 0;
}
