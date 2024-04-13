gimp_channel_to_selection (GimpItem       *item,
                           GimpChannelOps  op,
                           gboolean        antialias,
                           gboolean        feather,
                           gdouble         feather_radius_x,
                           gdouble         feather_radius_y)
{
  GimpChannel *channel = GIMP_CHANNEL (item);
  GimpImage   *image   = gimp_item_get_image (item);
  gint         off_x, off_y;

  gimp_item_get_offset (item, &off_x, &off_y);

  gimp_channel_select_channel (gimp_image_get_mask (image),
                               GIMP_ITEM_GET_CLASS (item)->to_selection_desc,
                               channel, off_x, off_y,
                               op,
                               feather, feather_radius_x, feather_radius_x);
}