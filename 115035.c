determine_versionability (struct cgraph_node *node,
			  class ipa_node_params *info)
{
  const char *reason = NULL;

  /* There are a number of generic reasons functions cannot be versioned.  We
     also cannot remove parameters if there are type attributes such as fnspec
     present.  */
  if (node->alias || node->thunk.thunk_p)
    reason = "alias or thunk";
  else if (!node->versionable)
    reason = "not a tree_versionable_function";
  else if (node->get_availability () <= AVAIL_INTERPOSABLE)
    reason = "insufficient body availability";
  else if (!opt_for_fn (node->decl, optimize)
	   || !opt_for_fn (node->decl, flag_ipa_cp))
    reason = "non-optimized function";
  else if (lookup_attribute ("omp declare simd", DECL_ATTRIBUTES (node->decl)))
    {
      /* Ideally we should clone the SIMD clones themselves and create
	 vector copies of them, so IPA-cp and SIMD clones can happily
	 coexist, but that may not be worth the effort.  */
      reason = "function has SIMD clones";
    }
  else if (lookup_attribute ("target_clones", DECL_ATTRIBUTES (node->decl)))
    {
      /* Ideally we should clone the target clones themselves and create
	 copies of them, so IPA-cp and target clones can happily
	 coexist, but that may not be worth the effort.  */
      reason = "function target_clones attribute";
    }
  /* Don't clone decls local to a comdat group; it breaks and for C++
     decloned constructors, inlining is always better anyway.  */
  else if (node->comdat_local_p ())
    reason = "comdat-local function";
  else if (node->calls_comdat_local)
    {
      /* TODO: call is versionable if we make sure that all
	 callers are inside of a comdat group.  */
      reason = "calls comdat-local function";
    }

  /* Functions calling BUILT_IN_VA_ARG_PACK and BUILT_IN_VA_ARG_PACK_LEN
     work only when inlined.  Cloning them may still lead to better code
     because ipa-cp will not give up on cloning further.  If the function is
     external this however leads to wrong code because we may end up producing
     offline copy of the function.  */
  if (DECL_EXTERNAL (node->decl))
    for (cgraph_edge *edge = node->callees; !reason && edge;
	 edge = edge->next_callee)
      if (fndecl_built_in_p (edge->callee->decl, BUILT_IN_NORMAL))
        {
	  if (DECL_FUNCTION_CODE (edge->callee->decl) == BUILT_IN_VA_ARG_PACK)
	    reason = "external function which calls va_arg_pack";
	  if (DECL_FUNCTION_CODE (edge->callee->decl)
	      == BUILT_IN_VA_ARG_PACK_LEN)
	    reason = "external function which calls va_arg_pack_len";
        }

  if (reason && dump_file && !node->alias && !node->thunk.thunk_p)
    fprintf (dump_file, "Function %s is not versionable, reason: %s.\n",
	     node->dump_name (), reason);

  info->versionable = (reason == NULL);
}