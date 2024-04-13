bool TensorSliceReader::HasTensor(const string& name, TensorShape* shape,
                                  DataType* type) const {
  mutex_lock l(mu_);
  const TensorSliceSet* tss = gtl::FindPtrOrNull(tensors_, name);
  if (!tss && !all_shards_loaded_) {
    VLOG(1) << "Did not find tensor in preferred shard, loading all shards: "
            << name;
    LoadAllShards();
    tss = gtl::FindPtrOrNull(tensors_, name);
  }
  if (tss) {
    if (shape) {
      *shape = tss->shape();
    }
    if (type) {
      *type = tss->type();
    }
    return true;
  } else {
    return false;
  }
}