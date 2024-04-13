inline any Holder::reduce(SemanticValues &sv, any &dt) const {
  if (outer_->action && !outer_->disable_action) {
    return outer_->action(sv, dt);
  } else if (sv.empty()) {
    return any();
  } else {
    return std::move(sv.front());
  }
}