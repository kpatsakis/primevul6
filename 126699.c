int64_t mg_to64(struct mg_str str) {
  int64_t result = 0, neg = 1, max = 922337203685477570 /* INT64_MAX/10-10 */;
  size_t i = 0;
  while (i < str.len && (str.ptr[i] == ' ' || str.ptr[i] == '\t')) i++;
  if (i < str.len && str.ptr[i] == '-') neg = -1, i++;
  while (i < str.len && str.ptr[i] >= '0' && str.ptr[i] <= '9') {
    if (result > max) return 0;
    result *= 10;
    result += (str.ptr[i] - '0');
    i++;
  }
  return result * neg;
}