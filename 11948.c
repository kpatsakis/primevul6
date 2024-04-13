dirserv_estimate_microdesc_size(const smartlist_t *fps, int compressed)
{
  size_t result = smartlist_len(fps) * microdesc_average_size(NULL);
  if (compressed)
    result /= 2;
  return result;
}