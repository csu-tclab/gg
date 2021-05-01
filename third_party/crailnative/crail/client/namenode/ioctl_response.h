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

#ifndef IOCTL_RESPONSE_H
#define IOCTL_RESPONSE_H

#include "crail/client/namenode/namenode_response.h"

using namespace crail;

class IoctlResponse : public NamenodeResponse {
public:
  IoctlResponse();
  virtual ~IoctlResponse();

  virtual int Update(ByteBuffer &buffer) { return 0; }

  int Size() const {
    return NamenodeResponse::Size() + sizeof(op_) + sizeof(long long);
  }

  long long count() const { return count_; }

private:
  unsigned char op_;
  long long count_;
};

#endif /* IOCTL_RESPONSE_H */
