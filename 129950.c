do_refanytype (VerifyContext *ctx)
{
	ILStackDesc *top;

	if (!check_underflow (ctx, 1))
		return;

	top = stack_pop (ctx);

	if (top->stype != TYPE_PTR || top->type->type != MONO_TYPE_TYPEDBYREF)
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Expected a typedref as argument for refanytype, but found %s at 0x%04x", stack_slot_get_name (top), ctx->ip_offset));

	set_stack_value (ctx, stack_push (ctx), &mono_defaults.typehandle_class->byval_arg, FALSE);

}