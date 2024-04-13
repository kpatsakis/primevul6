flatpak_context_allows_features (FlatpakContext        *context,
                                 FlatpakContextFeatures features)
{
  return (context->features & features) == features;
}