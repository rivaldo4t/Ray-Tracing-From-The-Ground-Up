#pragma once
#include <iostream>
#include <GL/glut.h>
#include <vector>
#include <string>
#include <math.h>
#include <algorithm>

#include "cyPoint.h"
#include "cyCore.h"
#include "Quadric.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "Camera.hpp"
#include "Utility.hpp"
#include "Tracer.hpp"
using namespace std;

Image Null_image;
Image Tex_env("textures/AdobeStock_space_3.2.jpeg");
//Image Tex_plane("textures/AdobeStock_stone_2.jpeg");
//Image Tex_plane_2("textures/tess3_sq.png");
Image Tex_sphere("textures/moon.jpg");
//Image Tex_sphere_2("textures/AdobeStock_space_2.2.jpeg");
//Image Tex_sphere_3("textures/nebula.jpg");
//Image Tex_sphere_4("textures/jupiter.jpg");
//Image Norm_sphere("textures/map.jpg");
//Image Norm_plane("textures/stone.jpg");
//Image camera_painting("textures/abs9_960x1080.jpg");

double rotX = 0.0, rotY = 0.0;
const int Xmax = 512, Ymax = 512;
vector<float> frameBuffer(Ymax * Xmax * 3, 0.0f);
AreaLight areaLight({ 0, 10, -2 }, { 1, 1, 1 }, { 0, -1, 0 }, { 0, 0, 1 });
vector<AreaLight> areaLights = { areaLight };
double animParam = 0, animParam2 = 0;
double cp_x = 0, cp_y = 0, cp_z = 0, cp_r, cp_g, cp_b;

void renderScene();
void update();
void keyRot(int key, int x, int y);
void quit(unsigned char key, int x, int y);