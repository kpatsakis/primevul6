static void _xmlrpc_startElement(void *userData, const char *name, const char **attrs)
{
   xml_element *c;
   xml_elem_data* mydata = (xml_elem_data*)userData;
   const char** p = attrs;

   if(mydata) {
      c = mydata->current;

      mydata->current = xml_elem_new();
      mydata->current->name = (char*)strdup(name);
      mydata->current->parent = c;

      /* init attrs */
      while(p && *p) {
         xml_element_attr* attr = malloc(sizeof(xml_element_attr));
         if(attr) {
            attr->key = strdup(*p);
            attr->val = strdup(*(p+1));
            Q_PushTail(&mydata->current->attrs, attr);

            p += 2;
         }
      }
   }
}