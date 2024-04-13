int fuse_interrupted(void)
{
    return fuse_req_interrupted(fuse_get_context_internal()->req);
}