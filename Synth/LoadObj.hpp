#pragma once
#include <string>
#include "OBJ_Loader.h"
#include "Quadric.hpp"
#include "Utility.hpp"
#include "Tracer.hpp"

void LoadObjFile(std::string filename, vector<Quadric>& quadrics);