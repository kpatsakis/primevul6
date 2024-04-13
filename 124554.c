gimp_channel_buffer_changed (GeglBuffer          *buffer,
                             const GeglRectangle *rect,
                             GimpChannel         *channel)
{
  gimp_drawable_invalidate_boundary (GIMP_DRAWABLE (channel));
}