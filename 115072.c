known_contexts_useful_p (vec<ipa_polymorphic_call_context> known_contexts)
{
  ipa_polymorphic_call_context *ctx;
  int i;

  FOR_EACH_VEC_ELT (known_contexts, i, ctx)
    if (!ctx->useless_p ())
      return true;
  return false;
}