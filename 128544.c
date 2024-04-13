void xml_elem_free(xml_element* root) {
   if(root) {
      xml_element* kids = Q_Head(&root->children);
      while(kids) {
         xml_elem_free(kids);
         kids = Q_Next(&root->children);
      }
      xml_elem_free_non_recurse(root);
   }
}