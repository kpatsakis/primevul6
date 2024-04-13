Status LoadSavedModelInternal(const SessionOptions& session_options,
                              const RunOptions& run_options,
                              const string& export_dir,
                              const std::unordered_set<string>& tags,
                              SavedModelBundle* const bundle) {
  TF_RETURN_IF_ERROR(ReadMetaGraphDefFromSavedModel(export_dir, tags,
                                                    &bundle->meta_graph_def));
  TF_RETURN_IF_ERROR(
      ReadSavedModelDebugInfoIfPresent(export_dir, &bundle->debug_info));
  TF_RETURN_IF_ERROR(LoadMetagraphIntoSession(
      session_options, bundle->meta_graph_def, &bundle->session));
  TF_RETURN_IF_ERROR(RestoreSession(run_options, bundle->meta_graph_def,
                                    export_dir, &bundle->session));
  return Status::OK();
}