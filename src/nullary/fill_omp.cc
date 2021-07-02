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

#include "nullary/fill.h"
#include "nullary/fill_template.inl"

namespace legate {
namespace numpy {

using namespace Legion;

template <typename T>
struct Identity {
  constexpr T operator()(const T &in) const { return in; }
};

template <typename VAL, int32_t DIM>
struct FillImplBody<VariantKind::OMP, VAL, DIM> {
  void operator()(AccessorWO<VAL, DIM> out,
                  const VAL &fill_value,
                  const Pitches<DIM - 1> &pitches,
                  const Rect<DIM> &rect,
                  bool dense) const
  {
    size_t volume = rect.volume();
    if (dense) {
      auto outptr = out.ptr(rect);
#pragma omp parallel for schedule(static)
      for (size_t idx = 0; idx < volume; ++idx) outptr[idx] = fill_value;
    } else {
      OMPLoop<DIM>::unary_loop(Identity<VAL>{}, out, Scalar<VAL, DIM>(fill_value), rect);
    }
  }
};

/*static*/ void FillTask::omp_variant(const Task *task,
                                      const std::vector<PhysicalRegion> &regions,
                                      Context context,
                                      Runtime *runtime)
{
  fill_template<VariantKind::OMP>(task, regions, context, runtime);
}

}  // namespace numpy
}  // namespace legate