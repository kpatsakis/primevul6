gimp_channel_real_sharpen (GimpChannel *channel,
                           gboolean     push_undo)
{
  GimpDrawable *drawable = GIMP_DRAWABLE (channel);

  if (push_undo)
    gimp_channel_push_undo (channel,
                            GIMP_CHANNEL_GET_CLASS (channel)->sharpen_desc);

  gimp_gegl_apply_threshold (gimp_drawable_get_buffer (drawable),
                             NULL, NULL,
                             gimp_drawable_get_buffer (drawable),
                             0.5);

  gimp_drawable_update (GIMP_DRAWABLE (channel), 0, 0, -1, -1);
}