push_attrs(cattr ca, cattrflags caflagsmask)
{
  if (cattr_stack_len == lengthof(cattr_stack)) {
    for (int i = 1; i < cattr_stack_len; i++)
      cattr_stack[i - 1] = cattr_stack[i];
    cattr_stack_len--;
  }
  //printf("push_attrs[%d] %llX\n", cattr_stack_len, caflagsmask);
  cattr_stack[cattr_stack_len].ca = ca;
  cattr_stack[cattr_stack_len].mask = caflagsmask;
  cattr_stack_len++;
}