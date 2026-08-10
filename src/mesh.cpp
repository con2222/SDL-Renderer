#include "mesh.h"


#include <string>
#include <fstream>
#include <sstream>
#include <color.h>


Mesh load_obj_file_data(const char* filename, 
        geom::vec3 translation, 
        geom::vec3 rotation,
        geom::vec3 scale
) 
{
    Mesh mesh;
    mesh.translation = translation;
    mesh.rotation = rotation;
    mesh.scale = scale;

    std::ifstream in(filename);
    std::string line;
    std::string digits = "-0123456789";

    if (in.is_open()) {
        while(std::getline(in, line)) {
            if (line[0] == '#') { continue; }
            if (line[0] == 'v' && line[1] == ' ') {
                long unsigned int index = line.find_first_of(digits);
                
                if (index != std::string::npos) {
                    line = line.substr(index); // take v/vn/vt line
                }
                std::istringstream coords(line);

                float x = 0.f, y = 0.f, z = 0.f;
                coords >> x >> y >> z;
                mesh.vertices.push_back({x, y, z}); 
            } else if (line.substr(0, 2) == "vt") {
                long unsigned int index = line.find_first_of(digits);
                if (index != std::string::npos) {
                    line = line.substr(index);
                }
                std::istringstream coords(line);
                float xt = 0.f, yt = 0.f;
                coords >> xt >> yt;
                mesh.tex_coords.emplace_back(xt, yt);
            } else if (line[0] == 'f' && line[1] == ' ') {
                long unsigned int index = line.find_first_of(digits);
                
                if (index != std::string::npos) {
                    line = line.substr(index);
                }
                
                std::istringstream iss(line);
                std::string str;

                struct VertexIndex { int v = 0, t = 0, n = 0; };
                std::vector<VertexIndex> parsed_vertices;

                while (iss >> str) {
                    VertexIndex vi;
                    int result = sscanf(str.c_str(), "%d/%d/%d", &vi.v, &vi.t, &vi.n);
                    if (result != 0) {
                        parsed_vertices.push_back(vi);
                    }
                }

                if (parsed_vertices.size() >= 3) { 
                    for (size_t j = 1; j < parsed_vertices.size() - 1; j++) {
                        Face face;
                    
                        face.a = parsed_vertices[0].v - 1;
                        face.a_t = parsed_vertices[0].t - 1;

                        face.b = parsed_vertices[j].v - 1;
                        face.b_t = parsed_vertices[j].t - 1;

                        face.c = parsed_vertices[j + 1].v - 1;
                        face.c_t = parsed_vertices[j + 1].t - 1;
                        face.color = C2::Color::Silver;
                        mesh.faces.push_back(face);
                    }

                }
            }
        }
    }

    return mesh;
}
