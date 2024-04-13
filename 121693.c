flatpak_remote_state_lookup_sparse_cache (FlatpakRemoteState *self,
                                          const char         *ref,
                                          VarMetadataRef     *out_metadata,
                                          GError            **error)
{
  VarSummaryRef summary;
  VarMetadataRef meta;
  VarVariantRef sparse_cache_v;
  guint32 summary_version;
  GVariant *summary_v;

  if (!flatpak_remote_state_ensure_summary (self, error))
    return FALSE;

  summary_v = get_summary_for_ref (self, ref);
  if (summary_v == NULL)
    return flatpak_fail_error (error, FLATPAK_ERROR_REF_NOT_FOUND,
                             _("No entry for %s in remote summary flatpak sparse cache "), ref);

  summary = var_summary_from_gvariant (summary_v);
  meta = var_summary_get_metadata (summary);

  summary_version = GUINT32_FROM_LE (var_metadata_lookup_uint32 (meta, "xa.summary-version", 0));

  if (summary_version == 0)
    {
      if (var_metadata_lookup (meta, "xa.sparse-cache", NULL, &sparse_cache_v))
        {
          VarSparseCacheRef sparse_cache = var_sparse_cache_from_variant (sparse_cache_v);
          if (var_sparse_cache_lookup (sparse_cache, ref, NULL, out_metadata))
            return TRUE;
        }
    }
  else if (summary_version == 1)
    {
      VarRefMapRef ref_map = var_summary_get_ref_map (summary);
      VarRefInfoRef info;

      if (flatpak_var_ref_map_lookup_ref (ref_map, ref, &info))
        {
          *out_metadata = var_ref_info_get_metadata (info);
          return TRUE;
        }
    }
  else
    {
      flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Unsupported summary version %d for remote %s"),
                          summary_version, self->remote_name);
      return FALSE;
    }

  return flatpak_fail_error (error, FLATPAK_ERROR_REF_NOT_FOUND,
                             _("No entry for %s in remote summary flatpak sparse cache "), ref);
}