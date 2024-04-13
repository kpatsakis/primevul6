gimp_channel_stroke (GimpItem           *item,
                     GimpDrawable       *drawable,
                     GimpStrokeOptions  *stroke_options,
                     gboolean            push_undo,
                     GimpProgress       *progress,
                     GError            **error)
{
  GimpChannel        *channel = GIMP_CHANNEL (item);
  const GimpBoundSeg *segs_in;
  const GimpBoundSeg *segs_out;
  gint                n_segs_in;
  gint                n_segs_out;
  gboolean            retval = FALSE;
  gint                offset_x, offset_y;

  if (! gimp_channel_boundary (channel, &segs_in, &segs_out,
                               &n_segs_in, &n_segs_out,
                               0, 0, 0, 0))
    {
      g_set_error_literal (error, GIMP_ERROR, GIMP_FAILED,
                           _("Cannot stroke empty channel."));
      return FALSE;
    }

  gimp_item_get_offset (item, &offset_x, &offset_y);

  switch (gimp_stroke_options_get_method (stroke_options))
    {
    case GIMP_STROKE_LINE:
      gimp_drawable_stroke_boundary (drawable,
                                     stroke_options,
                                     segs_in, n_segs_in,
                                     offset_x, offset_y,
                                     push_undo);
      retval = TRUE;
      break;

    case GIMP_STROKE_PAINT_METHOD:
      {
        GimpPaintInfo    *paint_info;
        GimpPaintCore    *core;
        GimpPaintOptions *paint_options;
        gboolean          emulate_dynamics;

        paint_info = gimp_context_get_paint_info (GIMP_CONTEXT (stroke_options));

        core = g_object_new (paint_info->paint_type, NULL);

        paint_options = gimp_stroke_options_get_paint_options (stroke_options);
        emulate_dynamics = gimp_stroke_options_get_emulate_dynamics (stroke_options);

        retval = gimp_paint_core_stroke_boundary (core, drawable,
                                                  paint_options,
                                                  emulate_dynamics,
                                                  segs_in, n_segs_in,
                                                  offset_x, offset_y,
                                                  push_undo, error);

        g_object_unref (core);
      }
      break;

    default:
      g_return_val_if_reached (FALSE);
    }

  return retval;
}