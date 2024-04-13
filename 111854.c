flatpak_context_take_filesystem (FlatpakContext        *context,
                                 char                  *fs,
                                 FlatpakFilesystemMode  mode)
{
  g_hash_table_insert (context->filesystems, fs, GINT_TO_POINTER (mode));
}