flatpak_context_adds_permissions (FlatpakContext *old,
                                  FlatpakContext *new)
{
  /* We allow upgrade to multiarch, that is really not a huge problem.
   * Similarly, having sensible semantics for /dev/shm is
   * not a security concern. */
  guint32 harmless_features = (FLATPAK_CONTEXT_FEATURE_MULTIARCH |
                               FLATPAK_CONTEXT_FEATURE_PER_APP_DEV_SHM);
  guint32 old_sockets;

  if (adds_flags (old->shares & old->shares_valid,
                  new->shares & new->shares_valid))
    return TRUE;

  old_sockets = old->sockets & old->sockets_valid;

  /* If we used to allow X11, also allow new fallback X11,
     as that is actually less permissions */
  if (old_sockets & FLATPAK_CONTEXT_SOCKET_X11)
    old_sockets |= FLATPAK_CONTEXT_SOCKET_FALLBACK_X11;

  if (adds_flags (old_sockets,
                  new->sockets & new->sockets_valid))
    return TRUE;

  if (adds_flags (old->devices & old->devices_valid,
                  new->devices & new->devices_valid))
    return TRUE;

  if (adds_flags ((old->features & old->features_valid) | harmless_features,
                  new->features & new->features_valid))
    return TRUE;

  if (adds_bus_policy (old->session_bus_policy, new->session_bus_policy))
    return TRUE;

  if (adds_bus_policy (old->system_bus_policy, new->system_bus_policy))
    return TRUE;

  if (adds_generic_policy (old->generic_policy, new->generic_policy))
    return TRUE;

  if (adds_filesystem_access (old->filesystems, new->filesystems))
    return TRUE;

  return FALSE;
}