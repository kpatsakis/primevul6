advance_to_name_end (GMarkupParseContext *context)
{
  do
    {
      if (IS_COMMON_NAME_END_CHAR (*(context->iter)))
        return;
      if (xml_isspace (*(context->iter)))
        return;
    }
  while (advance_char (context));
}