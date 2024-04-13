static bool isShiftedMask (ut32 value) {
  return value && isMask ((value - 1) | value);
}