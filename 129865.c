verify_stack_type_compatibility (VerifyContext *ctx, MonoType *type, ILStackDesc *stack)
{
	return verify_stack_type_compatibility_full (ctx, type, stack, FALSE, FALSE);
}