#include "mesh.h"


#include <string>
#include <fstream>
#include <sstream>
#include <color.h>

// TODO: Create implementation for mesh.h functions

Mesh mesh = {};

geom::vec3 cube_vertices[N_CUBE_VERTICES] = {
    { -1, -1, -1},
    { -1, 1, -1 },
    {  1, 1, -1 },
    { 1, -1, -1 },    
    { 1,  1,  1 },
    { 1, -1,  1 },
    { -1, 1,  1 },
    { -1, -1, 1 },
};

Face cube_faces[N_CUBE_FACES] = {
    // front
    {1, 2, 3, C2::Color::Red},
    {1, 3, 4, C2::Color::Gold},
    // right
    {4, 3, 5, C2::Color::RoyalBlue},
    {4, 5, 6, C2::Color::Silver},
    // back
    {6, 5, 7, C2::Color::Blue},
    {6, 7, 8, C2::Color::DeepPink},
    // left
    {8, 7, 2, C2::Color::Maroon},
    {8, 2, 1, C2::Color::Brown},
    // top
    {2, 7, 5, C2::Color::ForestGreen},
    {2, 5, 3, C2::Color::Pink},
    // bottom
    {6, 8, 1, C2::Color::Olive},
    {6, 1, 4, C2::Color::Navy}
};

void load_cube_mesh_data() {
    for (int i = 0; i < N_CUBE_VERTICES; i++) {
        mesh.vertices.push_back(cube_vertices[i]);
    }

    for (int i = 0; i < N_CUBE_FACES; i++) {
        Face f = cube_faces[i];
        f.a -= 1;
        f.b -= 1;
        f.c -= 1;
        mesh.faces.push_back(f);
    }
}

void load_obj_file_data(const char* filename) {
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

                float xt, yt, zt;
                coords >> xt >> yt >> zt;
                mesh.vertices.push_back({xt, yt, zt}); 
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
                    int v, t, n;
                    int result = sscanf(str.c_str(), "%d/%d/%d", &v, &t, &n);

                    if (result != 0) {
                        if (i == 0) {
                            face.a = v - 1;
                        } else if (i == 1) {
                            face.b = v - 1;
                        } else {
                            face.c = v - 1;
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
}
