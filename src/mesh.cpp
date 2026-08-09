#include "mesh.h"


#include <string>
#include <fstream>
#include <sstream>
#include <color.h>

// TODO: Create implementation for mesh.h functions


Mesh load_obj_file_data(const char* filename) {
    Mesh mesh;

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

                Face face;
                int i = 0;
                while (iss >> str && i < 3) {
                    int v = 0.f, t = 0.f, n = 0.f;
                    int result = sscanf(str.c_str(), "%d/%d/%d", &v, &t, &n);
                    if (result != 0) {
                        if (i == 0) {
                            face.a = v - 1;
                            face.a_t = t - 1;
                        } else if (i == 1) {
                            face.b = v - 1;
                            face.b_t = t - 1;
                        } else {
                            face.c = v - 1;
                            face.c_t = t - 1;
                        }
                        if (face.color == 0) {   
                            face.color = C2::Color::Silver;
                        }
                    }
                    i++;
                }
                
                mesh.faces.push_back(face);
            }
        }
    }

    return mesh;
}
