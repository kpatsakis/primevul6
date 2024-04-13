void ContentSettingsObserver::DidBlockRunInsecureContent(GURL resouce_url) {
  base::ListValue args;
    args.AppendString(resouce_url.spec());

    auto rf = render_frame();
    rf->Send(new AtomViewHostMsg_Message(rf->GetRoutingID(),
        base::UTF8ToUTF16("did-block-run-insecure-content"), args));
}