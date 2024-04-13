char* xml_elem_serialize_to_string(xml_element *el, XML_ELEM_OUTPUT_OPTIONS options, int *buf_len)
{
   simplestring buf;
   simplestring_init(&buf);

   xml_element_serialize(el, simplestring_out_fptr, (void *)&buf, options, 0);

   if(buf_len) {
      *buf_len = buf.len;
   }

   return buf.str;
}