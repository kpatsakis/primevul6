inline bool ReduceSumImpl(const In* input_data, const int* input_dims,
                          const int* output_dims, const int input_num_dims,
                          const int output_num_dims, const int* axis,
                          const int num_axis, int* input_iter,
                          Out* output_data) {
  auto reducer = [](const Out current, const In in) -> Out {
    const Out actual_in = static_cast<Out>(in);
    return current + actual_in;
  };
  return Reduce<In, Out>(input_data, input_dims, output_dims, input_num_dims,
                         output_num_dims, axis, num_axis, input_iter, reducer,
                         output_data);
}