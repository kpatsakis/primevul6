static GF_Node *lsr_read_scene_content_model(GF_LASeRCodec *lsr, SVG_Element *parent)
{
	GF_Node *n;
	u32 ntype;
	GF_LSR_READ_INT(lsr, ntype, 6, "ch4");
	n = NULL;
	switch (ntype) {
	case LSR_SCENE_CONTENT_MODEL_a:
		n = lsr_read_a(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_animate:
		n = lsr_read_animate(lsr, parent, 0);
		break;
	case LSR_SCENE_CONTENT_MODEL_animateColor:
		n = lsr_read_animate(lsr, parent, 1);
		break;
	case LSR_SCENE_CONTENT_MODEL_animateMotion:
		n = lsr_read_animateMotion(lsr, parent);
		break;
	case LSR_SCENE_CONTENT_MODEL_animateTransform:
		n = lsr_read_animateTransform(lsr, parent);
		break;
	case LSR_SCENE_CONTENT_MODEL_audio:
		n = lsr_read_audio(lsr, parent);
		break;
	case LSR_SCENE_CONTENT_MODEL_circle:
		n = lsr_read_circle(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_conditional:
		n = lsr_read_conditional(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_cursorManager:
		n = lsr_read_cursorManager(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_defs:
		n = lsr_read_defs(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_desc:
		n = lsr_read_data(lsr, TAG_SVG_desc);
		break;
	case LSR_SCENE_CONTENT_MODEL_ellipse:
		n = lsr_read_ellipse(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_foreignObject:
		n = lsr_read_foreignObject(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_g:
		n = lsr_read_g(lsr, 0);
		break;
	case LSR_SCENE_CONTENT_MODEL_image:
		n = lsr_read_image(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_line:
		n = lsr_read_line(lsr, 0);
		break;
	case LSR_SCENE_CONTENT_MODEL_linearGradient:
		n = lsr_read_linearGradient(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_metadata:
		n = lsr_read_data(lsr, TAG_SVG_metadata);
		break;
	case LSR_SCENE_CONTENT_MODEL_mpath:
		n = lsr_read_mpath(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_path:
		n = lsr_read_path(lsr, 0);
		break;
	case LSR_SCENE_CONTENT_MODEL_polygon:
		n = lsr_read_polygon(lsr, 0, 0);
		break;
	case LSR_SCENE_CONTENT_MODEL_polyline:
		n = lsr_read_polygon(lsr, 1, 0);
		break;
	case LSR_SCENE_CONTENT_MODEL_radialGradient:
		n = lsr_read_radialGradient(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_rect:
		n = lsr_read_rect(lsr, 0);
		break;
	case LSR_SCENE_CONTENT_MODEL_rectClip:
		n = lsr_read_rectClip(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_sameg:
		n = lsr_read_g(lsr, 1);
		break;
	case LSR_SCENE_CONTENT_MODEL_sameline:
		n = lsr_read_line(lsr, 1);
		break;
	case LSR_SCENE_CONTENT_MODEL_samepath:
		n = lsr_read_path(lsr, 1);
		break;
	case LSR_SCENE_CONTENT_MODEL_samepathfill:
		n = lsr_read_path(lsr, 2);
		break;
	case LSR_SCENE_CONTENT_MODEL_samepolygon:
		n = lsr_read_polygon(lsr, 0, 1);
		break;
	case LSR_SCENE_CONTENT_MODEL_samepolygonfill:
		n = lsr_read_polygon(lsr, 0, 2);
		break;
	case LSR_SCENE_CONTENT_MODEL_samepolygonstroke:
		n = lsr_read_polygon(lsr, 0, 3);
		break;
	case LSR_SCENE_CONTENT_MODEL_samepolyline:
		n = lsr_read_polygon(lsr, 1, 1);
		break;
	case LSR_SCENE_CONTENT_MODEL_samepolylinefill:
		n = lsr_read_polygon(lsr, 1, 2);
		break;
	case LSR_SCENE_CONTENT_MODEL_samepolylinestroke:
		n = lsr_read_polygon(lsr, 1, 3);
		break;
	case LSR_SCENE_CONTENT_MODEL_samerect:
		n = lsr_read_rect(lsr, 1);
		break;
	case LSR_SCENE_CONTENT_MODEL_samerectfill:
		n = lsr_read_rect(lsr, 2);
		break;
	case LSR_SCENE_CONTENT_MODEL_sametext:
		n = lsr_read_text(lsr, 1);
		break;
	case LSR_SCENE_CONTENT_MODEL_sametextfill:
		n = lsr_read_text(lsr, 2);
		break;
	case LSR_SCENE_CONTENT_MODEL_sameuse:
		n = lsr_read_use(lsr, 1);
		break;
	case LSR_SCENE_CONTENT_MODEL_script:
		n = lsr_read_script(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_selector:
		n = lsr_read_selector(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_set:
		n = lsr_read_set(lsr, parent);
		break;
	case LSR_SCENE_CONTENT_MODEL_simpleLayout:
		n = lsr_read_simpleLayout(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_stop:
		n = lsr_read_stop(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_switch:
		n = lsr_read_switch(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_text:
		n = lsr_read_text(lsr, 0);
		break;
	case LSR_SCENE_CONTENT_MODEL_title:
		n = lsr_read_data(lsr, TAG_SVG_title);
		break;
	case LSR_SCENE_CONTENT_MODEL_tspan:
		n = lsr_read_tspan(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_use:
		n = lsr_read_use(lsr, 0);
		break;
	case LSR_SCENE_CONTENT_MODEL_video:
		n = lsr_read_video(lsr, parent);
		break;
	case LSR_SCENE_CONTENT_MODEL_listener:
		n = lsr_read_listener(lsr, parent);
		break;
	case LSR_SCENE_CONTENT_MODEL_element_any:
		lsr_read_extend_class(lsr, NULL, 0, "node");
		break;
	case LSR_SCENE_CONTENT_MODEL_privateContainer:
		lsr_read_private_element_container(lsr);
		break;
	case LSR_SCENE_CONTENT_MODEL_textContent:
		lsr_read_text_content(lsr, (GF_Node*)parent);
		break;
	default:
		break;
	}
	if (n && n->sgprivate->interact && n->sgprivate->interact->dom_evt) {
		GF_DOM_Event evt;
		memset(&evt, 0, sizeof(GF_DOM_Event));
		evt.type = GF_EVENT_LOAD;
		gf_dom_event_fire(n, &evt);
	}
	return n;
}