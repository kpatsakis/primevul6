static bool isMask(ut32 value) {
  return value && ((value + 1) & value) == 0;
}