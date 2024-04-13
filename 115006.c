modify_known_vectors_with_val (vec<tree> *known_csts,
			       vec<ipa_polymorphic_call_context> *known_contexts,
			       ipcp_value<tree> *val,
			       int index)
{
  *known_csts = known_csts->copy ();
  *known_contexts = copy_useful_known_contexts (*known_contexts);
  (*known_csts)[index] = val->value;
}