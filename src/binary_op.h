/* Copyright 2021 NVIDIA Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once

#include "numpy.h"
#include "deserializer.h"

namespace legate {
namespace numpy {

class BinaryOpTask : public NumPyTask<BinaryOpTask> {
 public:
  static const int TASK_ID = NUMPY_BINARY_OP;
  static const int REGIONS = 3;

 public:
  static void cpu_variant(const Legion::Task* task,
                          const std::vector<Legion::PhysicalRegion>& regions,
                          Legion::Context ctx,
                          Legion::Runtime* runtime);
#ifdef LEGATE_USE_OPENMP
  static void omp_variant(const Legion::Task* task,
                          const std::vector<Legion::PhysicalRegion>& regions,
                          Legion::Context ctx,
                          Legion::Runtime* runtime);
#endif
#ifdef LEGATE_USE_CUDA
  static void gpu_variant(const Legion::Task* task,
                          const std::vector<Legion::PhysicalRegion>& regions,
                          Legion::Context ctx,
                          Legion::Runtime* runtime);
#endif
};

enum class BinaryOpCode : int {
  ADD = 1,
  DIVIDE,
  EQUAL,
  FLOOR_DIVIDE,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,
  MAXIMUM,
  MINIMUM,
  MOD,
  MULTIPLY,
  NOT_EQUAL,
  POWER,
  SUBTRACT,
};

void deserialize(Deserializer& ctx, BinaryOpCode& code);

template <typename Functor, typename... Fnargs>
constexpr decltype(auto) op_dispatch(BinaryOpCode op_code, Functor f, Fnargs&&... args)
{
  switch (op_code) {
    case BinaryOpCode::ADD:
      return f.template operator()<BinaryOpCode::ADD>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::DIVIDE:
      return f.template operator()<BinaryOpCode::DIVIDE>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::EQUAL:
      return f.template operator()<BinaryOpCode::EQUAL>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::FLOOR_DIVIDE:
      return f.template operator()<BinaryOpCode::FLOOR_DIVIDE>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::GREATER:
      return f.template operator()<BinaryOpCode::GREATER>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::GREATER_EQUAL:
      return f.template operator()<BinaryOpCode::GREATER_EQUAL>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::LESS:
      return f.template operator()<BinaryOpCode::LESS>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::LESS_EQUAL:
      return f.template operator()<BinaryOpCode::LESS_EQUAL>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::MAXIMUM:
      return f.template operator()<BinaryOpCode::MAXIMUM>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::MINIMUM:
      return f.template operator()<BinaryOpCode::MINIMUM>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::MOD:
      return f.template operator()<BinaryOpCode::MOD>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::MULTIPLY:
      return f.template operator()<BinaryOpCode::MULTIPLY>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::NOT_EQUAL:
      return f.template operator()<BinaryOpCode::NOT_EQUAL>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::POWER:
      return f.template operator()<BinaryOpCode::POWER>(std::forward<Fnargs>(args)...);
    case BinaryOpCode::SUBTRACT:
      return f.template operator()<BinaryOpCode::SUBTRACT>(std::forward<Fnargs>(args)...);
  }
  assert(false);
  return f.template operator()<BinaryOpCode::ADD>(std::forward<Fnargs>(args)...);
}

template <BinaryOpCode OP_CODE, LegateTypeCode CODE>
struct BinaryOp {
  static constexpr bool valid = false;
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::ADD, CODE> : std::plus<legate_type_of<CODE>> {
  static constexpr bool valid = true;
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::DIVIDE, CODE> : std::divides<legate_type_of<CODE>> {
  static constexpr bool valid = true;
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::EQUAL, CODE> : std::equal_to<legate_type_of<CODE>> {
  static constexpr bool valid = true;
};

using std::floor;
template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::FLOOR_DIVIDE, CODE> {
  using T                     = legate_type_of<CODE>;
  static constexpr bool valid = true;
  constexpr T operator()(const T& a, const T& b) const { return floor(a / b); }
};

template <>
struct BinaryOp<BinaryOpCode::FLOOR_DIVIDE, LegateTypeCode::COMPLEX64_LT> {
  static constexpr bool valid = false;
};

template <>
struct BinaryOp<BinaryOpCode::FLOOR_DIVIDE, LegateTypeCode::COMPLEX128_LT> {
  static constexpr bool valid = false;
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::GREATER, CODE> : std::greater<legate_type_of<CODE>> {
  static constexpr bool valid = true;
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::GREATER_EQUAL, CODE> : std::greater_equal<legate_type_of<CODE>> {
  static constexpr bool valid = true;
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::LESS, CODE> : std::less<legate_type_of<CODE>> {
  static constexpr bool valid = true;
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::LESS_EQUAL, CODE> : std::less_equal<legate_type_of<CODE>> {
  static constexpr bool valid = true;
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::MAXIMUM, CODE> {
  using T                     = legate_type_of<CODE>;
  static constexpr bool valid = true;
  constexpr T operator()(const T& a, const T& b) const { return std::max<T>(a, b); }
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::MINIMUM, CODE> {
  using T                     = legate_type_of<CODE>;
  static constexpr bool valid = true;
  constexpr T operator()(const T& a, const T& b) const { return std::min<T>(a, b); }
};

template <typename T>
constexpr T real_mod(const T& a, const T& b)
{
  T res = std::fmod(a, b);
  if (res) {
    if ((b < static_cast<T>(0)) != (res < static_cast<T>(0))) res += b;
  } else {
    res = std::copysign(static_cast<T>(0), b);
  }
  return res;
}

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::MOD, CODE> {
  using T                     = legate_type_of<CODE>;
  static constexpr bool valid = true;
  template <typename _T = T, std::enable_if_t<std::is_integral<_T>::value>* = nullptr>
  constexpr _T operator()(const _T& a, const _T& b) const
  {
    return a % b;
  }

  template <typename _T = T, std::enable_if_t<!std::is_integral<_T>::value>* = nullptr>
  constexpr _T operator()(const _T& a, const _T& b) const
  {
    return real_mod(a, b);
  }
};

template <>
struct BinaryOp<BinaryOpCode::MOD, LegateTypeCode::HALF_LT> {
  static constexpr bool valid = true;
  LEGATE_DEVICE_PREFIX __half operator()(const __half& a, const __half& b) const
  {
    return static_cast<__half>(real_mod(static_cast<float>(a), static_cast<float>(b)));
  }
};

template <>
struct BinaryOp<BinaryOpCode::MOD, LegateTypeCode::COMPLEX64_LT> {
  static constexpr bool valid = false;
};

template <>
struct BinaryOp<BinaryOpCode::MOD, LegateTypeCode::COMPLEX128_LT> {
  static constexpr bool valid = false;
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::MULTIPLY, CODE> : std::multiplies<legate_type_of<CODE>> {
  static constexpr bool valid = true;
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::NOT_EQUAL, CODE> : std::not_equal_to<legate_type_of<CODE>> {
  static constexpr bool valid = true;
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::POWER, CODE> {
  using VAL                   = legate_type_of<CODE>;
  static constexpr bool valid = true;
  constexpr VAL operator()(const VAL& a, const VAL& b) const { return std::pow(a, b); }
};

template <>
struct BinaryOp<BinaryOpCode::POWER, LegateTypeCode::HALF_LT> {
  static constexpr bool valid = true;
  LEGATE_DEVICE_PREFIX __half operator()(const __half& a, const __half& b) const
  {
    return pow(a, b);
  }
};

template <>
struct BinaryOp<BinaryOpCode::POWER, LegateTypeCode::COMPLEX64_LT> {
  static constexpr bool valid = true;
  LEGATE_DEVICE_PREFIX complex<float> operator()(const complex<float>& a,
                                                 const complex<float>& b) const
  {
    return pow(a, b);
  }
};

template <>
struct BinaryOp<BinaryOpCode::POWER, LegateTypeCode::COMPLEX128_LT> {
  static constexpr bool valid = true;
  LEGATE_DEVICE_PREFIX complex<double> operator()(const complex<double>& a,
                                                  const complex<double>& b) const
  {
    return pow(a, b);
  }
};

template <LegateTypeCode CODE>
struct BinaryOp<BinaryOpCode::SUBTRACT, CODE> : std::minus<legate_type_of<CODE>> {
  static constexpr bool valid = true;
};

}  // namespace numpy
}  // namespace legate