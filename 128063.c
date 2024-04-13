set_error_literal (GMarkupParseContext  *context,
                   GError              **error,
                   GMarkupError          code,
                   const gchar          *message)
{
  GError *tmp_error;

  tmp_error = g_error_new_literal (G_MARKUP_ERROR, code, message);

  g_prefix_error (&tmp_error,
                  _("Error on line %d char %d: "),
                  context->line_number,
                  context->char_number);

  mark_error (context, tmp_error);

  g_propagate_error (error, tmp_error);
}