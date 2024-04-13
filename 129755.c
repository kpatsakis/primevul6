do_unbox_any (VerifyContext *ctx, int klass_token)
{
	ILStackDesc *value;
	MonoType *type = get_boxable_mono_type (ctx, klass_token, "unbox.any");

	if (!type)
		return;
 
	if (!check_underflow (ctx, 1))
		return;

	value = stack_pop (ctx);

	/*Value should be: a boxed valuetype or a reference type*/
	if (!(stack_slot_get_type (value) == TYPE_COMPLEX &&
		(stack_slot_is_boxed_value (value) || !mono_class_from_mono_type (value->type)->valuetype)))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid type %s at stack for unbox.any operation at 0x%04x", stack_slot_get_name (value), ctx->ip_offset));
 
	set_stack_value (ctx, stack_push (ctx), type, FALSE);
}