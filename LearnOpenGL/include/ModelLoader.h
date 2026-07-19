#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>

class ModelLoader {
public:

	void loadOBJ(const char* path) {
		std::vector<Point> v;
		std::vector<Point> n;
		std::vector<Point> t;
		std::vector<Index> f;

		std::ifstream file;
		file.open(path);

		if (!file.is_open()) {
			std::cout << "file not open: " << path << std::endl;
			exit(EXIT_FAILURE);
		}

		std::streampos fStart;//signifies the start of the faces indices lines
		bool fStartSaved = false;

		//parse the file into 3 data arrays + 1 index array
		std::string line;
		while (true) {
			if (line.substr(0, 2).compare("v ") == 0) {
				std::istringstream iss{ line.substr(2)};
				float num;
				int i = 0;
				Point p{0.0f, 0.0f, 0.0f};
				while (iss >> num) {
					p.data[i] = num;
					++i;
				}
				v.push_back(p);
			}
			if (line.substr(0, 2).compare("vn") == 0) {
				std::istringstream iss{ line.substr(2) };
				float num;
				int i = 0;
				Point p{ 0.0f, 0.0f, 0.0f };
				while (iss >> num) {
					p.data[i] = num;
					++i;
				}
				n.push_back(p);
			}
			if (line.substr(0, 2).compare("vt") == 0) {
				std::istringstream iss{ line.substr(2) };
				float num;
				int i = 0;
				Point p{ 0.0f, 0.0f, 0.0f };
				while (iss >> num) {
					p.data[i] = num;
					++i;
				}
				t.push_back(p);
			}
			if (line.substr(0, 2).compare("f ") == 0) {
				if (!fStartSaved) fStartSaved = true; //stop overwriting the face starting line
				printf("FACE: %s\n", line.c_str());
				std::istringstream iss{ line.substr(2) };
				int num;
				int i = 0;
				bool notex = false;
				Index p{ 0, 0, 0 };
				while (iss >> num) {
					printf("%i, ", num);
					p.data[i] = num; 
					++i;
					if (iss.peek() == '/') {
						iss.get();
						if (iss.peek() == '/') {
							iss.get();
							printf("%i, ", 0);
							p.data[i] = 0; //skip the texture index, increment the counter
							++i;         
						}
					}
					else { // a set of indices was consumed, reset everything
						f.push_back(p);
						i = 0;
						notex = false;
						printf("\n");
						continue;
					}
				}
				printf("\n");
			}
			if (!fStartSaved) fStart = file.tellg();
			if (!std::getline(file, line)) 
				break;
		}

		printf("VERTICES: \n");
		for (Point p : v) {
			printf("{%g, %g, %g}\n", p.x, p.y, p.z);
		}							   			 
		printf("NORMALS: \n");		   			 
		for (Point p : n) {			   			 
			printf("{%g, %g, %g}\n", p.x, p.y, p.z);
		}							   			 
		printf("TEXTURE: \n");		   			 
		for (Point p : t) {			   			 
			printf("{%g, %g, %g}\n", p.x, p.y, p.z);
		}
		printf("FACE: \n");
		for (Index p : f) {
			printf("{%i, %i, %i}\n", p.v, p.t, p.n);
		}

		std::vector<float> vertices;
		std::vector<unsigned int> indices;

		//populate the vertices array by indexing through the 3 data arrays
		for (int i = 0; i < f.size(); ++i) {

			//always subrtact one from the resulting face index because OBJ uses 1-based indexing 
			//three coordinate vertex
			vertices.push_back(v[f[i].v - 1].x); //good lord the syntax
			vertices.push_back(v[f[i].v - 1].y);
			vertices.push_back(v[f[i].v - 1].z);

			//three coordinate normal
			vertices.push_back(n[f[i].n - 1].x); 
			vertices.push_back(n[f[i].n - 1].y);
			vertices.push_back(n[f[i].n - 1].z);

			//two coordinate texture
			if (f[i].t != 0) {
				vertices.push_back(t[f[i].t - 1].x);
				vertices.push_back(t[f[i].t - 1].y);
			}
		}
		
		//starting from the face indices data, dynamically triangulate the faces based on the vertex count (triangle fan)
		file.clear();
		file.seekg(fStart);
		int fi = 0; //current face index
		while (std::getline(file, line)) {
			if (line.substr(0, 2).compare("f ") == 0) {
				std::istringstream iss{ line.substr(2) };
				std::string word;
				int n = 0;
				//count the number of vertices per face
				while (iss >> word) {
					std::cout << word <<std::endl;
					++n;
				}
				if (n < 3) {
					std::cout << "face contains invalid number of vertices";
					exit(EXIT_FAILURE);
				}
				for (int i = 0; i < n-2; ++i) {
					indices.push_back(fi);
					indices.push_back(fi+i+1);
					indices.push_back(fi+i+2);
				}
				fi += n;
			}
		}


		printf("VERTICES: (%i points)\n", vertices.size()/8);
		for (int i = 0; i < vertices.size() / 8; ++i) {
			printf("{");
			for (int j = 0; j < 8; ++j) {
				printf("%g, ", vertices[i*8 + j]);
			}
			printf("\b\b}\n");
		}

		printf("INDICES: (%i triangles)\n", indices.size() / 3);
		for (int i = 0; i < indices.size() / 3; ++i) {
			printf("{");
			for (int j = 0; j < 3; ++j) {
				printf("%i, ", indices[i * 3 + j]);
			}
			printf("\b\b}\n");
		}
		
	}

private:
	union Point {
		struct {
			float x, y, z;
		};
		float data[3];
	};

	union Index {
		struct {
			int v, t, n;
		};
		int data[3];
	};
};