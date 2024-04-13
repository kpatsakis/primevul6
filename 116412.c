push_mode(int mode, int val)
{
  struct mode_entry * new_stack = renewn(mode_stack, mode_stack_len + 1);
  if (new_stack) {
    mode_stack = new_stack;
    mode_stack[mode_stack_len].mode = mode;
    mode_stack[mode_stack_len].val = val;
    mode_stack_len++;
  }
}