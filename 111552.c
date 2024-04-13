static gboolean tb_host_size_iter(gpointer key, gpointer value, gpointer data)
{
    const TranslationBlock *tb = value;
    size_t *size = data;

    *size += tb->tc.size;
    return false;
}