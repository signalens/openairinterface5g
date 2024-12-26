add_library(ldpc MODULE
  nrLDPC_coding_segment_decoder.c
  nrLDPC_coding_segment_encoder.c
  ${PHY_NR_CODINGIF}
)
set_target_properties(ldpc PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_dependencies(nr-softmodem ldpc)
add_dependencies(nr-uesoftmodem ldpc)
add_dependencies(nr_ulsim ldpc)
add_dependencies(nr_ulschsim ldpc)
add_dependencies(nr_dlsim ldpc)
add_dependencies(nr_dlschsim ldpc)

add_dependencies(ldpc ldpc_orig ldpc_optim ldpc_optim8seg ldpc_optim8segmulti)
if (ENABLE_LDPC_CUDA)
  add_dependencies(ldpc ldpc_cuda)
endif()
