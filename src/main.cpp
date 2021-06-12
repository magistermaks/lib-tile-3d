
#include "config.hpp"

const int width = 1024;
const int height = 768;

inline void setPixel(int x, int y, byte r, byte g, byte b, byte* img) {
	int i = y * width * 3 + x * 3;
	img[i] = r;
	img[i + 1] = g;
	img[i + 2] = b;
}

struct Ray {
	glm::vec3 orig;
	glm::vec3 invdir;
	int sign[3];
};

bool intersect(const Ray& r, const glm::vec3 bounds[2]) {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
	tmax = (bounds[1 - r.sign[0]].x - r.orig.x) * r.invdir.x;
	tymin = (bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
	tymax = (bounds[1 - r.sign[1]].y - r.orig.y) * r.invdir.y;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
	tzmax = (bounds[1 - r.sign[2]].z - r.orig.z) * r.invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}

inline float vdist(glm::vec3& origin, glm::vec3 bounds[2], int csize) {
	glm::vec3 pivot((float)csize * 0.5f);
	pivot += bounds[0];
	pivot -= origin;
	const float d = pivot.x * pivot.x + pivot.y * pivot.y + pivot.z * pivot.z;
	return d;
}

inline byte test_octree(glm::vec3 bounds[2], int csize, byte* octree, int layerid, Ray& ray, int x, int y, int z, int id, int globalid, float* dist, glm::vec3* origin, byte* mask) {
	byte vid = 255;
	float tmpdist;
	layerid += globalid + id;

	bounds[0].x = x;
	bounds[0].y = y;
	bounds[0].z = z;
	bounds[1].x = csize + x;
	bounds[1].y = csize + y;
	bounds[1].z = csize + z;
	if ((octree[(layerid) * 4 + 3]) > 128 && ((*mask >> id) & 1) == 1)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(*origin, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id;
				*dist = tmpdist;
			}
			//(*mask) |= 1 << id;
		}

	bounds[0].x = csize + x;
	//bounds[0].y = y;
	//bounds[0].z = z;
	bounds[1].x = csize * 2 + x;
	//bounds[1].y = csize * 0.5f + y;
	//bounds[1].z = csize * 0.5f + z;
	if ((octree[(layerid + 1) * 4 + 3]) > 128 && ((*mask >> (id + 1)) & 1) == 1)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(*origin, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id + 1;
				*dist = tmpdist;
			}
			//(*mask) |= 1 << (id + 1);
		}

	bounds[0].x = csize + x;
	//bounds[0].y = y;
	bounds[0].z = csize + z;
	//bounds[1].x = csize + x;
	//bounds[1].y = csize * 0.5f + y;
	bounds[1].z = csize * 2 + z;
	if ((octree[(layerid + 2) * 4 + 3]) > 128 && ((*mask >> (id + 2)) & 1) == 1)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(*origin, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id + 2;
				*dist = tmpdist;
			}
			//(*mask) |= 1 << (id + 2);
		}

	bounds[0].x = x;
	//bounds[0].y = y;
	//bounds[0].z = csize * 0.5f + z;
	bounds[1].x = csize + x;
	//bounds[1].y = csize * 0.5f + y;
	//bounds[1].z = csize + z;
	if ((octree[(layerid + 3) * 4 + 3]) > 128 && ((*mask >> (id + 3)) & 1) == 1)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(*origin, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id + 3;
				*dist = tmpdist;
			}
			//(*mask) |= 1 << (id + 3);
		}

	return vid;
}

struct Data {
	int xo = 0, yo = 0, zo = 0;
	int csize = 64;
	int globalid = 0;
	int layerindex = 1;
	int pow8 = 1;
	byte oc = 0;
	byte mask = 0b11111111;
};

void draw(int width, int height, byte* img, Region* region, glm::vec3* origin, glm::vec3* direction, byte* octree, const int octree_depth, const int _csize) {
	float sx = 1.0f / (float)width;
	float sy = 1.0f / (float)height;

	glm::vec3 bounds[2];
	glm::vec3 dir(1, 1, 1);

	Ray ray;
	ray.orig = *origin;

	glm::vec3 color(0, 0, 0);

	Data* alt_data = new Data[octree_depth + 1];
	//std::vector<Data> alt_data(octree_depth + 1);

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			dir.x = (float)x * sx - 0.5f;
			dir.y = (float)y * sy - 0.5f;
			glm::normalize(dir);
			color.r = 0;  color.g = 0; color.b = 0;

			ray.invdir.x = 1 / dir.x;
			ray.invdir.y = 1 / dir.y;
			ray.invdir.z = 1 / dir.z;
			ray.sign[0] = (ray.invdir.x < 0);
			ray.sign[1] = (ray.invdir.y < 0);
			ray.sign[2] = (ray.invdir.z < 0);

			float dist = 0xffffff;
			byte oc = 255;
			int xo = 0, yo = 0, zo = 0;
			int csize = _csize;
			int globalid = 0;
			int depth = 1;
			int layerindex = 1;
			int pow8 = 1;

			for (int d = 0; d <= octree_depth; d++) {
				alt_data[d].mask = 0b11111111;
			}

			for ( ; depth <= octree_depth; depth++) {
				Data* ad = &(alt_data[depth]);

				ad->globalid = globalid;
				ad->csize = csize;
				ad->layerindex = layerindex;

				dist = 0xffffff;
				csize /= 2;
				globalid = globalid * 8;

				oc = test_octree(bounds, csize, octree, layerindex, ray, xo, yo, zo, 0, globalid, &dist, origin, &(ad->mask));
				byte oc1 = test_octree(bounds, csize, octree, layerindex, ray, xo, yo + csize, zo, 4, globalid, &dist, origin, &(ad->mask));

				ad->pow8 = pow8;
				ad->xo = xo;
				ad->yo = yo;
				ad->zo = zo;

				if (oc1 != 255) oc = oc1;
				globalid += oc;
				if (oc != 255) {
					switch (oc) {
					case 1:
						xo += csize;
						break;

					case 2:
						xo += csize;
						zo += csize;
						break;

					case 3:
						zo += csize;
						break;

					case 4:
						yo += csize;
						break;

					case 5:
						xo += csize;
						yo += csize;
						break;

					case 6:
						xo += csize;
						yo += csize;
						zo += csize;
						break;

					case 7:
						yo += csize;
						zo += csize;
						break;

					default:
						break;
					}
					ad->mask &= ~(1 << oc);
					/*int a = (layerindex + globalid) * 4;
					//if (octree[a + 3] > 128) {
						color.r = octree[a];
						color.g = octree[a + 1];
						color.b = octree[a + 2];
					//}*/

					ad->oc = oc;
					pow8 *= 8;
					layerindex += pow8;				
				}
				else { 
					if(alt_data[1].mask == 0 || depth == 1)
						break;

					ad->mask = 0b11111111;
					ad = &(alt_data[depth - 1]);
					pow8 = ad->pow8;
					layerindex = ad->layerindex;
					globalid = ad->globalid;
					csize = ad->csize;
					xo = ad->xo;
					yo = ad->yo;
					zo = ad->zo;
					depth -= 2;
				}
			}

			const int index = ((1 - pow8) / -7 + globalid) * 4;
			if (depth >= octree_depth + 1 && octree[index + 3] > 128) {
				color.r = octree[index];
				color.g = octree[index + 1];
				color.b = octree[index + 2];
			}
			setPixel(x, y, (byte)color.x, (byte)color.y, (byte)color.z, img);
		}
	}
	//std::cout << "frame\n";
	delete[] alt_data;
}

	//      7-------6   
	//y    /|      /|   
	//^   / |     / |     
	//|  4--|----5  |  
	//|  |  3----|--2    
	//|  | /     | /       
	//|	 0-------1 
	//=====>x

void set_voxel(int x, int y, int z, byte* octree, int octree_depth, byte r, byte g, byte b, int csize) {
	int globalid = 0;
	int xo = 0, yo = 0, zo = 0;
	int layerindex = 1;
	int pow8 = 1;
	for (int depth = 1; depth <= octree_depth; depth++) {
		csize /= 2;
		
		globalid *= 8;

		if (x < xo + csize) {
			if (y < yo + csize) {
				if (z < zo + csize) {
					globalid += 0;
				}
				else {
					globalid += 3;
					zo += csize;
				}
			}
			else {
				yo += csize;
				if(z < zo + csize) {
					globalid += 4;
				}
				else {
					globalid += 7;
					zo += csize;
				}
			}
		}
		else {
			xo += csize;
			if (y < yo + csize) {
				if (z < zo + csize) {
					globalid += 1;					
				}
				else {
					globalid += 2;
					zo += csize;
				}
			}
			else {
				yo += csize;
				if (z < zo + csize) {
					globalid += 5;
				}
				else {
					globalid += 6;
					zo += csize;
				}
			}
		}

		int a = (layerindex + globalid) * 4;
		octree[a + 3] = 255;
		pow8 *= 8;
		layerindex += pow8;

	}
	const int index = ((1 - pow(8, (octree_depth))) / -7 + globalid) * 4;
	octree[index] = r;
	octree[index + 1] = g;
	octree[index + 2] = b;
	octree[index + 3] = 255;
}

int main(void) {

	bool building = false;

	const int octree_depth = 5;
	const int len = (1 - pow(8, (octree_depth + 1))) / -7;
	byte* octree = new byte[ len * 4 ]; //Sn = 1 * (1 - q^n) / (1 - q)

	int index = 0;
	for (int i = 0; i <= octree_depth; i++) {
		int size = std::pow(8, i);
		byte* node = octree + index; //r g b a
		index += size * 4;
		for (int n = 0; n < size * 4; n += 4) {
			node[n] = 255;
			node[n + 1] = 255;
			node[n + 2] = 255;
			node[n + 3] = 0;
			/*if (i < octree_depth - 2)
				node[n + 3] = 255;// randomByte();
			else
				node[n + 3] = 0;*/
		}
	}
	//std::fill_n(octree, len * 4, 0xff);

	const int csize = std::pow(2, octree_depth);
	const int cm = 256 / csize;
	/*for (int x = 0; x < csize; x++)
		for (int y = 0; y < csize; y++)
			for (int z = 0; z < csize; z++)
				set_voxel(x, y, z, octree, octree_depth, x * cm, y * cm, z * cm, csize);*/

	const int c = csize/2;
	
	for (int x = 0; x < csize; x++) {
		for (int y = 0; y < csize; y++) {
			for (int z = 0; z < csize; z++) {

				float A = c - x, B = c - y, C = c - z;
				if((x > c + 1 || x < c - 1) && (y > c + 1 || y < c - 1) && sqrt(A * A + B * B + C * C) < 14) {
					set_voxel(x, y, z, octree, octree_depth, x * cm, y * cm, z * cm, csize);
				}
			}
		}
	}



	// print cwd, nice for debugging
	{
		char temp[CWD_MAX_PATH];
		logger::info("Current working directory: '" + std::string(POSIX_GETCWD(temp, sizeof(temp)) ? temp : "") + "'");
	}

	// initilize GLFW, GLEW, and OpenGL
	if (!GLHelper::init(width, height, "LibTile3D | FPS: 0")) {
		return -1;
	}

	GLFWwindow* window = GLHelper::window();

	logger::info("Generating voxel data...");

	//Voxel* arr1 = Chunk::allocate();
	//Chunk::genBall(arr1, 0, 40);

	// compile GLSL program from the shaders
	GLHelper::ShaderProgram program = GLHelper::loadShaders("layer");

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	Region region;
	Renderer renderer;

	byte* img = new byte[width * height * 3];
	std::memset(img, 0, width * height * 3);

	renderer.addLayer(1).update(img, width, height);

	// get locations from shader program
	GLuint texture_loc = program.location("canvas");

	time_t last = 0;
	long count = 0;

	// enable shader program
	program.bind();

	glm::vec3 origin(0, 0, -1);
	glm::vec3 rot(0, 0, 0);

	Camera camera(CameraMode::fpv, window);

	do {

		glm::mat4 model = glm::mat4(1.0f);

		if (last != time(0)) {
			std::string title = "LibTile3D | FPS: " + std::to_string(count);
			glfwSetWindowTitle(window, title.c_str());
			last = time(0);
			count = 0;
		}


		origin = camera.update(window, &rot);
		draw(width, height, img, &region, &origin, &rot, octree, octree_depth, csize);

		// clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer.render(img, width, height);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		GLHelper::getError();

		count++;

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	// close window
	glfwTerminate();

	return 0;
}


