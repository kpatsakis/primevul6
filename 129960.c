stack_push_val (VerifyContext *ctx, int stype, MonoType *type)
{
	ILStackDesc *top = stack_push (ctx);
	top->stype = stype;
	top->type = type;
	return top;
}