#pragma once

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <node.h>
#include <vector>
#include <iostream>
#include <algorithm>

namespace genetic {
namespace detail {

static constexpr float MIN_VAL = 0.001f;

inline bool is_terminal(node::type t) {
  return t == node::type::variable || t == node::type::constant;
}

inline bool is_nonterminal(node::type t) { return !is_terminal(t); }

inline int arity(node::type t) {
  if (node::type::unary_begin <= t && t <= node::type::unary_end) {
    return 1;
  }
  if (node::type::binary_begin <= t && t <= node::type::binary_end) {
    return 2;
  }
  return 0;
}

// `data` assumed to be stored in col-major format
inline float evaluate_node(const node &n, const float *data, const uint64_t stride,
                    const uint64_t idx, const float *in) {
  if (n.t == node::type::constant) {
    return n.u.val;
  } else if (n.t == node::type::variable) {
    return data[(stride * n.u.fid) + idx];
  } else {
    auto abs_inval = fabsf(in[0]), abs_inval1 = fabsf(in[1]);
    // note: keep the case statements in alphabetical order under each category
    // of operators.
    switch (n.t) {
    // binary operators
    case node::type::add:
      return in[0] + in[1];
    case node::type::atan2:
      return atan2f(in[0], in[1]);
    case node::type::div:
      return abs_inval1 < MIN_VAL ? 1.0f : (in[0]/in[1]);//fdividef(in[0], in[1]);
    case node::type::fdim:
      return fdimf(in[0], in[1]);
    case node::type::gt:
      return in[0] > in[1] ? 1.0f : 0.0f;
    case node::type::max:
      return fmaxf(in[0], in[1]);
    case node::type::min:
      return fminf(in[0], in[1]);
    case node::type::mul:
      return in[0] * in[1];
    case node::type::pow:
      return powf(in[0], in[1]);
    case node::type::sub:
      return in[0] - in[1];
    // unary operators
    case node::type::abs:
      return abs_inval;
    case node::type::acos:
      return acosf(in[0]);
    case node::type::acosh:
      return acoshf(in[0]);
    case node::type::asin:
      return asinf(in[0]);
    case node::type::asinh:
      return asinhf(in[0]);
    case node::type::atan:
      return atanf(in[0]);
    case node::type::atanh:
      return atanhf(in[0]);
    case node::type::cbrt:
      return cbrtf(in[0]);
    case node::type::cos:
      return cosf(in[0]);
    case node::type::cosh:
      return coshf(in[0]);
    case node::type::cube:
      return in[0] * in[0] * in[0];
    case node::type::exp:
      return expf(in[0]);
    case node::type::inv:
      return abs_inval < MIN_VAL ? 0.f : 1.f / in[0];
    case node::type::log:
      return abs_inval < MIN_VAL ? 0.f : logf(abs_inval);
    case node::type::neg:
      return -in[0];
    case node::type::rcbrt:
      return static_cast<float>(1.0) / cbrtf(in[0]);
    case node::type::rsqrt:
      return static_cast<float>(1.0) / sqrtf(abs_inval);
    case node::type::sin:
      return sinf(in[0]);
    case node::type::sinh:
      return sinhf(in[0]);
    case node::type::sq:
      return in[0] * in[0];
    case node::type::sqrt:
      return sqrtf(abs_inval);
    case node::type::tan:
      return tanf(in[0]);
    case node::type::tanh:
      return tanhf(in[0]);
    // shouldn't reach here!
    default:
      return 0.f;
    };
  }
}

// `data` assumed to be stored in col-major format
inline void looped_evaluate_node(const node &n, const float* __restrict__ data, const uint64_t stride,
                                 const uint64_t offset, const uint64_t rowsize,
                                 const float* __restrict__ in1, const float* __restrict__ in2,
                                 float* __restrict__ results) {
  if (n.t == node::type::constant) {
    std::fill_n(results, rowsize, n.u.val);
  } else if (n.t == node::type::variable) {
    std::copy_n(data + (stride*n.u.fid) + offset, rowsize, results);
  } else {
    auto nodetype = n.t;

    // note: keep the case statements in alphabetical order under each category
    // of operators.
    switch (nodetype) {
    // binary operators
    case node::type::add:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = in1[itr] + in2[itr];
      break;
    case node::type::atan2:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = atan2f(in1[itr], in2[itr]);
      break;
    case node::type::div:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = fabsf(in2[itr]) < MIN_VAL ? 1.0f : (in1[itr]/in2[itr]);//fdividef(in1[itr], in2[itr]);
      break;
    case node::type::fdim:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = fdimf(in1[itr], in2[itr]);
      break;
    case node::type::gt:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = in1[itr] > in2[itr] ? 1.0f : 0.0f;
      break;
    case node::type::max:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = fmaxf(in1[itr], in2[itr]);
      break;
    case node::type::min:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = fminf(in1[itr], in2[itr]);
      break;
    case node::type::mul:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = in1[itr] * in2[itr];
      break;
    case node::type::pow:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = powf(in1[itr], in2[itr]);
      break;
    case node::type::sub:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = in1[itr] - in2[itr];
      break;

    // unary operators
    case node::type::abs:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = fabsf(in1[itr]);
      break;
    case node::type::acos:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = acosf(in1[itr]);
      break;
    case node::type::acosh:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = acoshf(in1[itr]);
      break;
    case node::type::asin:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = asinf(in1[itr]);
      break;
    case node::type::asinh:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = asinhf(in1[itr]);
      break;
    case node::type::atan:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = atanf(in1[itr]);
      break;
    case node::type::atanh:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = atanhf(in1[itr]);
      break;
    case node::type::cbrt:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = cbrtf(in1[itr]);
      break;
    case node::type::cos:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = cosf(in1[itr]);
      break;
    case node::type::cosh:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = coshf(in1[itr]);
      break;
    case node::type::cube:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = in1[itr] * in1[itr] * in1[itr];
      break;
    case node::type::exp:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = expf(in1[itr]);
      break;
    case node::type::inv:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = fabsf(in1[itr]) < MIN_VAL ? 0.f : 1.f / in1[itr];
      break;
    case node::type::log:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = fabsf(in1[itr]) < MIN_VAL ? 0.f : logf(fabsf(in1[itr]));
      break;
    case node::type::neg:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = -in1[itr];
      break;
    case node::type::rcbrt:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = static_cast<float>(1.0) / cbrtf(in1[itr]);
      break;
    case node::type::rsqrt:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = static_cast<float>(1.0) / sqrtf(fabsf(in1[itr]));
      break;
    case node::type::sin:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = sinf(in1[itr]);
      break;
    case node::type::sinh:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = sinhf(in1[itr]);
      break;
    case node::type::sq:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = in1[itr] * in1[itr];
      break;
    case node::type::sqrt:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = sqrtf(fabsf(in1[itr]));
      break;
    case node::type::tan:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = tanf(in1[itr]);
      break;
    case node::type::tanh:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = tanhf(in1[itr]);
      break;
      // shouldn't reach here!
    default:
      for (uint64_t itr=0; itr < rowsize; itr++) results[itr] = 0.f;
    };
  }
}

} // namespace detail
} // namespace genetic
