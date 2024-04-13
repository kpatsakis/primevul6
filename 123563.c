void ContentSettingsObserver::DidBlockContentType(
    const std::string& settings_type) {
  DidBlockContentType(settings_type,
      blink::WebStringToGURL(render_frame()->GetWebFrame()->
          GetSecurityOrigin().ToString()).spec());
}