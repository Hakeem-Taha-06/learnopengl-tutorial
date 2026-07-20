#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class ModelLoader {
public:

	void loadOBJ(const char* path, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
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
			else if (line.substr(0, 2).compare("vn") == 0) {
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
			else if (line.substr(0, 2).compare("vt") == 0) {
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
			else if (line.substr(0, 2).compare("f ") == 0) {
				if (!fStartSaved) fStartSaved = true; //stop overwriting the face starting line
				std::istringstream iss{ line.substr(2) };
				std::string token; //represents one set of indices for a vertex "v/vt/vn"
				while (iss >> token) {
					std::istringstream iss2{ token };
					std::string index;
					int i = 0;
					Index p{ 0, 0, 0 };
					while (std::getline(iss2, index, '/')) {
						if (!index.empty())
							p.data[i] = std::stoi(index);
						++i;
					}
					f.push_back(p);
				}
			}
			if (!fStartSaved) fStart = file.tellg();
			if (!std::getline(file, line)) 
				break;
		}

		//just in case :D
		vertices.clear();
		indices.clear();

		//populate the vertices array by indexing through the 3 data arrays
		for (int i = 0; i < f.size(); ++i) {

			//always subrtact one from the resulting face index because OBJ uses 1-based indexing 
			//three coordinate vertex
			vertices.push_back(v[f[i].v - 1].x); //good lord the syntax
			vertices.push_back(v[f[i].v - 1].y);
			vertices.push_back(v[f[i].v - 1].z);

			//three coordinate normal
			if (f[i].n != 0) {
				vertices.push_back(n[f[i].n - 1].x);
				vertices.push_back(n[f[i].n - 1].y);
				vertices.push_back(n[f[i].n - 1].z);
			}
			else {
				vertices.push_back(0.0f);vertices.push_back(0.0f);vertices.push_back(0.0f);
			}

			//two coordinate texture
			if (f[i].t != 0) {
				vertices.push_back(t[f[i].t - 1].x);
				vertices.push_back(t[f[i].t - 1].y);
			}
			else {
				vertices.push_back(0.0f);vertices.push_back(0.0f);
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
				while (iss >> word) ++n;
				if (n < 3) {
					std::cout << "face contains invalid number of vertices";
					exit(EXIT_FAILURE);
				}
				//triangle fan
				for (int i = 0; i < n-2; ++i) {
					indices.push_back(fi);
					indices.push_back(fi+i+1);
					indices.push_back(fi+i+2);
				}
				fi += n;
			}
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