flatpak_related_free (FlatpakRelated *self)
{
  flatpak_decomposed_unref (self->ref);
  g_free (self->commit);
  g_strfreev (self->subpaths);
  g_free (self);
}