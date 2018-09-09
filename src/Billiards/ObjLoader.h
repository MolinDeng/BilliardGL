#include <vector>
#include <string>
#include "glm/glm.hpp"

struct Material {
	std::string mname;

	glm::vec3 ambient;	//ambient
	glm::vec3 diffuse;	//diffuse
	glm::vec3 specular; //specular
	glm::vec3 emission;
	int	nShininess;		//weight of specular
	std::string texture; // name of texture
};

struct Vertex {
	//triangles
	glm::vec3 position;
	glm::vec3 texture;
	glm::vec3 normal;
	int type; // type == 0 triangles; type == 1 polygons
	Vertex(glm::vec3 p, glm::vec3 t, glm::vec3 n, int tp)
		: position(p), texture(t), normal(n), type(tp) {}
};

class ObjLoader {
public:
	ObjLoader(std::string filename);
	~ObjLoader(){}

	std::vector<Vertex> vertices; //vertices of face,including position, texture, mormal
	std::vector<Material*> materials; //materials
	std::vector<int> mtlIndex;//index of matirials

private:

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> textures;
	std::vector<glm::vec3> normals;

	void parseObj(std::vector<std::string> parameters);
	void parseMtl(std::string filename);
	int getMtl(std::string mtlName);
	std::vector<std::string> splitString(std::string line, std::string delim);
};

