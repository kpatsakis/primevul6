bool IsScriptDisabledForPreview(const content::RenderFrame* render_frame) {
  return render_frame->GetPreviewsState() & content::NOSCRIPT_ON;
}