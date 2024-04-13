gimp_channel_get_tree (GimpItem *item)
{
  if (gimp_item_is_attached (item))
    {
      GimpImage *image = gimp_item_get_image (item);

      return gimp_image_get_channel_tree (image);
    }

  return NULL;
}