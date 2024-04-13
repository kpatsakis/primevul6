gimp_channel_real_invert (GimpChannel *channel,
                          gboolean     push_undo)
{
  GimpDrawable *drawable = GIMP_DRAWABLE (channel);

  if (push_undo)
    gimp_channel_push_undo (channel,
                            GIMP_CHANNEL_GET_CLASS (channel)->invert_desc);

  if (channel->bounds_known && channel->empty)
    {
      gimp_channel_all (channel, FALSE);
    }
  else
    {
      gimp_gegl_apply_invert_linear (gimp_drawable_get_buffer (drawable),
                                     NULL, NULL,
                                     gimp_drawable_get_buffer (drawable));

      gimp_drawable_update (GIMP_DRAWABLE (channel), 0, 0, -1, -1);
    }
}