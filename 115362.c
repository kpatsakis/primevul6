inline void DetectLeftRecursion::visit(Reference &ope) {
  if (ope.name_ == name_) {
    error_s = ope.s_;
  } else if (!refs_.count(ope.name_)) {
    refs_.insert(ope.name_);
    if (ope.rule_) {
      ope.rule_->accept(*this);
      if (done_ == false) { return; }
    }
  }
  done_ = true;
}