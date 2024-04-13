check_no_ext_ops(codegen_scope *s, uint16_t a, uint16_t b)
{
  if (s->parser->no_ext_ops && (a | b) > 0xff) {
    codegen_error(s, "need OP_EXTs instruction (currently OP_EXTs are prohibited)");
  }
}