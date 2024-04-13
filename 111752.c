void InferenceContext::Relax(DimensionHandle d_old, DimensionHandle d_new,
                             DimensionHandle* out) {
  if (d_old.SameHandle(d_new)) {
    *out = d_old;
  } else if (!ValueKnown(d_old) && !ValueKnown(d_new)) {
    // The node will be fed by the dimension d_new instead of d_old: any
    // equality assertion between d_old and other input dimension on this node
    // may not be true anymore, so forget them all.
    ForgetMerges();
    // Return the new shape handle to force the relaxation to propagate to the
    // fanout of the context.
    *out = d_new;
  } else if (!ValueKnown(d_new)) {
    ForgetMerges();
    *out = d_new;
  } else if (Value(d_old) == Value(d_new)) {
    // Return the old shape handle. This will stop the relaxation in the fanout
    // of the context.
    *out = d_old;
  } else {
    // Return a new handle that encodes a different unknown dim.
    ForgetMerges();
    *out = UnknownDim();
  }
}