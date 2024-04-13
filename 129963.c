do_load_function_ptr (VerifyContext *ctx, guint32 token, gboolean virtual)
{
	ILStackDesc *top;
	MonoMethod *method;

	if (virtual && !check_underflow (ctx, 1))
		return;

	if (!virtual && !check_overflow (ctx))
		return;

	if (!IS_METHOD_DEF_OR_REF_OR_SPEC (token) || !token_bounds_check (ctx->image, token)) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid token %x for ldftn  at 0x%04x", token, ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
		return;
	}

	if (!(method = verifier_load_method (ctx, token, virtual ? "ldvirtfrn" : "ldftn")))
		return;

	if (mono_method_is_constructor (method))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use ldftn with a constructor at 0x%04x", ctx->ip_offset));

	if (virtual) {
		ILStackDesc *top = stack_pop (ctx);
	
		if (stack_slot_get_type (top) != TYPE_COMPLEX || top->type->type == MONO_TYPE_VALUETYPE)
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid argument to ldvirtftn at 0x%04x", ctx->ip_offset));
	
		if (method->flags & METHOD_ATTRIBUTE_STATIC)
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use ldvirtftn with a constructor at 0x%04x", ctx->ip_offset));

		if (!verify_stack_type_compatibility (ctx, &method->klass->byval_arg, top))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Unexpected object for ldvirtftn at 0x%04x", ctx->ip_offset));
	}
	
	if (!mono_method_can_access_method_full (ctx->method, method, NULL))
		CODE_NOT_VERIFIABLE2 (ctx, g_strdup_printf ("Loaded method is not visible for ldftn/ldvirtftn at 0x%04x", ctx->ip_offset), MONO_EXCEPTION_METHOD_ACCESS);

	top = stack_push_val(ctx, TYPE_PTR, mono_type_create_fnptr_from_mono_method (ctx, method));
	top->method = method;
}