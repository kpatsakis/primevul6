ContentSettingsObserver::ContentSettingsObserver(
    content::RenderFrame* render_frame,
    extensions::Dispatcher* extension_dispatcher,
    bool should_whitelist,
    service_manager::BinderRegistry* registry)
    : content::RenderFrameObserver(render_frame),
      content::RenderFrameObserverTracker<ContentSettingsObserver>(
          render_frame),
#if BUILDFLAG(ENABLE_EXTENSIONS)
      extension_dispatcher_(extension_dispatcher),
#endif
      content_settings_manager_(NULL),
      allow_running_insecure_content_(false),
      is_interstitial_page_(false),
      current_request_id_(0),
      should_whitelist_(should_whitelist) {
  ClearBlockedContentSettings();
  render_frame->GetWebFrame()->SetContentSettingsClient(this);

  content::RenderFrame* main_frame =
      render_frame->GetRenderView()->GetMainRenderFrame();
  // TODO(nasko): The main frame is not guaranteed to be in the same process
  // with this frame with --site-per-process. This code needs to be updated
  // to handle this case. See https://crbug.com/496670.
  if (main_frame && main_frame != render_frame) {
    // Copy all the settings from the main render frame to avoid race conditions
    // when initializing this data. See https://crbug.com/333308.
    ContentSettingsObserver* parent = ContentSettingsObserver::Get(main_frame);
    allow_running_insecure_content_ = parent->allow_running_insecure_content_;
    temporarily_allowed_plugins_ = parent->temporarily_allowed_plugins_;
    is_interstitial_page_ = parent->is_interstitial_page_;
  }
}