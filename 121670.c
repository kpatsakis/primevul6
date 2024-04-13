rewrite_mime_xml (xmlDoc *doc)
{
  xmlNode *root_element = xmlDocGetRootElement (doc);
  xmlNode *top_node = NULL;

  for (top_node = root_element; top_node; top_node = top_node->next)
    {
      xmlNode *mime_node = NULL;
      if (top_node->type != XML_ELEMENT_NODE)
        continue;

      if (strcmp ((char *) top_node->name, "mime-info") != 0)
        return FALSE;

      for (mime_node = top_node->children; mime_node; mime_node = mime_node->next)
        {
          xmlNode *sub_node = NULL;
          xmlNode *next_sub_node = NULL;

          xml_autofree xmlChar *mimetype = NULL;
          if (mime_node->type != XML_ELEMENT_NODE)
            continue;

          if (strcmp ((char *) mime_node->name, "mime-type") != 0)
            return FALSE;

          mimetype = xmlGetProp (mime_node, (xmlChar *) "type");
          for (sub_node = mime_node->children; sub_node; sub_node = next_sub_node)
            {
              next_sub_node = sub_node->next;

              if (sub_node->type != XML_ELEMENT_NODE)
                continue;

              if (strcmp ((char *) sub_node->name, "magic") == 0)
                {
                  g_warning ("Removing magic mime rule from exports");
                  xmlUnlinkNode (sub_node);
                  xmlFreeNode (sub_node);
                }
              else if (strcmp ((char *) sub_node->name, "glob") == 0)
                {
                  xmlSetProp (sub_node,
                              (const xmlChar *) "weight",
                              (const xmlChar *) "5");
                }
            }
        }
    }

  return TRUE;
}