onig_set_capture_num_limit(int num)
{
  if (num < 0) return -1;

  MaxCaptureNum = num;
  return 0;
}