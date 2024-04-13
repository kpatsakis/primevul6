static void _xmlrpc_charHandler(void *userData,
                        const char *s,
                        int len)
{
   xml_elem_data* mydata = (xml_elem_data*)userData;
   if(mydata && mydata->current) {

      /* Check if we need to decode utf-8 parser output to another encoding */
      if(mydata->needs_enc_conversion && mydata->input_options->encoding) {
         int new_len = 0;
         char* add_text = utf8_decode(s, len, &new_len, mydata->input_options->encoding);
         if(add_text) {
            len = new_len;
            simplestring_addn(&mydata->current->text, add_text, len);
            free(add_text);
            return;
         }
      }
      simplestring_addn(&mydata->current->text, s, len);
   }
}