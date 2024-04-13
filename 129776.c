verifier_load_method (VerifyContext *ctx, int token, const char *opcode) {
	MonoMethod* method;
	
	if (!IS_METHOD_DEF_OR_REF_OR_SPEC (token) || !token_bounds_check (ctx->image, token)) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid method token 0x%08x for %s at 0x%04x", token, opcode, ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
		return NULL;
	}

	method = mono_get_method_full (ctx->image, token, NULL, ctx->generic_context);

	if (!method || mono_loader_get_last_error ()) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Cannot load method from token 0x%08x for %s at 0x%04x", token, opcode, ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
		mono_loader_clear_error ();
		return NULL;
	}
	
	if (mono_method_is_valid_in_context (ctx, method) == RESULT_INVALID)
		return NULL;

	return method;
}