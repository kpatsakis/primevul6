static int php_unicode_is_prop(unsigned long code, unsigned long mask1,
                               unsigned long mask2) {
  unsigned long i;

  if (mask1 == 0 && mask2 == 0)
    return 0;

  for (i = 0; mask1 && i < 32; i++) {
    if ((mask1 & masks32[i]) && prop_lookup(code, i))
      return 1;
  }

  for (i = 32; mask2 && i < _ucprop_size; i++) {
    if ((mask2 & masks32[i & 31]) && prop_lookup(code, i))
      return 1;
  }

  return 0;
}