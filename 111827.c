flatpak_context_get_run_flags (FlatpakContext *context)
{
  FlatpakRunFlags flags = 0;

  if (flatpak_context_allows_features (context, FLATPAK_CONTEXT_FEATURE_DEVEL))
    flags |= FLATPAK_RUN_FLAG_DEVEL;

  if (flatpak_context_allows_features (context, FLATPAK_CONTEXT_FEATURE_MULTIARCH))
    flags |= FLATPAK_RUN_FLAG_MULTIARCH;

  if (flatpak_context_allows_features (context, FLATPAK_CONTEXT_FEATURE_BLUETOOTH))
    flags |= FLATPAK_RUN_FLAG_BLUETOOTH;

  if (flatpak_context_allows_features (context, FLATPAK_CONTEXT_FEATURE_CANBUS))
    flags |= FLATPAK_RUN_FLAG_CANBUS;

  return flags;
}