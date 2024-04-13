check_is_valid_type_for_field_ops (VerifyContext *ctx, int token, ILStackDesc *obj, MonoClassField **ret_field, const char *opcode)
{
	MonoClassField *field;
	MonoClass *klass;
	gboolean is_pointer;

	/*must be a reference type, a managed pointer, an unamanaged pointer, or a valuetype*/
	if (!(field = verifier_load_field (ctx, token, &klass, opcode)))
		return FALSE;

	*ret_field = field;
	//the value on stack is going to be used as a pointer
	is_pointer = stack_slot_get_type (obj) == TYPE_PTR || (stack_slot_get_type (obj) == TYPE_NATIVE_INT && !get_stack_type (&field->parent->byval_arg));

	if (field->type->type == MONO_TYPE_TYPEDBYREF) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Typedbyref field is an unverfiable type at 0x%04x", ctx->ip_offset));
		return FALSE;
	}
	g_assert (obj->type);

	/*The value on the stack must be a subclass of the defining type of the field*/ 
	/* we need to check if we can load the field from the stack value*/
	if (is_pointer) {
		if (stack_slot_get_underlying_type (obj) == TYPE_NATIVE_INT)
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Native int is not a verifiable type to reference a field at 0x%04x", ctx->ip_offset));

		if (!IS_SKIP_VISIBILITY (ctx) && !mono_method_can_access_field_full (ctx->method, field, NULL))
				CODE_NOT_VERIFIABLE2 (ctx, g_strdup_printf ("Type at stack is not accessible at 0x%04x", ctx->ip_offset), MONO_EXCEPTION_FIELD_ACCESS);
	} else {
		if (!field->parent->valuetype && stack_slot_is_managed_pointer (obj))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Type at stack is a managed pointer to a reference type and is not compatible to reference the field at 0x%04x", ctx->ip_offset));

		/*a value type can be loaded from a value or a managed pointer, but not a boxed object*/
		if (field->parent->valuetype && stack_slot_is_boxed_value (obj))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Type at stack is a boxed valuetype and is not compatible to reference the field at 0x%04x", ctx->ip_offset));

		if (!stack_slot_is_null_literal (obj) && !verify_stack_type_compatibility_full (ctx, &field->parent->byval_arg, obj, TRUE, FALSE)) {
			char *found = stack_slot_full_name (obj);
			char *expected = mono_type_full_name (&field->parent->byval_arg);
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Expected type '%s' but found '%s' referencing the 'this' argument at 0x%04x", expected, found, ctx->ip_offset));
			g_free (found);
			g_free (expected);
		}

		if (!IS_SKIP_VISIBILITY (ctx) && !mono_method_can_access_field_full (ctx->method, field, mono_class_from_mono_type (obj->type)))
			CODE_NOT_VERIFIABLE2 (ctx, g_strdup_printf ("Type at stack is not accessible at 0x%04x", ctx->ip_offset), MONO_EXCEPTION_FIELD_ACCESS);
	} 

	check_unmanaged_pointer (ctx, obj);
	return TRUE;
}