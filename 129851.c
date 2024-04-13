is_valid_generic_instantiation_in_context (VerifyContext *ctx, MonoGenericInst *ginst)
{
	int i;
	for (i = 0; i < ginst->type_argc; ++i) {
		MonoType *type = ginst->type_argv [i];
		if (!is_valid_type_in_context (ctx, type))
			return FALSE;
	}
	return TRUE;
}