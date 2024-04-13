skip_spaces (GMarkupParseContext *context)
{
  do
    {
      if (!xml_isspace (*context->iter))
        return;
    }
  while (advance_char (context));
}