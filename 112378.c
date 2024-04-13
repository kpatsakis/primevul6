TypedValue HHVM_FUNCTION(array_count_values,
                         ArrayArg input) {
  return tvReturn(ArrayUtil::CountValues(ArrNR(input.get())));
}