get_summary_for_ref (FlatpakRemoteState *self,
                     const char *ref)
{
  GVariant *summary = NULL;

  if (self->index != NULL)
    {
      g_autofree char * arch = flatpak_get_arch_for_ref (ref);

      if (arch != NULL)
        summary = g_hash_table_lookup (self->subsummaries, arch);

      if (summary == NULL)
        {
          const char *non_compat_arch = flatpak_get_compat_arch_reverse (arch);

          if (non_compat_arch != NULL)
            summary = g_hash_table_lookup (self->subsummaries, non_compat_arch);
        }
    }
  else
    summary = self->summary;

  return summary;
}