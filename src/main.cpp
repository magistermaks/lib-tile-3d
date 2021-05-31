
#include "config.hpp"

const int width = 1024;
const int height = 768;
const int ray_max = 128;

inline void setPixel(int x, int y, byte r, byte g, byte b, byte* img) {
	int i = y * width * 3 + x * 3;
	img[i] = r;
	img[i + 1] = g;
	img[i + 2] = b;
}

class Box3 {
public:
	Box3(const glm::vec3& vmin, const glm::vec3& vmax, const glm::vec3& _color) {
		bounds[0] = vmin;
		bounds[1] = vmax;
		color = _color;
	}
	glm::vec3 bounds[2];
	glm::vec3 color;
};

class Ray {
public:
	Ray(const glm::vec3& orig, const glm::vec3& dir) : orig(orig), dir(dir) {
		invdir.x = 1 / dir.x;
		invdir.y = 1 / dir.y;
		invdir.z = 1 / dir.z;
		sign[0] = (invdir.x < 0);
		sign[1] = (invdir.y < 0);
		sign[2] = (invdir.z < 0);
	}

	void update(const glm::vec3& dir) {
		invdir.x = 1 / dir.x;
		invdir.y = 1 / dir.y;
		invdir.z = 1 / dir.z;
		sign[0] = (invdir.x < 0);
		sign[1] = (invdir.y < 0);
		sign[2] = (invdir.z < 0);
	}
	glm::vec3 orig, dir;
	glm::vec3 invdir;
	int sign[3];
};

bool intersect(const Ray& r, const Box3& box) {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (box.bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
	tmax = (box.bounds[1 - r.sign[0]].x - r.orig.x) * r.invdir.x;
	tymin = (box.bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
	tymax = (box.bounds[1 - r.sign[1]].y - r.orig.y) * r.invdir.y;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (box.bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
	tzmax = (box.bounds[1 - r.sign[2]].z - r.orig.z) * r.invdir.z;

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

inline float vdist(glm::vec3& origin, Box3& box, int csize) {
	const glm::vec3 pivot((float)csize * 0.5f);
	float d = glm::distance(origin, box.bounds[0] + pivot);
	return d;
}

inline byte test_octree(Box3& box, int csize, std::vector<Node*>& octree, int depth, Ray& ray, int x, int y, int z, byte id, int globalid, float* dist, glm::vec3* origin) {
	byte vid = 255;
	float tmpdist;
	box.bounds[0].x = x;
	box.bounds[0].y = y;
	box.bounds[0].z = z;
	box.bounds[1].x = csize + x;
	box.bounds[1].y = csize + y;
	box.bounds[1].z = csize + z;
	if ((octree[depth][globalid + id]).a > 128)
		if (intersect(ray, box)) {
			tmpdist = vdist(*origin, box, csize);
			if (*dist >= tmpdist) {
				vid = id;
				*dist = tmpdist;
			}
		}

	box.bounds[0].x = csize + x;
	//box.bounds[0].y = y;
	//box.bounds[0].z = z;
	box.bounds[1].x = csize * 2 + x;
	//box.bounds[1].y = csize * 0.5f + y;
	//box.bounds[1].z = csize * 0.5f + z;
	if ((octree[depth][globalid + 1 + id]).a > 128)
		if (intersect(ray, box)) {
			tmpdist = vdist(*origin, box, csize);
			if (*dist >= tmpdist) {
				vid = id + 1;
				*dist = tmpdist;
			}
		}

	box.bounds[0].x = csize + x;
	//box.bounds[0].y = y;
	box.bounds[0].z = csize + z;
	//box.bounds[1].x = csize + x;
	//box.bounds[1].y = csize * 0.5f + y;
	box.bounds[1].z = csize * 2 + z;
	if ((octree[depth][globalid + 2 + id]).a > 128)
		if (intersect(ray, box)) {
			tmpdist = vdist(*origin, box, csize);
			if (*dist >= tmpdist) {
				vid = id + 2;
				*dist = tmpdist;
			}
		}

	box.bounds[0].x = x;
	//box.bounds[0].y = y;
	//box.bounds[0].z = csize * 0.5f + z;
	box.bounds[1].x = csize + x;
	//box.bounds[1].y = csize * 0.5f + y;
	//box.bounds[1].z = csize + z;
	if ((octree[depth][globalid + 3 + id]).a > 128)
		if (intersect(ray, box)) {
			tmpdist = vdist(*origin, box, csize);
			if (*dist >= tmpdist) {
				vid = id + 3;
				*dist = tmpdist;
			}
		}

	return vid;
}

void draw(int width, int height, byte* img, Region* region, glm::vec3* origin, glm::vec3* direction, std::vector<Node*>& octree) {
	float sx = 1.0f / (float)width;
	float sy = 1.0f / (float)height;

	int csize = 64;

	glm::vec3 dir(1, 1, 1);
	Ray ray(*origin, dir);
	glm::vec3 color(0, 0, 0);
	Box3 box(glm::vec3(0), glm::vec3(1), glm::vec3(255));
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			dir.x = (float)x * sx - 0.5f;
			dir.y = (float)y * sy - 0.5f;
			glm::normalize(dir);
			color.r = 0;  color.g = 0; color.b = 0;
			ray.update(dir);
			float dist = 0xffffff;
			byte oc = 255;
			//auto it = boxes.s
			int xo = 0, yo = 0, zo = 0;
			csize = 64;
			int globalid = 0;
			int depth = 1;
			for ( ; depth <= 6; depth++) {
				dist = 0xffffff;
				csize /= 2;
				oc = test_octree(box, csize, octree, depth, ray, xo, yo, zo, 0, globalid, &dist, origin);
				byte oc1 = test_octree(box, csize, octree, depth, ray, xo, yo + csize, zo, 4, globalid, &dist, origin);
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

			if (depth >= 7 && octree[6][globalid].a > 128) {
				color.r = octree[6][globalid].r;
				color.g = octree[6][globalid].g;
				color.b = octree[6][globalid].b;
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

void set_voxel(int x, int y, int z, std::vector<Node*>& octree, int octree_depth, byte r, byte g, byte b) {
	/*octree[octree_depth][x].r = r;
	octree[octree_depth][x].g = g;
	octree[octree_depth][x].b = b;*/
	int csize = 64;
	int globalid = 0;
	int xo = 0, yo = 0, zo = 0;
	for (int depth = 1; depth <= 6; depth++) {
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

	std::vector <Box3> boxes;
	bool building = false;

	const int octree_depth = 6;
	std::vector<Node*> octree;
	octree.reserve(octree_depth);

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
		octree.push_back(node);
	}

	for (int x = 0; x < 64; x++)
		for (int y = 0; y < 64; y++)
			for (int z = 0; z < 64; z++)
				set_voxel(x, y, z, octree, octree_depth, x * 4, y * 4, z * 4);

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

	//Box3 b()
	/*for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			for (int z = 0; z < 4; z++) {
				boxes.emplace_back(glm::vec3(x * 2, y * 2, z * 2), glm::vec3(x * 2 + 1, y * 2 + 1, z * 2 + 1), glm::vec3(randomByte(), randomByte(), randomByte()));
			}
		}
	}*/

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
		draw(width, height, img, &region, &origin, &rot, octree);

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


