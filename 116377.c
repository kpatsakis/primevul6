pop_attrs(cattr * _ca, cattrflags * _caflagsmask)
{
  if (!cattr_stack_len)
    return false;
  cattr_stack_len--;
  //printf("pop_attrs[%d] %llX\n", cattr_stack_len, cattr_stack[cattr_stack_len].mask);
  *_ca = cattr_stack[cattr_stack_len].ca;
  *_caflagsmask = cattr_stack[cattr_stack_len].mask;
  return true;
}