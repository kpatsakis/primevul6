bool MaybeSavedModelDirectory(const string& export_dir) {
  const string saved_model_pb_path =
      io::JoinPath(export_dir, kSavedModelFilenamePb);
  const string saved_model_pbtxt_path =
      io::JoinPath(export_dir, kSavedModelFilenamePbTxt);
  return Env::Default()->FileExists(saved_model_pb_path).ok() ||
         Env::Default()->FileExists(saved_model_pbtxt_path).ok();
}