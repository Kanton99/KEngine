#include "util_structs.hpp"
#include <cstddef>
#include <format>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION
/*#define TINYOBJLOADER_USE_MAPBOX_EARCUT*/
#include <tiny_obj_loader.h>
#include "objLoader.hpp"
namespace mvk {
std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadObj(vkEngine* engine, std::filesystem::path filePath){
  tinyobj::ObjReaderConfig readerConfig;
  tinyobj::ObjReader reader;

  if(!reader.ParseFromFile(filePath.string())){
    if(!reader.Error().empty()){
      std::cerr << "TinyObjReader: " << reader.Error();
    }
    auto errorMessage = std::format("Failed to open .obj file: {}", filePath.string());
    throw std::runtime_error("Failed to open obj file");
  }

  if(!reader.Warning().empty()) std::cout << reader.Warning();

  auto& attrib = reader.GetAttrib();
  auto& shapes = reader.GetShapes();
  auto& materials = reader.GetMaterials();
  
  std::vector<std::shared_ptr<MeshAsset>> assets;

  std::vector<VertexData> vertices;
  std::vector<uint32_t> indeces;
  for(const auto& shape : shapes){
    MeshAsset asset;
    asset.name = shape.name;

    vertices.clear();
    indeces.clear();
    GeoSurface surface;
    surface.startIndex = (uint32_t)indeces.size();
    surface.count = (uint32_t)shape.mesh.indices.size();
    asset.surfaces.push_back(surface);

    for(auto i : shape.mesh.indices) indeces.push_back((uint32_t)i.vertex_index);  

    for(size_t v = 0; v < attrib.vertices.size(); v+=3) {
      VertexData vertex;

      vertex.position.x = attrib.vertices[v+0];
      vertex.position.y = attrib.vertices[v+1];
      vertex.position.z = attrib.vertices[v+2];

      vertices.push_back(vertex);
    }

    if(shape.mesh.indices[0].normal_index >= 0){
      for(size_t n = 0; n < attrib.normals.size(); n+=3){
        vertices[n/3].normals.x = attrib.normals[n+0];
        vertices[n/3].normals.y = attrib.normals[n+1];
        vertices[n/3].normals.z = attrib.normals[n+2];
      }
    }else{
      //CALCULATE NORMALS 
      for(size_t i = 0; i < indeces.size(); i+=3){
        VertexData A = vertices[indeces[i]+0];
        VertexData B = vertices[indeces[i]+1];
        VertexData C = vertices[indeces[i]+2];

        auto ab = B.position - A.position;
        auto ac = C.position - A.position;

        auto normal = glm::normalize(glm::cross(ab, ac));

        vertices[indeces[i]+0].normals = normal;
        vertices[indeces[i]+1].normals = normal;
        vertices[indeces[i]+2].normals = normal;
      }
    }

    constexpr bool showNormals = true;
    if(showNormals){
      for(VertexData& vertex : vertices){
        vertex.colour = glm::vec4(vertex.normals, 1.f);
      }
    }
    asset.buffers = engine->uploadMesh(vertices, indeces);
    assets.emplace_back(std::make_shared<MeshAsset>(std::move(asset)));
  }
  return assets;
}
}
