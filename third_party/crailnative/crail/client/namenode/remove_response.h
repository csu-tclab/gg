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

#ifndef REMOVE_RESPONSE_H
#define REMOVE_RESPONSE_H

#include <memory>

#include "crail/client/metadata/file_info.h"
#include "crail/client/namenode/namenode_response.h"

class RemoveResponse : public NamenodeResponse {
public:
  RemoveResponse();
  virtual ~RemoveResponse();

  virtual int Update(ByteBuffer &buffer);

  int Size() const { return NamenodeResponse::Size() + file_info_->Size() * 2; }

  shared_ptr<FileInfo> file() const { return file_info_; }
  shared_ptr<FileInfo> parent() const { return parent_info_; }

private:
  shared_ptr<FileInfo> file_info_;
  shared_ptr<FileInfo> parent_info_;
};

#endif /* REMOVE_RESPONSE_H */
