//
// Created by evtus on 1/10/2021.
//

#include <cuda_runtime.h>

#include "questions/basics/mem_copy_01.h"
#include "questions/helpers/array_functions.h"

#include "answer.h"

const char *mem_copy_01_question_t::get_question_content () const
{
  return R"(
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT

__global__ void mem_copy_kernel (
  const float *in,
  float *out)
{
  // TODO Copy variable pointed by in into variable pointed by out
}

extern "C" BOOST_SYMBOL_EXPORT void mem_copy (
  const float *in,
  float *out)
{
  mem_copy_kernel<<<1, 1>>> (in, out);
}
)";
}

bool mem_copy_01_question_t::check_answer_implementation () const
{
  const int n = 1;
  float *in {}, *out {}, *reference {};

  cudaMalloc (&in, n * sizeof (float));
  cudaMalloc (&out, n * sizeof (float));
  cudaMalloc (&reference, n * sizeof (float));

  cudaMemset (in, 42, n * sizeof (float));

  auto user_solution =
      load_answer<void(const float *, float *)> (
          "answer.so", "mem_copy");

  mem_copy_01_kernel_wrapper (n, in, reference);
  check (cudaDeviceSynchronize ());
  check (cudaGetLastError ());

  bool status = true;

  try
  {
    user_solution (in, out);
    check (cudaDeviceSynchronize ());
    check (cudaGetLastError ());
  }
  catch (...)
  {
    status = false;
  }

  if (status)
    status = is_equal (n, reference, out);

  cudaFree (reference);
  cudaFree (out);
  cudaFree (in);

  return status;
}
