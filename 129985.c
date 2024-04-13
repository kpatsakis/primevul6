verify_delegate_compatibility (VerifyContext *ctx, MonoClass *delegate, ILStackDesc *value, ILStackDesc *funptr)
{
#define IS_VALID_OPCODE(offset, opcode) (ip [ip_offset - offset] == opcode && (ctx->code [ip_offset - offset].flags & IL_CODE_FLAG_SEEN))
#define IS_LOAD_FUN_PTR(kind) (IS_VALID_OPCODE (6, CEE_PREFIX1) && ip [ip_offset - 5] == kind)

	MonoMethod *invoke, *method;
	const guint8 *ip = ctx->header->code;
	guint32 ip_offset = ctx->ip_offset;
	gboolean is_static_ldftn = FALSE, is_first_arg_bound = FALSE;
	
	if (stack_slot_get_type (funptr) != TYPE_PTR || !funptr->method) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid function pointer parameter for delegate constructor at 0x%04x", ctx->ip_offset));
		return;
	}
	
	invoke = mono_get_delegate_invoke (delegate);
	method = funptr->method;

	if (!method || !mono_method_signature (method)) {
		char *name = mono_type_get_full_name (delegate);
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid method on stack to create delegate %s construction at 0x%04x", name, ctx->ip_offset));
		g_free (name);
		return;
	}

	if (!invoke || !mono_method_signature (invoke)) {
		char *name = mono_type_get_full_name (delegate);
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Delegate type %s with bad Invoke method at 0x%04x", name, ctx->ip_offset));
		g_free (name);
		return;
	}

	is_static_ldftn = (ip_offset > 5 && IS_LOAD_FUN_PTR (CEE_LDFTN)) && method->flags & METHOD_ATTRIBUTE_STATIC;

	if (is_static_ldftn)
		is_first_arg_bound = mono_method_signature (invoke)->param_count + 1 ==  mono_method_signature (method)->param_count;

	if (!mono_delegate_signature_equal (mono_method_signature (invoke), mono_method_signature (method), is_first_arg_bound)) {
		char *fun_sig = mono_signature_get_desc (mono_method_signature (method), FALSE);
		char *invoke_sig = mono_signature_get_desc (mono_method_signature (invoke), FALSE);
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Function pointer signature '%s' doesn't match delegate's signature '%s' at 0x%04x", fun_sig, invoke_sig, ctx->ip_offset));
		g_free (fun_sig);
		g_free (invoke_sig);
	}

	/* 
	 * Delegate code sequences:
	 * [-6] ldftn token
	 * newobj ...
	 * 
	 * 
	 * [-7] dup
	 * [-6] ldvirtftn token
	 * newobj ...
	 * 
	 * ldftn sequence:*/
	if (ip_offset > 5 && IS_LOAD_FUN_PTR (CEE_LDFTN)) {
		verify_ldftn_delegate (ctx, delegate, value, funptr);
	} else if (ip_offset > 6 && IS_VALID_OPCODE (7, CEE_DUP) && IS_LOAD_FUN_PTR (CEE_LDVIRTFTN)) {
		ctx->code [ip_offset - 6].flags |= IL_CODE_DELEGATE_SEQUENCE;	
	}else {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid code sequence for delegate creation at 0x%04x", ctx->ip_offset));
	}
	ctx->code [ip_offset].flags |= IL_CODE_DELEGATE_SEQUENCE;

	//general tests
	if (is_first_arg_bound) {
		if (mono_method_signature (method)->param_count == 0 || !verify_stack_type_compatibility_full (ctx, mono_method_signature (method)->params [0], value, FALSE, TRUE))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("This object not compatible with function pointer for delegate creation at 0x%04x", ctx->ip_offset));
	} else {
		if (method->flags & METHOD_ATTRIBUTE_STATIC) {
			if (!stack_slot_is_null_literal (value) && !is_first_arg_bound)
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Non-null this args used with static function for delegate creation at 0x%04x", ctx->ip_offset));
		} else {
			if (!verify_stack_type_compatibility_full (ctx, &method->klass->byval_arg, value, FALSE, TRUE) && !stack_slot_is_null_literal (value))
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("This object not compatible with function pointer for delegate creation at 0x%04x", ctx->ip_offset));
		}
	}

	if (stack_slot_get_type (value) != TYPE_COMPLEX)
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid first parameter for delegate creation at 0x%04x", ctx->ip_offset));

#undef IS_VALID_OPCODE
#undef IS_LOAD_FUN_PTR
}