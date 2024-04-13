php_apache_sapi_get_stat(TSRMLS_D)
{
	php_struct *ctx = SG(server_context);

	ctx->finfo.st_uid = ctx->r->finfo.user;
	ctx->finfo.st_gid = ctx->r->finfo.group;
	ctx->finfo.st_dev = ctx->r->finfo.device;
	ctx->finfo.st_ino = ctx->r->finfo.inode;
#if defined(NETWARE) && defined(CLIB_STAT_PATCH)
	ctx->finfo.st_atime.tv_sec = apr_time_sec(ctx->r->finfo.atime);
	ctx->finfo.st_mtime.tv_sec = apr_time_sec(ctx->r->finfo.mtime);
	ctx->finfo.st_ctime.tv_sec = apr_time_sec(ctx->r->finfo.ctime);
#else
	ctx->finfo.st_atime = apr_time_sec(ctx->r->finfo.atime);
	ctx->finfo.st_mtime = apr_time_sec(ctx->r->finfo.mtime);
	ctx->finfo.st_ctime = apr_time_sec(ctx->r->finfo.ctime);
#endif

	ctx->finfo.st_size = ctx->r->finfo.size;
	ctx->finfo.st_nlink = ctx->r->finfo.nlink;

	return &ctx->finfo;
}