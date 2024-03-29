#pragma once

#include "cgmath.h"			// slee's simple math library
#include "cgut2.h"

mesh2* load_model(const std::string& path, bool bFlipUV = false, int floor = 0, float size = 1.0f);
Texture* load_path_texture(const std::string& path);
void delete_texture_cache(void);