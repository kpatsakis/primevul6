static void _xmlrpc_endElement(void *userData, const char *name)
{
   xml_elem_data* mydata = (xml_elem_data*)userData;

   if(mydata && mydata->current && mydata->current->parent) {
      Q_PushTail(&mydata->current->parent->children, mydata->current);

      mydata->current = mydata->current->parent;
   }
}