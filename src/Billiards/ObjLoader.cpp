#define FREEGLUT_STATIC
#include "ObjLoader.h"
#include <fstream>
#include <iostream>
#include "GL/freeglut.h"

using namespace std;

ObjLoader::ObjLoader(string filename)
{
	fstream f;
	f.open(filename, ios::in);
	if (!f.is_open()) {
		cout << "Open obj file error!" << endl;
		return;
	}

	string line;
	while (!f.eof()) {
		//get a line in obj file
		getline(f, line);
		//split and get parameters
		vector<string> parameters = splitString(line, " ");
		//parse
		if (parameters.size() > 0)
			parseObj(parameters);
	}
	f.close();
}

vector<string> ObjLoader::splitString(std::string line, std::string delim) {
	vector<string>parameters;
	size_t pos1 = 0;
	size_t pos2 = line.find(delim);
	while (string::npos != pos2) {
		parameters.push_back(line.substr(pos1, pos2 - pos1));
		pos1 = pos2 + delim.size();
		pos2 = line.find(delim, pos1);
	}
	if (pos1 != line.length())
		parameters.push_back(line.substr(pos1));
	return parameters;
}

void ObjLoader::parseObj(vector<string> parameters) {
	if (parameters[0] == "v") {
		glm::vec3 pos(stof(parameters[1]), 
			stof(parameters[2]), stof(parameters[3]));
		positions.push_back(pos);
	}
	else if (parameters[0] == "vn") {
		glm::vec3 norm(stof(parameters[1]),
			stof(parameters[2]), stof(parameters[3]));
		normals.push_back(norm);
	}
	else if (parameters[0] == "vt") {
		glm::vec3 texture(stof(parameters[1]),
			stof(parameters[2]), 0);
		textures.push_back(texture);
	}
	else if (parameters[0] == "g") {
		
	}
	else if (parameters[0] == "mtllib") {
		parseMtl(parameters[1]);
	}
	else if (parameters[0] == "usemtl") {
		int index = getMtl(parameters[1]);
		mtlIndex.push_back(vertices.size());
		mtlIndex.push_back(index);
	}
	else if (parameters[0] == "f") {
		
		if (parameters.size() == 5) {
			for (int i = 1; i < 4; i++) {
				vector<string> data = splitString(parameters[i], "/");
				if (data[1] != "") {
					vertices.push_back(Vertex(positions[stoi(data[0]) - 1],
						textures[stoi(data[1]) - 1], normals[stoi(data[2]) - 1], 1));
				}
				else {
					vertices.push_back(Vertex(positions[stoi(data[0]) - 1],
						glm::vec3(-1, -1, -1), normals[stoi(data[2]) - 1], 1));
				}
			}
			for (int i = 1; i < 5; i++) {
				if (i == 2) continue;
				vector<string> data = splitString(parameters[i], "/");
				if (data[1] != "") {
					vertices.push_back(Vertex(positions[stoi(data[0]) - 1],
						textures[stoi(data[1]) - 1], normals[stoi(data[2]) - 1], 1));
				}
				else {
					vertices.push_back(Vertex(positions[stoi(data[0]) - 1],
						glm::vec3(-1, -1, -1), normals[stoi(data[2]) - 1], 1));
				}
			}
		}
		else if (parameters.size() == 4) {
			for (int i = 1; i < 4; i++) {
				vector<string> data = splitString(parameters[i], "/");
				if (data[1] != "")
					vertices.push_back(Vertex(positions[stoi(data[0]) - 1],
						textures[stoi(data[1]) - 1], normals[stoi(data[2]) - 1], 0));
				else {
					vertices.push_back(Vertex(positions[stoi(data[0]) - 1],
						glm::vec3(-1, -1, -1), normals[stoi(data[2]) - 1], 0));
				}
			}
		}
		
	}
}

void ObjLoader::parseMtl(string filename) {
	fstream f;
	f.open(".//obj//" + filename, ios::in);
	if (!f.is_open()) {
		cout << "Open mtl file error!" << endl;
	}

	string line;
	Material *pM = NULL;
	while (!f.eof()) {
		//get a line in obj file
		getline(f, line);
		if (line.size() == 0) continue;
		//split and get parameters
		vector<string> parameters = splitString(line, " ");
		//parse
		if (parameters[0] == "Ka") {
			pM->ambient = glm::vec3(stof(parameters[1]), stof(parameters[2]), 
				stof(parameters[3]));
		}
		else if (parameters[0] == "Kd") {
			pM->diffuse = glm::vec3(stof(parameters[1]), stof(parameters[2]), 
				stof(parameters[3]));
		}
		else if (parameters[0] == "Ks") {
			pM->specular = glm::vec3(stof(parameters[1]), stof(parameters[2]), 
				stof(parameters[3]));
		}
		else if (parameters[0] == "Ke") {
			pM->emission = glm::vec3(stof(parameters[1]), stof(parameters[2]),
				stof(parameters[3]));
		}
		else if (parameters[0] == "Ns") {
			pM->nShininess = stoi(parameters[1]);
		}
		else if (parameters[0] == "map_Ka") {
			pM->texture = parameters[1];
		}
		else if (parameters[0] == "newmtl") {
			if (pM != NULL) {
				materials.push_back(pM);
			}
			pM = new Material();
			pM->mname = parameters[1];
		}
	}
	if (pM != NULL) materials.push_back(pM);
	f.close();
}

int ObjLoader::getMtl(std::string mtlName) {
	for (int i = 0; i < (int)materials.size(); i++) {
		if (mtlName == materials[i]->mname)
			return i;
	}
	return -1;
}