  void FlipBooleanAttr(const string& attr_name, NodeDef* node) {
    const bool old_value =
        !node->attr().count(attr_name) ? false : node->attr().at(attr_name).b();
    (*node->mutable_attr())[attr_name].set_b(!old_value);
  }