flatpak_context_features_to_string (FlatpakContextFeatures features, FlatpakContextFeatures valid)
{
  return flatpak_context_bitmask_to_string (features, valid, flatpak_context_features);
}