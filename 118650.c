DxfData::DxfData(double fn, double fs, double fa, 
								 const std::string &filename, const std::string &layername, 
								 double xorigin, double yorigin, double scale)
{
	std::ifstream stream(filename.c_str());
	if (!stream.good()) {
		LOG(message_group::Warning,Location::NONE,"","Can't open DXF file '%1$s'.",filename);
		return;
	}

	Grid2d<std::vector<int>> grid(GRID_COARSE);
	std::vector<Line> lines;                       // Global lines
	std::unordered_map<std::string, std::vector<Line>> blockdata; // Lines in blocks

	auto in_entities_section = false;
	auto in_blocks_section = false;
	std::string current_block;

#define ADD_LINE(_x1, _y1, _x2, _y2) do {										\
		double _p1x = _x1, _p1y = _y1, _p2x = _x2, _p2y = _y2;  \
		if (!in_entities_section && !in_blocks_section)         \
			break;                                                \
		if (in_entities_section &&                              \
				!(layername.empty() || layername == layer))         \
			break;                                                \
		grid.align(_p1x, _p1y);                                 \
		grid.align(_p2x, _p2y);                                 \
		grid.data(_p1x, _p1y).push_back(lines.size());          \
		grid.data(_p2x, _p2y).push_back(lines.size());          \
		if (in_entities_section)                                \
			lines.emplace_back(                                   \
			  addPoint(_p1x, _p1y), addPoint(_p2x, _p2y));        \
		if (in_blocks_section && !current_block.empty())        \
			blockdata[current_block].emplace_back(	              \
				addPoint(_p1x, _p1y), addPoint(_p2x, _p2y));      	\
	} while (0)

	std::string mode, layer, name, iddata;
	int dimtype = 0;
	double coords[7][2]; // Used by DIMENSION entities
	std::vector<double> xverts;
	std::vector<double> yverts;
	double radius = 0;
	double arc_start_angle = 0, arc_stop_angle = 0;
	double ellipse_start_angle = 0, ellipse_stop_angle = 0;

	for (int i = 0; i < 7; ++i) {
		for (int j = 0; j < 2; ++j) {
			coords[i][j] = 0;
		}
	}

	typedef std::unordered_map<std::string, int> EntityList;
	EntityList unsupported_entities_list;

	//
	// Parse DXF file. Will populate this->points, this->dims, lines and blockdata
	//
	while (!stream.eof())	{
		std::string id_str, data;
		std::getline(stream, id_str);
		boost::trim(id_str);
		std::getline(stream, data);
		boost::trim(data);

		int id;
    try {
		  id = boost::lexical_cast<int>(id_str);
    }
    catch (const boost::bad_lexical_cast &blc) {
			if (!stream.eof()) {
				LOG(message_group::Warning,Location::NONE,"","Illegal ID '%1$s' in `%2$s'",id_str,filename);
			}
			break;
  	}
    try {
		if (id >= 10 && id <= 16) {
			if (in_blocks_section) {
				coords[id-10][0] = boost::lexical_cast<double>(data);
			}
			else if (id == 11 || id == 12 || id == 16) {
				coords[id-10][0] = boost::lexical_cast<double>(data) * scale;
			}
			else {
				coords[id-10][0] = (boost::lexical_cast<double>(data) - xorigin) * scale;
			}
		}

		if (id >= 20 && id <= 26) {
			if (in_blocks_section) {
				coords[id-20][1] = boost::lexical_cast<double>(data);
			}
			else if (id == 21 || id == 22 || id == 26) {
				coords[id-20][1] = boost::lexical_cast<double>(data) * scale;
			}
			else {
				coords[id-20][1] = (boost::lexical_cast<double>(data) - yorigin) * scale;
			}
		}

		switch (id) {
		case 0:
			if (mode == "SECTION") {
				in_entities_section = iddata == "ENTITIES";
				in_blocks_section = iddata == "BLOCKS";
			}
			else if (mode == "LINE") {
				ADD_LINE(xverts.at(0), yverts.at(0), xverts.at(1), yverts.at(1));
			}
			else if (mode == "LWPOLYLINE") {
				// assert(xverts.size() == yverts.size());
				// Get maximum to enforce managed exception if xverts.size() != yverts.size()
				int numverts = std::max(xverts.size(), yverts.size());
				for (int i=1; i<numverts; ++i) {
					ADD_LINE(xverts.at(i-1), yverts.at(i-1), xverts.at(i%numverts), yverts.at(i%numverts));
				}
				// polyline flag is stored in 'dimtype'
				if (dimtype & 0x01) { // closed polyline
					ADD_LINE(xverts.at(numverts-1), yverts.at(numverts-1), xverts.at(0), yverts.at(0));
				}
			}
			else if (mode == "CIRCLE") {
				int n = Calc::get_fragments_from_r(radius, fn, fs, fa);
				Vector2d center(xverts.at(0), yverts.at(0));
				for (int i = 0; i < n; ++i) {
					double a1 = (360.0 * i) / n;
					double a2 = (360.0 *(i + 1)) / n;
					ADD_LINE(cos_degrees(a1)*radius + center[0], sin_degrees(a1)*radius + center[1],
					         cos_degrees(a2)*radius + center[0], sin_degrees(a2)*radius + center[1]);
				}
			}
			else if (mode == "ARC") {
				Vector2d center(xverts.at(0), yverts.at(0));
				int n = Calc::get_fragments_from_r(radius, fn, fs, fa);
				while (arc_start_angle > arc_stop_angle) {
					arc_stop_angle += 360.0;
				}
				double arc_angle = arc_stop_angle - arc_start_angle;
				n = static_cast<int>(ceil(n * arc_angle / 360));
				for (int i = 0; i < n; ++i) {
					double a1 = arc_start_angle + arc_angle * i / n;
					double a2 = arc_start_angle + arc_angle * (i + 1) / n;
					ADD_LINE(cos_degrees(a1)*radius + center[0], sin_degrees(a1)*radius + center[1],
					         cos_degrees(a2)*radius + center[0], sin_degrees(a2)*radius + center[1]);
				}
			}
			else if (mode == "ELLIPSE") {
				// Commented code is meant as documentation of vector math
				while (ellipse_start_angle > ellipse_stop_angle) ellipse_stop_angle += 2 * M_PI;
//				Vector2d center(xverts[0], yverts[0]);
				Vector2d center(xverts.at(0), yverts.at(0));
//				Vector2d ce(xverts[1], yverts[1]);
				Vector2d ce(xverts.at(1), yverts.at(1));
//				double r_major = ce.length();
				double r_major = sqrt(ce[0]*ce[0] + ce[1]*ce[1]);
//				double rot_angle = ce.angle();
				double rot_angle;
				{
//					double dot = ce.dot(Vector2d(1.0, 0.0));
					double dot = ce[0];
					double cosval = dot / r_major;
					if (cosval > 1.0) cosval = 1.0;
					if (cosval < -1.0) cosval = -1.0;
					rot_angle = acos(cosval);
					if (ce[1] < 0.0) rot_angle = 2 * M_PI - rot_angle;
				}

				// the ratio stored in 'radius; due to the parser code not checking entity type
				double r_minor = r_major * radius;
				double sweep_angle = ellipse_stop_angle-ellipse_start_angle;
				int n = Calc::get_fragments_from_r(r_major, fn, fs, fa);
				n = static_cast<int>(ceil(n * sweep_angle / (2 * M_PI)));
//				Vector2d p1;
				Vector2d p1{0.0, 0.0};
				for (int i=0; i<=n; ++i) {
					double a = (ellipse_start_angle + sweep_angle*i/n);
//					Vector2d p2(cos(a)*r_major, sin(a)*r_minor);
					Vector2d p2(cos(a)*r_major, sin(a)*r_minor);
//					p2.rotate(rot_angle);
					Vector2d p2_rot(cos(rot_angle)*p2[0] - sin(rot_angle)*p2[1],
					                sin(rot_angle)*p2[0] + cos(rot_angle)*p2[1]);
//					p2 += center;
					p2_rot[0] += center[0];
					p2_rot[1] += center[1];
					if (i > 0) {
// 						ADD_LINE(p1[0], p1[1], p2[0], p2[1]);
						ADD_LINE(p1[0], p1[1], p2_rot[0], p2_rot[1]);
					}
//					p1 = p2;
					p1[0] = p2_rot[0];
					p1[1] = p2_rot[1];
				}
			}
			else if (mode == "INSERT") {
				// scale is stored in ellipse_start|stop_angle, rotation in arc_start_angle;
				// due to the parser code not checking entity type
				int n = blockdata[iddata].size();
				for (int i = 0; i < n; ++i) {
					double a = arc_start_angle;
					double lx1 = this->points[blockdata[iddata][i].idx[0]][0] * ellipse_start_angle;
					double ly1 = this->points[blockdata[iddata][i].idx[0]][1] * ellipse_stop_angle;
					double lx2 = this->points[blockdata[iddata][i].idx[1]][0] * ellipse_start_angle;
					double ly2 = this->points[blockdata[iddata][i].idx[1]][1] * ellipse_stop_angle;
					double px1 = (cos_degrees(a)*lx1 - sin_degrees(a)*ly1) * scale + xverts.at(0);
					double py1 = (sin_degrees(a)*lx1 + cos_degrees(a)*ly1) * scale + yverts.at(0);
					double px2 = (cos_degrees(a)*lx2 - sin_degrees(a)*ly2) * scale + xverts.at(0);
					double py2 = (sin_degrees(a)*lx2 + cos_degrees(a)*ly2) * scale + yverts.at(0);
					ADD_LINE(px1, py1, px2, py2);
				}
			}
			else if (mode == "DIMENSION" &&
							 (layername.empty() || layername == layer)) {
				this->dims.push_back(Dim());
				this->dims.back().type = dimtype;
				for (int i = 0; i < 7; ++i) {
					for (int j = 0; j < 2; ++j) {
						this->dims.back().coords[i][j] = coords[i][j];
					}
				}
				this->dims.back().angle = arc_start_angle;
				this->dims.back().length = radius;
				this->dims.back().name = name;
			}
			else if (mode == "BLOCK") {
				current_block = iddata;
			}
			else if (mode == "ENDBLK") {
				current_block.erase();
			}
			else if (mode == "ENDSEC") {
			}
			else if (in_blocks_section || (in_entities_section &&
																		 (layername.empty() || layername == layer))) {
				unsupported_entities_list[mode]++;
			}
			mode = data;
			layer.erase();
			name.erase();
			iddata.erase();
			dimtype = 0;
			for (int i = 0; i < 7; ++i) {
				for (int j = 0; j < 2; ++j) {
					coords[i][j] = 0;
				}
			}
			xverts.clear();
			yverts.clear();
			radius = arc_start_angle = arc_stop_angle = 0;
			ellipse_start_angle = ellipse_stop_angle = 0;
			if (mode == "INSERT") {
				ellipse_start_angle = ellipse_stop_angle = 1.0; // scale
			}
			break;
		case 1:
			name = data;
			break;
		case 2:
			iddata = data;
			break;
		case 8:
			layer = data;
			break;
		case 10:
			if (in_blocks_section) {
				xverts.push_back((boost::lexical_cast<double>(data)));
			}
			else {
				xverts.push_back((boost::lexical_cast<double>(data) - xorigin) * scale);
			}
			break;
		case 11:
			if (in_blocks_section) {
				xverts.push_back((boost::lexical_cast<double>(data)));
			}
			else {
				xverts.push_back((boost::lexical_cast<double>(data) - xorigin) * scale);
			}
			break;
		case 20:
			if (in_blocks_section) {
				yverts.push_back((boost::lexical_cast<double>(data)));
			}
			else {
				yverts.push_back((boost::lexical_cast<double>(data) - yorigin) * scale);
			}
			break;
		case 21:
			if (in_blocks_section) {
				yverts.push_back((boost::lexical_cast<double>(data)));
			}
			else {
				yverts.push_back((boost::lexical_cast<double>(data) - yorigin) * scale);
			}
			break;
		case 40:
			// CIRCLE, ARC: radius
			// ELLIPSE: minor to major ratio
			// DIMENSION (radial, diameter): Leader length
			radius = boost::lexical_cast<double>(data);
			if (!in_blocks_section) radius *= scale;
			break;
		case 41:
			// ELLIPSE: start_angle
			// INSERT: X scale
			ellipse_start_angle = boost::lexical_cast<double>(data);
			break;
		case 50:
			// ARC: start_angle
			// INSERT: rot angle
      // DIMENSION: linear and rotated: angle
			arc_start_angle = boost::lexical_cast<double>(data);
			break;
		case 42:
			// ELLIPSE: stop_angle
			// INSERT: Y scale
			ellipse_stop_angle = boost::lexical_cast<double>(data);
			break;
		case 51: // ARC
			arc_stop_angle = boost::lexical_cast<double>(data);
			break;
		case 70:
			// LWPOLYLINE: polyline flag
			// DIMENSION: dimension type
			dimtype = boost::lexical_cast<int>(data);
			break;
		}
    }
    catch (boost::bad_lexical_cast &blc) {
		LOG(message_group::Warning,Location::NONE,"","Illegal value '%1$s'in `%2$s'",data,filename);
  	}
    catch (const std::out_of_range& oor) {
		LOG(message_group::Warning,Location::NONE,"","Not enough input values for %1$s. in '%2$s'",data,filename);
  	}
	}

	for (const auto &i : unsupported_entities_list) {
		if (layername.empty()) {
			LOG(message_group::Warning,Location::NONE,"",
				"Unsupported DXF Entity '%1$s' (%2$x) in %3$s.",i.first,i.second,QuotedString(boostfs_uncomplete(filename, fs::current_path()).generic_string()));
		} else {
			LOG(message_group::Warning,Location::NONE,"",
				"Unsupported DXF Entity '%1$s' (%2$x) in layer '%3$s' of %4$s",i.first,i.second,layername,boostfs_uncomplete(filename, fs::current_path()).generic_string());
		}
	}

	// Extract paths from parsed data

	typedef std::map<int, int> LineMap;
	LineMap enabled_lines;
	for (size_t i = 0; i < lines.size(); ++i) {
		enabled_lines[i] = i;
	}

	// extract all open paths
	while (enabled_lines.size() > 0) {
		int current_line, current_point;

		for (const auto &l : enabled_lines) {
			int idx = l.second;
			for (int j = 0; j < 2; ++j) {
				auto lv = grid.data(this->points[lines[idx].idx[j]][0], this->points[lines[idx].idx[j]][1]);
				for (size_t ki = 0; ki < lv.size(); ++ki) {
					int k = lv.at(ki);
                    if (k < 0 || k >= lines.size()) {
                        LOG(message_group::Warning,Location::NONE,"",
                            "Bad DXF line index in %1$s.",QuotedString(boostfs_uncomplete(filename, fs::current_path()).generic_string()));
                        continue;
                    }
					if (k == idx || lines[k].disabled) continue;
					goto next_open_path_j;
				}
				current_line = idx;
				current_point = j;
				goto create_open_path;
			next_open_path_j:;
			}
		}

		break;

	create_open_path:
		this->paths.push_back(Path());
		Path *this_path = &this->paths.back();

		this_path->indices.push_back(lines[current_line].idx[current_point]);
		while (1) {
			this_path->indices.push_back(lines[current_line].idx[!current_point]);
			const auto &ref_point = this->points[lines[current_line].idx[!current_point]];
			lines[current_line].disabled = true;
			enabled_lines.erase(current_line);
			auto lv = grid.data(ref_point[0], ref_point[1]);
			for (size_t ki = 0; ki < lv.size(); ++ki) {
				int k = lv.at(ki);
                if (k < 0 || k >= lines.size()) {
                    LOG(message_group::Warning,Location::NONE,"",
                        "Bad DXF line index in %1$s.",QuotedString(boostfs_uncomplete(filename, fs::current_path()).generic_string()));
                    continue;
                }
				if (lines[k].disabled) continue;
                auto idk0 = lines[k].idx[0];    // make it easier to read and debug
                auto idk1 = lines[k].idx[1];
				if (grid.eq(ref_point[0], ref_point[1], this->points[idk0][0], this->points[idk0][1])) {
					current_line = k;
					current_point = 0;
					goto found_next_line_in_open_path;
				}
				if (grid.eq(ref_point[0], ref_point[1], this->points[idk1][0], this->points[idk1][1])) {
					current_line = k;
					current_point = 1;
					goto found_next_line_in_open_path;
				}
			}
			break;
		found_next_line_in_open_path:;
		}
	}

	// extract all closed paths
	while (enabled_lines.size() > 0) {
		int current_line = enabled_lines.begin()->second;
		int current_point = 0;

		this->paths.push_back(Path());
		auto& this_path = this->paths.back();
		this_path.is_closed = true;
		
		this_path.indices.push_back(lines[current_line].idx[current_point]);
		while (1) {
			this_path.indices.push_back(lines[current_line].idx[!current_point]);
			const auto &ref_point = this->points[lines[current_line].idx[!current_point]];
			lines[current_line].disabled = true;
			enabled_lines.erase(current_line);
			auto lv = grid.data(ref_point[0], ref_point[1]);
			for (size_t ki = 0; ki < lv.size(); ++ki) {
				int k = lv.at(ki);
                if (k < 0 || k >= lines.size()) {
                    LOG(message_group::Warning,Location::NONE,"",
                        "Bad DXF line index in %1$s.",QuotedString(boostfs_uncomplete(filename, fs::current_path()).generic_string()));
                    continue;
                }
				if (lines[k].disabled) continue;
                auto idk0 = lines[k].idx[0];    // make it easier to read and debug
                auto idk1 = lines[k].idx[1];
				if (grid.eq(ref_point[0], ref_point[1], this->points[idk0][0], this->points[idk0][1])) {
					current_line = k;
					current_point = 0;
					goto found_next_line_in_closed_path;
				}
                if (grid.eq(ref_point[0], ref_point[1], this->points[idk1][0], this->points[idk1][1])) {
					current_line = k;
					current_point = 1;
					goto found_next_line_in_closed_path;
				}
			}
			break;
		found_next_line_in_closed_path:;
		}
	}

	fixup_path_direction();

#if 0
	printf("----- DXF Data -----\n");
	for (int i = 0; i < this->paths.size(); ++i) {
		printf("Path %d (%s):\n", i, this->paths[i].is_closed ? "closed" : "open");
		for (int j = 0; j < this->paths[i].points.size(); ++j)
			printf("  %f %f\n", (*this->paths[i].points[j])[0], (*this->paths[i].points[j])[1]);
	}
	printf("--------------------\n");
	fflush(stdout);
#endif
}