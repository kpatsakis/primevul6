inline void AssignIDToDefinition::visit(PrecedenceClimbing &ope) {
  ope.atom_->accept(*this);
  ope.binop_->accept(*this);
}