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

#ifndef NARPC_STORAGE_RESPONSE_H
#define NARPC_STORAGE_RESPONSE_H

#include "crail/client/common/serializable.h"
#include "ioutils/byte_buffer.h"
#include "narpc/rpc_message.h"

using namespace crail;

class NarpcStorageResponse : public RpcMessage {
public:
  NarpcStorageResponse(int error, int type);
  virtual ~NarpcStorageResponse();

  virtual int Update(ByteBuffer &buffer);
  virtual int Write(ByteBuffer &buffer) { return 0; }

  int Size() const { return sizeof(int) * 2; }

  int error() const { return error_; }
  int type() const { return type_; }
  virtual string ToString() const { return "NarpcStorageResponse"; }

private:
  int error_;
  int type_;
};

#endif /* NARPC_STORAGE_RESPONSE_H */
