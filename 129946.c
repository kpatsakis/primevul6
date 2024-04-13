do_push_field (VerifyContext *ctx, int token, gboolean take_addr)
{
	ILStackDesc *obj;
	MonoClassField *field;

	if (!take_addr)
		CLEAR_PREFIX (ctx, PREFIX_UNALIGNED | PREFIX_VOLATILE);

	if (!check_underflow (ctx, 1))
		return;
	obj = stack_pop_safe (ctx);

	if (!check_is_valid_type_for_field_ops (ctx, token, obj, &field, take_addr ? "ldflda" : "ldfld"))
		return;

	if (take_addr && field->parent->valuetype && !stack_slot_is_managed_pointer (obj))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot take the address of a temporary value-type at 0x%04x", ctx->ip_offset));

	if (take_addr && (field->type->attrs & FIELD_ATTRIBUTE_INIT_ONLY) &&
		!(field->parent == ctx->method->klass && mono_method_is_constructor (ctx->method)))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot take the address of a init-only field at 0x%04x", ctx->ip_offset));

	set_stack_value (ctx, stack_push (ctx), field->type, take_addr);
}