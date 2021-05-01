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

#ifndef NARPC_STORAGE_REQUEST_H
#define NARPC_STORAGE_REQUEST_H

#include "ioutils/byte_buffer.h"
#include "narpc/rpc_message.h"

using namespace crail;

enum class NarpcStorageRequestType : short { Read = 1, Write = 2 };

class NarpcStorageRequest : public RpcMessage {
public:
  NarpcStorageRequest(int type);
  virtual ~NarpcStorageRequest();

  virtual int Write(ByteBuffer &buffer);
  virtual int Update(ByteBuffer &buffer) { return 0; };

  int Size() const { return sizeof(int); }
  string ToString() const { return "NarpcStorageRequest"; }

private:
  int type_;
};

#endif /* NARPC_STORAGE_REQUEST_H */
