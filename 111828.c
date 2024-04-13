flatpak_context_shared_to_string (FlatpakContextShares shares, FlatpakContextShares valid)
{
  return flatpak_context_bitmask_to_string (shares, valid, flatpak_context_shares);
}