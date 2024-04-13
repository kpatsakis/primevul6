copy_useful_known_contexts (vec<ipa_polymorphic_call_context> known_contexts)
{
  if (known_contexts_useful_p (known_contexts))
    return known_contexts.copy ();
  else
    return vNULL;
}