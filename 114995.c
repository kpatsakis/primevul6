ipa_get_indirect_edge_target_1 (struct cgraph_edge *ie,
				vec<tree> known_csts,
				vec<ipa_polymorphic_call_context> known_contexts,
				vec<ipa_agg_value_set> known_aggs,
				struct ipa_agg_replacement_value *agg_reps,
				bool *speculative)
{
  int param_index = ie->indirect_info->param_index;
  HOST_WIDE_INT anc_offset;
  tree t = NULL;
  tree target = NULL;

  *speculative = false;

  if (param_index == -1)
    return NULL_TREE;

  if (!ie->indirect_info->polymorphic)
    {
      tree t = NULL;

      if (ie->indirect_info->agg_contents)
	{
	  t = NULL;
	  if (agg_reps && ie->indirect_info->guaranteed_unmodified)
	    {
	      while (agg_reps)
		{
		  if (agg_reps->index == param_index
		      && agg_reps->offset == ie->indirect_info->offset
		      && agg_reps->by_ref == ie->indirect_info->by_ref)
		    {
		      t = agg_reps->value;
		      break;
		    }
		  agg_reps = agg_reps->next;
		}
	    }
	  if (!t)
	    {
	      struct ipa_agg_value_set *agg;
	      if (known_aggs.length () > (unsigned int) param_index)
		agg = &known_aggs[param_index];
	      else
		agg = NULL;
	      bool from_global_constant;
	      t = ipa_find_agg_cst_for_param (agg,
					      (unsigned) param_index
						 < known_csts.length ()
					      ? known_csts[param_index]
					      : NULL,
					      ie->indirect_info->offset,
					      ie->indirect_info->by_ref,
					      &from_global_constant);
	      if (t
		  && !from_global_constant
		  && !ie->indirect_info->guaranteed_unmodified)
		t = NULL_TREE;
	    }
	}
      else if ((unsigned) param_index < known_csts.length ())
	t = known_csts[param_index];

      if (t
	  && TREE_CODE (t) == ADDR_EXPR
	  && TREE_CODE (TREE_OPERAND (t, 0)) == FUNCTION_DECL)
	return TREE_OPERAND (t, 0);
      else
	return NULL_TREE;
    }

  if (!opt_for_fn (ie->caller->decl, flag_devirtualize))
    return NULL_TREE;

  gcc_assert (!ie->indirect_info->agg_contents);
  anc_offset = ie->indirect_info->offset;

  t = NULL;

  /* Try to work out value of virtual table pointer value in replacements.  */
  if (!t && agg_reps && !ie->indirect_info->by_ref)
    {
      while (agg_reps)
	{
	  if (agg_reps->index == param_index
	      && agg_reps->offset == ie->indirect_info->offset
	      && agg_reps->by_ref)
	    {
	      t = agg_reps->value;
	      break;
	    }
	  agg_reps = agg_reps->next;
	}
    }

  /* Try to work out value of virtual table pointer value in known
     aggregate values.  */
  if (!t && known_aggs.length () > (unsigned int) param_index
      && !ie->indirect_info->by_ref)
    {
      struct ipa_agg_value_set *agg = &known_aggs[param_index];
      t = ipa_find_agg_cst_for_param (agg,
				      (unsigned) param_index
					 < known_csts.length ()
				      ? known_csts[param_index] : NULL,
				      ie->indirect_info->offset, true);
    }

  /* If we found the virtual table pointer, lookup the target.  */
  if (t)
    {
      tree vtable;
      unsigned HOST_WIDE_INT offset;
      if (vtable_pointer_value_to_vtable (t, &vtable, &offset))
	{
	  bool can_refer;
	  target = gimple_get_virt_method_for_vtable (ie->indirect_info->otr_token,
						      vtable, offset, &can_refer);
	  if (can_refer)
	    {
	      if (!target
		  || fndecl_built_in_p (target, BUILT_IN_UNREACHABLE)
		  || !possible_polymorphic_call_target_p
		       (ie, cgraph_node::get (target)))
		{
		  /* Do not speculate builtin_unreachable, it is stupid!  */
		  if (ie->indirect_info->vptr_changed)
		    return NULL;
		  target = ipa_impossible_devirt_target (ie, target);
		}
	      *speculative = ie->indirect_info->vptr_changed;
	      if (!*speculative)
		return target;
	    }
	}
    }

  /* Do we know the constant value of pointer?  */
  if (!t && (unsigned) param_index < known_csts.length ())
    t = known_csts[param_index];

  gcc_checking_assert (!t || TREE_CODE (t) != TREE_BINFO);

  ipa_polymorphic_call_context context;
  if (known_contexts.length () > (unsigned int) param_index)
    {
      context = known_contexts[param_index];
      context.offset_by (anc_offset);
      if (ie->indirect_info->vptr_changed)
	context.possible_dynamic_type_change (ie->in_polymorphic_cdtor,
					      ie->indirect_info->otr_type);
      if (t)
	{
	  ipa_polymorphic_call_context ctx2 = ipa_polymorphic_call_context
	    (t, ie->indirect_info->otr_type, anc_offset);
	  if (!ctx2.useless_p ())
	    context.combine_with (ctx2, ie->indirect_info->otr_type);
	}
    }
  else if (t)
    {
      context = ipa_polymorphic_call_context (t, ie->indirect_info->otr_type,
					      anc_offset);
      if (ie->indirect_info->vptr_changed)
	context.possible_dynamic_type_change (ie->in_polymorphic_cdtor,
					      ie->indirect_info->otr_type);
    }
  else
    return NULL_TREE;

  vec <cgraph_node *>targets;
  bool final;

  targets = possible_polymorphic_call_targets
    (ie->indirect_info->otr_type,
     ie->indirect_info->otr_token,
     context, &final);
  if (!final || targets.length () > 1)
    {
      struct cgraph_node *node;
      if (*speculative)
	return target;
      if (!opt_for_fn (ie->caller->decl, flag_devirtualize_speculatively)
	  || ie->speculative || !ie->maybe_hot_p ())
	return NULL;
      node = try_speculative_devirtualization (ie->indirect_info->otr_type,
					       ie->indirect_info->otr_token,
					       context);
      if (node)
	{
	  *speculative = true;
	  target = node->decl;
	}
      else
	return NULL;
    }
  else
    {
      *speculative = false;
      if (targets.length () == 1)
	target = targets[0]->decl;
      else
	target = ipa_impossible_devirt_target (ie, NULL_TREE);
    }

  if (target && !possible_polymorphic_call_target_p (ie,
						     cgraph_node::get (target)))
    {
      if (*speculative)
	return NULL;
      target = ipa_impossible_devirt_target (ie, target);
    }

  return target;
}