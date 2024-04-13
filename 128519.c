void xml_elem_free_non_recurse(xml_element* root) {
   if(root) {
      xml_element_attr* attrs = Q_Head(&root->attrs);
      while(attrs) {
         my_free(attrs->key);
         my_free(attrs->val);
         my_free(attrs);
         attrs = Q_Next(&root->attrs);
      }

      Q_Destroy(&root->children);
      Q_Destroy(&root->attrs);
      if(root->name) {
          free((char *)root->name);
          root->name = NULL;
      }
      simplestring_free(&root->text);
      my_free(root);
   }
}