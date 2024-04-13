void TensorSliceReader::LoadAllShards() const {
  VLOG(1) << "Loading all shards for " << filepattern_;
  for (size_t i = 0; i < fnames_.size() && status_.ok(); ++i) {
    LoadShard(i);
  }
  all_shards_loaded_ = true;
}