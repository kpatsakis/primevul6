getProperty(Property prop)
{
	switch(prop)
	{
	case SWF_SETPROPERTY_X: 	return("_x"); break;
	case SWF_SETPROPERTY_Y:
	case PROPERTY_Y:		return("_y"); break;
	case PROPERTY_XMOUSE:		return("_xMouse"); break;
	case PROPERTY_YMOUSE:		return("_yMouse"); break;
	case SWF_SETPROPERTY_XSCALE:
	case PROPERTY_XSCALE:	   	return("_xScale"); break;
	case SWF_SETPROPERTY_YSCALE:
	case PROPERTY_YSCALE:	   	return("_yScale"); break;
	case PROPERTY_CURRENTFRAME:	return("_currentFrame"); break;
	case PROPERTY_TOTALFRAMES:	return("_totalFrames"); break;
	case SWF_SETPROPERTY_ALPHA:
	case PROPERTY_ALPHA:		return("_alpha"); break;
	case SWF_SETPROPERTY_VISIBILITY:
	case PROPERTY_VISIBLE:		return("_visible"); break;
	case PROPERTY_WIDTH:		return("_width"); break;
	case PROPERTY_HEIGHT:		return("_height"); break;
	case SWF_SETPROPERTY_ROTATION:
	case PROPERTY_ROTATION:		return("_rotation"); break;
	case PROPERTY_TARGET:		return("_target"); break;
	case PROPERTY_FRAMESLOADED:	return("_framesLoaded"); break;
	case SWF_SETPROPERTY_NAME:
	case PROPERTY_NAME:		return("_name"); break;
	case PROPERTY_DROPTARGET:	return("_dropTarget"); break;
	case PROPERTY_URL:		return("_url"); break;
	case SWF_SETPROPERTY_HIGHQUALITY:
	case PROPERTY_HIGHQUALITY:	return("_quality"); break;
	case SWF_SETPROPERTY_SHOWFOCUSRECT:
	case PROPERTY_FOCUSRECT:	return("_focusRect"); break;
	case SWF_SETPROPERTY_SOUNDBUFFERTIME:
	case PROPERTY_SOUNDBUFTIME:	return("_soundBufTime"); break;
	case SWF_SETPROPERTY_WTHIT:
	case PROPERTY_WTHIT:		return("_WTHIT!?"); break;
	default:			return("unknown property!"); break;
	}
}