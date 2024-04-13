no_optimize(codegen_scope *s)
{
  if (s && s->parser && s->parser->no_optimize)
    return TRUE;
  return FALSE;
}