gimp_channel_get_description (GimpViewable  *viewable,
                              gchar        **tooltip)
{
  if (! strcmp (GIMP_IMAGE_QUICK_MASK_NAME,
                gimp_object_get_name (viewable)))
    {
      return g_strdup (_("Quick Mask"));
    }

  return GIMP_VIEWABLE_CLASS (parent_class)->get_description (viewable,
                                                              tooltip);
}