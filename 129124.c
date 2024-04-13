void auditsc_get_stamp(struct audit_context *ctx,
		       struct timespec *t, unsigned int *serial)
{
	if (!ctx->serial)
		ctx->serial = audit_serial();
	t->tv_sec  = ctx->ctime.tv_sec;
	t->tv_nsec = ctx->ctime.tv_nsec;
	*serial    = ctx->serial;
	ctx->auditable = 1;
}