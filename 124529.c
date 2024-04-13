gimp_channel_real_flood (GimpChannel *channel,
                         gboolean     push_undo)
{
  gint x, y, width, height;

  if (! gimp_item_bounds (GIMP_ITEM (channel), &x, &y, &width, &height))
    return;

  if (gimp_channel_is_empty (channel))
    return;

  if (push_undo)
    gimp_channel_push_undo (channel,
                            GIMP_CHANNEL_GET_CLASS (channel)->flood_desc);

  gimp_gegl_apply_flood (gimp_drawable_get_buffer (GIMP_DRAWABLE (channel)),
                         NULL, NULL,
                         gimp_drawable_get_buffer (GIMP_DRAWABLE (channel)),
                         GEGL_RECTANGLE (x, y, width, height));

  gimp_drawable_update (GIMP_DRAWABLE (channel), x, y, width, height);
}