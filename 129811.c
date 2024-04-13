stack_init (VerifyContext *ctx, ILCodeDesc *state) 
{
	if (state->flags & IL_CODE_FLAG_STACK_INITED)
		return;
	state->size = state->max_size = 0;
	state->flags |= IL_CODE_FLAG_STACK_INITED;
}