uid_t audit_get_loginuid(struct audit_context *ctx)
{
	return ctx ? ctx->loginuid : -1;
}