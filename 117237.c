static void fuse_put_module(struct fuse_module *m)
{
    pthread_mutex_lock(&fuse_context_lock);
    assert(m->ctr > 0);
    m->ctr--;
    if (!m->ctr && m->so) {
        struct fusemod_so *so = m->so;
        assert(so->ctr > 0);
        so->ctr--;
        if (!so->ctr) {
            struct fuse_module **mp;
            for (mp = &fuse_modules; *mp;) {
                if ((*mp)->so == so)
                    *mp = (*mp)->next;
                else
                    mp = &(*mp)->next;
            }
            dlclose(so->handle);
            free(so);
        }
    }
    pthread_mutex_unlock(&fuse_context_lock);
}