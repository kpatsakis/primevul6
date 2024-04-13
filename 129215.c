static inline void audit_zero_context(struct audit_context *context,
				      enum audit_state state)
{
	uid_t loginuid = context->loginuid;

	memset(context, 0, sizeof(*context));
	context->state      = state;
	context->loginuid   = loginuid;
}