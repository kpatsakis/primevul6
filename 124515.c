gimp_channel_get_active_components (GimpDrawable *drawable,
                                    gboolean     *active)
{
  /*  Make sure that the alpha channel is not valid.  */
  active[GRAY]    = TRUE;
  active[ALPHA_G] = FALSE;
}