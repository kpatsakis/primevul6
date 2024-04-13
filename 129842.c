do_unbox_value (VerifyContext *ctx, int klass_token)
{
	ILStackDesc *value;
	MonoType *type = get_boxable_mono_type (ctx, klass_token, "unbox");

	if (!type)
		return;
 
	if (!check_underflow (ctx, 1))
		return;

	if (!mono_class_from_mono_type (type)->valuetype)
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid reference type for unbox at 0x%04x", ctx->ip_offset));

	value = stack_pop (ctx);

	/*Value should be: a boxed valuetype or a reference type*/
	if (!(stack_slot_get_type (value) == TYPE_COMPLEX &&
		(stack_slot_is_boxed_value (value) || !mono_class_from_mono_type (value->type)->valuetype)))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid type %s at stack for unbox operation at 0x%04x", stack_slot_get_name (value), ctx->ip_offset));

	set_stack_value (ctx, value = stack_push (ctx), mono_type_get_type_byref (type), FALSE);
	value->stype |= CMMP_MASK;
}