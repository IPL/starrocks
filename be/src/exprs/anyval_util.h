// This file is made available under Elastic License 2.0.
// This file is based on code available under the Apache license here:
//   https://github.com/apache/incubator-doris/blob/master/be/src/exprs/anyval_util.h

// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#ifndef STARROCKS_BE_SRC_QUERY_EXPRS_ANYVAL_UTIL_H
#define STARROCKS_BE_SRC_QUERY_EXPRS_ANYVAL_UTIL_H

#include "common/status.h"
#include "exprs/expr.h"
#include "runtime/primitive_type.h"
#include "udf/udf.h"
#include "util/hash_util.hpp"
#include "util/types.h"

namespace starrocks {

class MemPool;

// Utilities for AnyVals
class AnyValUtil {
public:
    static FunctionContext::TypeDesc column_type_to_type_desc(const TypeDescriptor& type);
};

} // namespace starrocks
#endif
