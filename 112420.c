bool HHVM_FUNCTION(shuffle,
                   VRefParam array) {
  if (!array.isArray()) {
    throw_expected_array_exception("shuffle");
    return false;
  }
  array.assignIfRef(ArrayUtil::Shuffle(array));
  return true;
}