Status LoadMetagraphIntoSession(const SessionOptions& session_options,
                                const MetaGraphDef& meta_graph,
                                std::unique_ptr<Session>* session) {
  Session* session_p = nullptr;
  TF_RETURN_IF_ERROR(NewSession(session_options, &session_p));
  session->reset(session_p);
  TF_RETURN_IF_ERROR(ValidateSavedTensors(meta_graph.graph_def()));
  return (*session)->Create(meta_graph.graph_def());
}