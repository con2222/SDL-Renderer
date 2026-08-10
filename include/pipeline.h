#ifndef PIPELINE_H
#define PIPELINE_H

#include "Geometry.h"
#include "configurations.h"
#include <vector>


namespace C2 {

struct Mesh;
struct SceneData;
struct Polygon;
struct Triangle;
struct EngineCore;
struct RenderContext;
struct Frustum;


geom::mat4 make_world_matrix(const Mesh& mesh);
std::vector<Polygon> process_and_cull_geometry(const Mesh& mesh, const SceneData& scene, CullMethod cull_method);
std::vector<Triangle> clip_geometry(std::vector<Polygon>& polygons, const Frustum& frustum);
void project_geometry(std::vector<Triangle>& clipped_triangles, RenderContext& render_context, EngineCore& engine_core);

} // C2

#endif // PIPELINE_H
