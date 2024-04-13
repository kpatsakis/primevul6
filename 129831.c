do_sizeof (VerifyContext *ctx, int token)
{
	MonoType *type;

	if (!IS_TYPE_DEF_OR_REF_OR_SPEC (token) || !token_bounds_check (ctx->image, token)) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid type token %x at 0x%04x", token, ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
		return;
	}
	
	if (!(type = verifier_load_type (ctx, token, "sizeof")))
		return;

	if (type->byref && type->type != MONO_TYPE_TYPEDBYREF) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid use of byref type at 0x%04x", ctx->ip_offset));
		return;
	}

	if (type->type == MONO_TYPE_VOID) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid use of void type at 0x%04x", ctx->ip_offset));
		return;
	}

	if (check_overflow (ctx))
		set_stack_value (ctx, stack_push (ctx), &mono_defaults.uint32_class->byval_arg, FALSE);
}