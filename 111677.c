void FinalizeAggregation(TfLiteCombinerType combiner, int num_elements,
                         float current_total_weight,
                         float current_squares_weight, int embedding_size,
                         float* output) {
  if (combiner != kTfLiteCombinerTypeSum && num_elements > 0) {
    float multiplier = 1.0;
    switch (combiner) {
      case kTfLiteCombinerTypeMean:
        multiplier = current_total_weight;
        break;
      case kTfLiteCombinerTypeSqrtn:
        multiplier = std::sqrt(current_squares_weight);
        break;
      default:
        break;
    }
    for (int k = 0; k < embedding_size; k++) {
      output[k] /= multiplier;
    }
  }
}