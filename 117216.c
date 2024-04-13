static void fuse_delete_context_key(void)
{
    pthread_mutex_lock(&fuse_context_lock);
    fuse_context_ref--;
    if (!fuse_context_ref) {
        free(pthread_getspecific(fuse_context_key));
        pthread_key_delete(fuse_context_key);
    }
    pthread_mutex_unlock(&fuse_context_lock);
}