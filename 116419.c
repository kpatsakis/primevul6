pop_mode(int mode)
{
  for (int i = mode_stack_len - 1; i >= 0; i--)
    if (mode_stack[i].mode == mode) {
      int val = mode_stack[i].val;
      mode_stack_len--;
      for (int j = i; j < mode_stack_len; j++)
        mode_stack[j] = mode_stack[j + 1];
      struct mode_entry * new_stack = renewn(mode_stack, mode_stack_len);
      if (new_stack)
        mode_stack = new_stack;
      return val;
    }
  return -1;
}