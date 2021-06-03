
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

struct Node {
	byte r = 0xff, g = 0xff, b = 0xff, a = 0xff;
};

inline float vdist(glm::vec3& origin, glm::vec3 bounds[2], int csize) {
	glm::vec3 pivot((float)csize * 0.5f);
	pivot += bounds[0];
	pivot -= origin;
	const float d = pivot.x * pivot.x + pivot.y * pivot.y + pivot.z * pivot.z;
	return d;
}

inline byte test_octree(glm::vec3 bounds[2], int csize, Node** octree, int depth, Ray& ray, int x, int y, int z, byte id, int globalid, float* dist, glm::vec3* origin) {
	byte vid = 255;
	float tmpdist;
	bounds[0].x = x;
	bounds[0].y = y;
	bounds[0].z = z;
	bounds[1].x = csize + x;
	bounds[1].y = csize + y;
	bounds[1].z = csize + z;
	if ((octree[depth][globalid + id]).a > 128)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(*origin, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id;
				*dist = tmpdist;
			}
		}

	bounds[0].x = csize + x;
	//bounds[0].y = y;
	//bounds[0].z = z;
	bounds[1].x = csize * 2 + x;
	//bounds[1].y = csize * 0.5f + y;
	//bounds[1].z = csize * 0.5f + z;
	if ((octree[depth][globalid + 1 + id]).a > 128)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(*origin, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id + 1;
				*dist = tmpdist;
			}
		}

	bounds[0].x = csize + x;
	//bounds[0].y = y;
	bounds[0].z = csize + z;
	//bounds[1].x = csize + x;
	//bounds[1].y = csize * 0.5f + y;
	bounds[1].z = csize * 2 + z;
	if ((octree[depth][globalid + 2 + id]).a > 128)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(*origin, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id + 2;
				*dist = tmpdist;
			}
		}

	bounds[0].x = x;
	//bounds[0].y = y;
	//bounds[0].z = csize * 0.5f + z;
	bounds[1].x = csize + x;
	//bounds[1].y = csize * 0.5f + y;
	//bounds[1].z = csize + z;
	if ((octree[depth][globalid + 3 + id]).a > 128)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(*origin, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id + 3;
				*dist = tmpdist;
			}
		}

	return vid;
}

void draw(int width, int height, byte* img, Region* region, glm::vec3* origin, glm::vec3* direction, Node** octree, int octree_depth, const int _csize) {
	float sx = 1.0f / (float)width;
	float sy = 1.0f / (float)height;

	glm::vec3 bounds[2];
	glm::vec3 dir(1, 1, 1);

	Ray ray;
	ray.orig = *origin;
	ray.invdir.x = 1 / dir.x;
	ray.invdir.y = 1 / dir.y;
	ray.invdir.z = 1 / dir.z;
	ray.sign[0] = (ray.invdir.x < 0);
	ray.sign[1] = (ray.invdir.y < 0);
	ray.sign[2] = (ray.invdir.z < 0);

	glm::vec3 color(0, 0, 0);

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
			for ( ; depth <= octree_depth; depth++) {
				dist = 0xffffff;
				csize /= 2;
				oc = test_octree(bounds, csize, octree, depth, ray, xo, yo, zo, 0, globalid, &dist, origin);
				byte oc1 = test_octree(bounds, csize, octree, depth, ray, xo, yo + csize, zo, 4, globalid, &dist, origin);
				if (oc1 != 255) oc = oc1;

				
				if (oc != 255) {
					globalid = globalid * 8 + oc;
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

					
					
				}
				else break;
			}

			if (depth >= octree_depth + 1 && octree[octree_depth][globalid].a > 128) {
				color.r = octree[octree_depth][globalid].r;
				color.g = octree[octree_depth][globalid].g;
				color.b = octree[octree_depth][globalid].b;
			}
			setPixel(x, y, (byte)color.x, (byte)color.y, (byte)color.z, img);
		}
	}
}

	//      7-------6   
	//y    /|      /|   
	//^   / |     / |     
	//|  4--|----5  |  
	//|  |  3----|--2    
	//|  | /     | /       
	//|	 0-------1 
	//=====>x

void set_voxel(int x, int y, int z, Node** octree, int octree_depth, byte r, byte g, byte b, int csize) {
	int globalid = 0;
	int xo = 0, yo = 0, zo = 0;
	for (int depth = 1; depth <= octree_depth; depth++) {
		csize /= 2;
		octree[depth - 1][globalid].a = 255;
		if (x < xo + csize) {
			if (y < yo + csize) {
				if (z < zo + csize) {
					globalid = globalid * 8 + 0;
				}
				else {
					globalid = globalid * 8 + 3;
					zo += csize;
				}
			}
			else {
				yo += csize;
				if(z < zo + csize) {
					globalid = globalid * 8 + 4;
				}
				else {
					globalid = globalid * 8 + 7;
					zo += csize;
				}
			}
		}
		else {
			xo += csize;
			if (y < yo + csize) {
				if (z < zo + csize) {
					globalid = globalid * 8 + 1;					
				}
				else {
					globalid = globalid * 8 + 2;
					zo += csize;
				}
			}
			else {
				yo += csize;
				if (z < zo + csize) {
					globalid = globalid * 8 + 5;
				}
				else {
					globalid = globalid * 8 + 6;
					zo += csize;
				}
			}
		}

	}
	octree[octree_depth][globalid].r = r;
	octree[octree_depth][globalid].g = g;
	octree[octree_depth][globalid].b = b;
	octree[octree_depth][globalid].a = 255;
}

int main(void) {

	//std::vector <Box3> boxes;
	bool building = false;

	const int octree_depth = 4;
	Node** octree = new Node*[octree_depth + 1];

	for (int i = 0; i <= octree_depth; i++) {
		int size = std::pow(8, i);
		Node* node = new Node[size];
		for (int n = 0; n < size; n++) {
			node[n].r = 0;//randomByte();
			node[n].g = 10;//randomByte();
			node[n].b = 0;//randomByte();
			//if(i == octree_depth)
			node[n].a = 255;// randomByte();
		}
		octree[i] = node;
	}

	const int csize = std::pow(2, octree_depth);
	const int cm = 256 / csize;
	for (int x = 0; x < csize; x++)
		for (int y = 0; y < csize; y++)
			for (int z = 0; z < csize; z++)
				set_voxel(x, y, z, octree, octree_depth, x * cm, y * cm, z * cm, csize);

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

	Voxel* arr1 = Chunk::allocate();
	Chunk::genBall(arr1, 0, 40);

	// compile GLSL program from the shaders
	GLHelper::ShaderProgram program = GLHelper::loadShaders("layer");

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	Region region;
	Renderer renderer;

	byte* img = new byte[width * height * 3];
	std::memset(img, 0, width * height * 3);

	renderer.addLayer(1).update(img, width, height);

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 1; y++) {
			for (int z = 0; z < 8; z++) {
				region.put(arr1, x, y, z);
			}
		}
	}

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


