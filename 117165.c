static struct fuse *req_fuse_prepare(fuse_req_t req)
{
    struct fuse_context_i *c = fuse_get_context_internal();
    const struct fuse_ctx *ctx = fuse_req_ctx(req);
    c->req = req;
    c->ctx.fuse = req_fuse(req);
    c->ctx.uid = ctx->uid;
    c->ctx.gid = ctx->gid;
    c->ctx.pid = ctx->pid;
#ifdef POSIXACLS
    c->ctx.umask = ctx->umask;
#endif
    return c->ctx.fuse;
}