static int fastrpc_create_maps(struct fastrpc_invoke_ctx *ctx)
{
	struct device *dev = ctx->fl->sctx->dev;
	int i, err;

	for (i = 0; i < ctx->nscalars; ++i) {
		/* Make sure reserved field is set to 0 */
		if (ctx->args[i].reserved)
			return -EINVAL;

		if (ctx->args[i].fd == 0 || ctx->args[i].fd == -1 ||
		    ctx->args[i].length == 0)
			continue;

		err = fastrpc_map_create(ctx->fl, ctx->args[i].fd,
					 ctx->args[i].length, &ctx->maps[i]);
		if (err) {
			dev_err(dev, "Error Creating map %d\n", err);
			return -EINVAL;
		}

	}
	return 0;
}