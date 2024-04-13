bool ContentSettingsObserver::AllowWriteToClipboard(bool default_value) {
  bool allowed = default_value;
#if BUILDFLAG(ENABLE_EXTENSIONS)
  // All blessed extension pages could historically write to the clipboard, so
  // preserve that for compatibility.
  extensions::ScriptContext* current_context =
      extension_dispatcher_->script_context_set().GetCurrent();
  if (current_context) {
    if (current_context->effective_context_type() ==
        extensions::Feature::BLESSED_EXTENSION_CONTEXT) {
      allowed = true;
    } else {
      allowed |= current_context->HasAPIPermission(
          extensions::APIPermission::kClipboardWrite);
    }
  }
#endif
  return allowed;
}