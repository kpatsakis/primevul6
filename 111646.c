Status LoadSavedModel(const SessionOptions& session_options,
                      const RunOptions& run_options, const string& export_dir,
                      const std::unordered_set<string>& tags,
                      SavedModelBundleLite* const bundle) {
  SavedModelBundle legacy_bundle;
  SessionOptions rewritten_options(session_options);
  // We disallow calls to Session::Extend() on the returned session, so we can
  // reduce memory consumption by not storing the original GraphDef.
  rewritten_options.config.mutable_experimental()
      ->set_optimize_for_static_graph(true);
  // Disallowing the `RunOptions.output_partition_graphs` option (typically used
  // in debugging and tests) allows us to reduce memory consumption further by
  // not storing the rewritten subgraph for each signature.
  rewritten_options.config.mutable_experimental()
      ->set_disable_output_partition_graphs(true);
  // TODO(mrry): Consider specializing the session creation to reduce peak
  // RAM consumption by using `Session::Create(GraphDef&&)`.
  TF_RETURN_IF_ERROR(LoadSavedModel(rewritten_options, run_options, export_dir,
                                    tags, &legacy_bundle));
  *bundle = SavedModelBundleLite(
      absl::make_unique<LiteSessionWrapper>(std::move(legacy_bundle.session)),
      std::move(*legacy_bundle.meta_graph_def.mutable_signature_def()));
  return Status::OK();
}