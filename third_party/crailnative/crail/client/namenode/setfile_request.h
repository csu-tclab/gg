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

#ifndef SETFILE_REQUEST_H
#define SETFILE_REQUEST_H

#include <memory>
#include <string>

#include "crail/client/common/serializable.h"
#include "crail/client/metadata/file_info.h"
#include "crail/client/metadata/filename.h"
#include "crail/client/namenode/namenode_request.h"
#include "ioutils/byte_buffer.h"

class SetfileRequest : public NamenodeRequest {
public:
  SetfileRequest(FileInfo file_info, bool close);
  virtual ~SetfileRequest();

  virtual int Write(ByteBuffer &buffer);

  int Size() const {
    return NamenodeRequest::Size() + file_info_.Size() + sizeof(int);
  }

  const FileInfo file_info() const { return file_info_; }
  virtual string ToString() const {
    return "SetfileRequest:: " + file_info_.ToString() + ", " +
           to_string(close_);
  }

private:
  FileInfo file_info_;
  bool close_;
};

#endif /* SETFILE_REQUEST_H */
