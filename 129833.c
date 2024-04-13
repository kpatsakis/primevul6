do_ldlen (VerifyContext *ctx)
{
	ILStackDesc *value;

	if (!check_underflow (ctx, 1))
		return;

	value = stack_pop (ctx);

	if (stack_slot_get_type (value) != TYPE_COMPLEX || value->type->type != MONO_TYPE_SZARRAY)
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid array type for ldlen at 0x%04x", ctx->ip_offset));

	stack_push_val (ctx, TYPE_NATIVE_INT, &mono_defaults.int_class->byval_arg);	
}