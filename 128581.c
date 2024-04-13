void xml_elem_serialize_to_stream(xml_element *el, FILE *output, XML_ELEM_OUTPUT_OPTIONS options)
{
   xml_element_serialize(el, file_out_fptr, (void *)output, options, 0);
}