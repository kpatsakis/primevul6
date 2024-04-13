do_cmp_op (VerifyContext *ctx, const unsigned char table [TYPE_MAX][TYPE_MAX], guint32 opcode)
{
	ILStackDesc *a, *b;
	int idxa, idxb;
	unsigned char res;

	if (!check_underflow (ctx, 2))
		return;
	b = stack_pop (ctx);
	a = stack_pop (ctx);

	if (opcode == CEE_CGT_UN) {
		if (stack_slot_get_type (a) == TYPE_COMPLEX && stack_slot_get_type (b) == TYPE_COMPLEX) {
			stack_push_val (ctx, TYPE_I4, &mono_defaults.int32_class->byval_arg);
			return;
		}
	}

	idxa = stack_slot_get_underlying_type (a);
	if (stack_slot_is_managed_pointer (a))
		idxa = TYPE_PTR;

	idxb = stack_slot_get_underlying_type (b);
	if (stack_slot_is_managed_pointer (b)) 
		idxb = TYPE_PTR;

	if (stack_slot_is_complex_type_not_reference_type (a) || stack_slot_is_complex_type_not_reference_type (b)) {
		res = TYPE_INV;
	} else {
		--idxa;
		--idxb;
		res = table [idxa][idxb];
	}

	if(res == TYPE_INV) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf("Compare instruction applyed to ill formed stack (%s x %s) at 0x%04x", stack_slot_get_name (a), stack_slot_get_name (b), ctx->ip_offset));
	} else if (res & NON_VERIFIABLE_RESULT) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Compare instruction is not verifiable (%s x %s) at 0x%04x", stack_slot_get_name (a), stack_slot_get_name (b), ctx->ip_offset)); 
 		res = res & ~NON_VERIFIABLE_RESULT;
 	}
 	stack_push_val (ctx, TYPE_I4, &mono_defaults.int32_class->byval_arg);
}