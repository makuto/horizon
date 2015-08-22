#ifndef NOISEGENERATION_HPP
#define NOISEGENERATION_HPP
#include "../utilities/simplexnoise.h"
#include "coord.hpp"

//Wrapper funcs for simplex noise
float scaledOctaveNoise3d(float octaves, float persistence, float scale, float x, float y, float z);
#endif
