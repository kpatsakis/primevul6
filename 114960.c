modify_known_vectors_with_val (vec<tree> *known_csts,
			       vec<ipa_polymorphic_call_context> *known_contexts,
			       ipcp_value<ipa_polymorphic_call_context> *val,
			       int index)
{
  *known_csts = known_csts->copy ();
  *known_contexts = known_contexts->copy ();
  (*known_contexts)[index] = val->value;
}