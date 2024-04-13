inline void AssignIDToDefinition::visit(Reference &ope) {
  if (ope.rule_) {
    for (auto arg : ope.args_) {
      arg->accept(*this);
    }
    ope.rule_->accept(*this);
  }
}