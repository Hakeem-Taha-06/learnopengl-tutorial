#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>



class ModelLoader {
public:

	static void loadOBJ(const char* path) {
		std::vector<Point> v;
		std::vector<Point> n;
		std::vector<Point> t;
		std::vector<Index> f;

		std::ifstream file;
		file.open(path);

		if (file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {
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
			}
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