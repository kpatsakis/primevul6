propagate_error (GMarkupParseContext  *context,
                 GError              **dest,
                 GError               *src)
{
  if (context->flags & G_MARKUP_PREFIX_ERROR_POSITION)
    g_prefix_error (&src,
                    _("Error on line %d char %d: "),
                    context->line_number,
                    context->char_number);

  mark_error (context, src);

  g_propagate_error (dest, src);
}