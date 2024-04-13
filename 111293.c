TensorSliceReader::TensorSliceReader(const string& filepattern,
                                     OpenTableFunction open_function,
                                     int preferred_shard)
    : filepattern_(filepattern), open_function_(std::move(open_function)) {
  VLOG(1) << "TensorSliceReader for " << filepattern;
  Status s = Env::Default()->GetMatchingPaths(filepattern, &fnames_);
  if (!s.ok()) {
    status_ = errors::InvalidArgument(
        "Unsuccessful TensorSliceReader constructor: "
        "Failed to get matching files on ",
        filepattern, ": ", s.ToString());
    return;
  }
  if (fnames_.empty()) {
    status_ = errors::NotFound(
        "Unsuccessful TensorSliceReader constructor: "
        "Failed to find any matching files for ",
        filepattern);
    return;
  }
  sss_.resize(fnames_.size());
  for (size_t shard = 0; shard < fnames_.size(); ++shard) {
    fname_to_index_.insert(std::make_pair(fnames_[shard], shard));
  }
  if (preferred_shard == kLoadAllShards || fnames_.size() == 1 ||
      static_cast<size_t>(preferred_shard) >= fnames_.size()) {
    LoadAllShards();
  } else {
    VLOG(1) << "Loading shard " << preferred_shard << " for " << filepattern_;
    LoadShard(preferred_shard);
  }
}