adds_filesystem_access (GHashTable *old, GHashTable *new)
{
  FlatpakFilesystemMode old_host_mode = GPOINTER_TO_INT (g_hash_table_lookup (old, "host"));

  GLNX_HASH_TABLE_FOREACH_KV (new, const char *, location, gpointer, _new_mode)
    {
      FlatpakFilesystemMode new_mode = GPOINTER_TO_INT (_new_mode);
      FlatpakFilesystemMode old_mode = GPOINTER_TO_INT (g_hash_table_lookup (old, location));

      /* Allow more limited access to the same thing */
      if (new_mode <= old_mode)
        continue;

      /* Allow more limited access if we used to have access to everything */
     if (new_mode <= old_host_mode)
        continue;

     /* For the remainder we have to be pessimistic, for instance even
        if we have home access we can't allow adding access to ~/foo,
        because foo might be a symlink outside home which didn't work
        before but would work with an explicit access to that
        particular file. */

      return TRUE;
    }

  return FALSE;
}