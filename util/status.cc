// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "leveldb/status.h"

#include <cstdio>

#include "port/port.h"

namespace leveldb {

const char* Status::CopyState(const char* state) {
  uint32_t size;
  std::memcpy(&size, state, sizeof(size)); //复制长度到size
  char* result = new char[size + 5];  //结果总长度包括前面5个字节
  std::memcpy(result, state, size + 5); //复制完整state内容，长度为size + 5
  return result;
}

Status::Status(Code code, const Slice& msg, const Slice& msg2) {
  assert(code != kOk);
  const uint32_t len1 = static_cast<uint32_t>(msg.size());
  const uint32_t len2 = static_cast<uint32_t>(msg2.size());
  const uint32_t size = len1 + (len2 ? (2 + len2) : 0);//2表示':'和' '
  char* result = new char[size + 5];// state_总长度包括前面5个字节
  std::memcpy(result, &size, sizeof(size));// 信息长度存入前4个字节
  result[4] = static_cast<char>(code);// 状态存入第5个字节
  std::memcpy(result + 5, msg.data(), len1);// 从第6个字节开始存储信息内容
  if (len2) {// 如果msg2不为空，则信息内容加上': ' + msg2
    result[5 + len1] = ':';
    result[6 + len1] = ' ';
    std::memcpy(result + 7 + len1, msg2.data(), len2);
  }
  state_ = result;
}

std::string Status::ToString() const {
  if (state_ == nullptr) {
    return "OK";
  } else {
    char tmp[30];
    const char* type;
    switch (code()) {
      case kOk:
        type = "OK";
        break;
      case kNotFound:
        type = "NotFound: ";
        break;
      case kCorruption:
        type = "Corruption: ";
        break;
      case kNotSupported:
        type = "Not implemented: ";
        break;
      case kInvalidArgument:
        type = "Invalid argument: ";
        break;
      case kIOError:
        type = "IO error: ";
        break;
      default:
        /**
         * snprintf将格式化的n个字节数据写入字符串
         */
        std::snprintf(tmp, sizeof(tmp),
                      "Unknown code(%d): ", static_cast<int>(code()));
        type = tmp;
        break;
    }
    std::string result(type);
    uint32_t length;
    std::memcpy(&length, state_, sizeof(length));// 取出state_信息长度存入length
    result.append(state_ + 5, length);// 添加具体信息
    return result;
  }
}

}  // namespace leveldb
