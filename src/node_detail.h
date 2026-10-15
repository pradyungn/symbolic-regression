#pragma once

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <node.h>
#include <vector>
#include <iostream>

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
inline std::vector<float> looped_evaluate_node(const node &n, const float* data, const uint64_t stride,
                                               const uint64_t offset, const uint64_t rowsize,
                                               const std::vector<float> &in1, const std::vector<float> &in2) {
  if (n.t == node::type::constant) {
    return std::vector<float>(rowsize, n.u.val);
  } else if (n.t == node::type::variable) {
    return std::vector<float>(data + (stride*n.u.fid) + offset,
                              data + (stride*n.u.fid) + offset + rowsize);
  } else {
    auto nodetype = n.t;
    std::vector<float> results(rowsize); // could reuse in1, but this improves vectorization

    // note: keep the case statements in alphabetical order under each category
    // of operators.
    for (uint64_t itr=0; itr<rowsize; itr++) {
      // note: keep the case statements in alphabetical order under each category
      // of operators.
      switch (nodetype) {
      // binary operators
      case node::type::add:
        results[itr] = in1[itr] + in2[itr];
        break;
      case node::type::atan2:
        results[itr] = atan2f(in1[itr], in2[itr]);
        break;
      case node::type::div:
        results[itr] = fabsf(in2[itr]) < MIN_VAL ? 1.0f : (in1[itr]/in2[itr]);//fdividef(in1[itr], in2[itr]);
        break;
      case node::type::fdim:
        results[itr] = fdimf(in1[itr], in2[itr]);
        break;
      case node::type::gt:
        results[itr] = in1[itr] > in2[itr] ? 1.0f : 0.0f;
        break;
      case node::type::max:
        results[itr] = fmaxf(in1[itr], in2[itr]);
        break;
      case node::type::min:
        results[itr] = fminf(in1[itr], in2[itr]);
        break;
      case node::type::mul:
        results[itr] = in1[itr] * in2[itr];
        break;
      case node::type::pow:
        results[itr] = powf(in1[itr], in2[itr]);
        break;
      case node::type::sub:
        results[itr] = in1[itr] - in2[itr];
        break;

        // unary operators
      case node::type::abs:
        results[itr] = fabsf(in1[itr]);
        break;
      case node::type::acos:
        results[itr] = acosf(in1[itr]);
        break;
      case node::type::acosh:
        results[itr] = acoshf(in1[itr]);
        break;
      case node::type::asin:
        results[itr] = asinf(in1[itr]);
        break;
      case node::type::asinh:
        results[itr] = asinhf(in1[itr]);
        break;
      case node::type::atan:
        results[itr] = atanf(in1[itr]);
        break;
      case node::type::atanh:
        results[itr] = atanhf(in1[itr]);
        break;
      case node::type::cbrt:
        results[itr] = cbrtf(in1[itr]);
        break;
      case node::type::cos:
        results[itr] = cosf(in1[itr]);
        break;
      case node::type::cosh:
        results[itr] = coshf(in1[itr]);
        break;
      case node::type::cube:
        results[itr] = in1[itr] * in1[itr] * in1[itr];
        break;
      case node::type::exp:
        results[itr] = expf(in1[itr]);
        break;
      case node::type::inv:
        results[itr] = fabsf(in1[itr]) < MIN_VAL ? 0.f : 1.f / in1[itr];
        break;
      case node::type::log:
        results[itr] = fabsf(in1[itr]) < MIN_VAL ? 0.f : logf(fabsf(in1[itr]));
        break;
      case node::type::neg:
        results[itr] = -in1[itr];
        break;
      case node::type::rcbrt:
        results[itr] = static_cast<float>(1.0) / cbrtf(in1[itr]);
        break;
      case node::type::rsqrt:
        results[itr] = static_cast<float>(1.0) / sqrtf(fabsf(in1[itr]));
        break;
      case node::type::sin:
        results[itr] = sinf(in1[itr]);
        break;
      case node::type::sinh:
        results[itr] = sinhf(in1[itr]);
        break;
      case node::type::sq:
        results[itr] = in1[itr] * in1[itr];
        break;
      case node::type::sqrt:
        results[itr] = sqrtf(fabsf(in1[itr]));
        break;
      case node::type::tan:
        results[itr] = tanf(in1[itr]);
        break;
      case node::type::tanh:
        results[itr] = tanhf(in1[itr]);
        break;
      // shouldn't reach here!
      default:
        results[itr] = 0.f;
      };
    }

    return results;
  }
}

} // namespace detail
} // namespace genetic
