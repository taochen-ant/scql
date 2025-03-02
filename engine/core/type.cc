// Copyright 2023 Ant Group Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "engine/core/type.h"

namespace scql::engine {

// TODO(shunde.csd): try to simplify the following data type conversion via
// macro instructions.

pb::PrimitiveDataType FromArrowDataType(
    const std::shared_ptr<arrow::DataType>& dtype) {
  pb::PrimitiveDataType ty;
  switch (dtype->id()) {
    case arrow::Type::BOOL:
      ty = pb::PrimitiveDataType::BOOL;
      break;
    case arrow::Type::UINT8:
      ty = pb::PrimitiveDataType::UINT8;
      break;
    case arrow::Type::INT8:
      ty = pb::PrimitiveDataType::INT8;
      break;
    case arrow::Type::UINT16:
      ty = pb::PrimitiveDataType::UINT16;
      break;
    case arrow::Type::INT16:
      ty = pb::PrimitiveDataType::INT16;
      break;
    case arrow::Type::UINT32:
      ty = pb::PrimitiveDataType::UINT32;
      break;
    case arrow::Type::INT32:
      ty = pb::PrimitiveDataType::INT32;
      break;
    case arrow::Type::UINT64:
      ty = pb::PrimitiveDataType::UINT64;
      break;
    case arrow::Type::INT64:
      ty = pb::PrimitiveDataType::INT64;
      break;
    case arrow::Type::FLOAT:
      ty = pb::PrimitiveDataType::FLOAT;
      break;
    case arrow::Type::DOUBLE:
      ty = pb::PrimitiveDataType::DOUBLE;
      break;
    case arrow::Type::STRING:
      ty = pb::PrimitiveDataType::STRING;
      break;
    default:
      ty = pb::PrimitiveDataType::PrimitiveDataType_UNDEFINED;
  }
  return ty;
}

std::shared_ptr<arrow::DataType> ToArrowDataType(pb::PrimitiveDataType dtype) {
  std::shared_ptr<arrow::DataType> dt;
  switch (dtype) {
    case pb::PrimitiveDataType::UINT8:
      dt = arrow::uint8();
      break;
    case pb::PrimitiveDataType::INT8:
      dt = arrow::int8();
      break;
    case pb::PrimitiveDataType::UINT16:
      dt = arrow::uint16();
      break;
    case pb::PrimitiveDataType::INT16:
      dt = arrow::int16();
      break;
    case pb::PrimitiveDataType::UINT32:
      dt = arrow::uint32();
      break;
    case pb::PrimitiveDataType::INT32:
      dt = arrow::int32();
      break;
    case pb::PrimitiveDataType::UINT64:
      dt = arrow::uint64();
      break;
    case pb::PrimitiveDataType::INT64:
      dt = arrow::int64();
      break;
    case pb::PrimitiveDataType::BOOL:
      dt = arrow::boolean();
      break;
    case pb::PrimitiveDataType::FLOAT:
      dt = arrow::float32();
      break;
    case pb::PrimitiveDataType::DOUBLE:
      dt = arrow::float64();
      break;
    default:
      dt = nullptr;
  }
  return dt;
}

spu::PtType ArrowDataTypeToSpuPtType(
    const std::shared_ptr<arrow::DataType>& dtype) {
  spu::PtType pt;
  switch (dtype->id()) {
    case arrow::Type::BOOL:
      pt = spu::PT_BOOL;
      break;
    case arrow::Type::UINT8:
      pt = spu::PT_U8;
      break;
    case arrow::Type::INT8:
      pt = spu::PT_I8;
      break;
    case arrow::Type::UINT16:
      pt = spu::PT_U16;
      break;
    case arrow::Type::INT16:
      pt = spu::PT_I16;
      break;
    case arrow::Type::UINT32:
      pt = spu::PT_U32;
      break;
    case arrow::Type::INT32:
      pt = spu::PT_I32;
      break;
    case arrow::Type::UINT64:
      pt = spu::PT_U64;
      break;
    case arrow::Type::INT64:
      pt = spu::PT_I64;
      break;
    case arrow::Type::FLOAT:
      pt = spu::PT_F32;
      break;
    case arrow::Type::DOUBLE:
      pt = spu::PT_F64;
      break;
    default:
      pt = spu::PT_INVALID;
  }
  return pt;
}

std::shared_ptr<arrow::DataType> SpuPtTypeToArrowDataType(spu::PtType pt_type) {
  std::shared_ptr<arrow::DataType> dt;
  switch (pt_type) {
    case spu::PT_I8:
      dt = arrow::int8();
      break;
    case spu::PT_U8:
      dt = arrow::uint8();
      break;
    case spu::PT_I16:
      dt = arrow::int16();
      break;
    case spu::PT_U16:
      dt = arrow::uint16();
      break;
    case spu::PT_I32:
      dt = arrow::int32();
      break;
    case spu::PT_U32:
      dt = arrow::uint32();
      break;
    case spu::PT_I64:
      dt = arrow::int64();
      break;
    case spu::PT_U64:
      dt = arrow::uint64();
      break;
    case spu::PT_F32:
      dt = arrow::float32();
      break;
    case spu::PT_F64:
      dt = arrow::float64();
      break;
    case spu::PT_BOOL:
      dt = arrow::boolean();
      break;
    default:
      dt = nullptr;
  }
  return dt;
}

spu::PtType DataTypeToSpuPtType(pb::PrimitiveDataType dtype) {
  spu::PtType pt;
  switch (dtype) {
    case pb::PrimitiveDataType::UINT8:
      pt = spu::PT_U8;
      break;
    case pb::PrimitiveDataType::INT8:
      pt = spu::PT_I8;
      break;
    case pb::PrimitiveDataType::UINT16:
      pt = spu::PT_U16;
      break;
    case pb::PrimitiveDataType::INT16:
      pt = spu::PT_I16;
      break;
    case pb::PrimitiveDataType::UINT32:
      pt = spu::PT_U32;
      break;
    case pb::PrimitiveDataType::INT32:
      pt = spu::PT_I32;
      break;
    case pb::PrimitiveDataType::UINT64:
      pt = spu::PT_U64;
      break;
    case pb::PrimitiveDataType::INT64:
      pt = spu::PT_I64;
      break;
    case pb::PrimitiveDataType::BOOL:
      pt = spu::PT_BOOL;
      break;
    case pb::PrimitiveDataType::FLOAT:
      pt = spu::PT_F32;
      break;
    case pb::PrimitiveDataType::DOUBLE:
      pt = spu::PT_F64;
      break;
    default:
      pt = spu::PT_INVALID;
  }
  return pt;
}

}  // namespace scql::engine