#include "util_structs.hpp"
#include <cstddef>
#include <cstdint>
#include <format>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <unordered_map>
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

  std::unordered_map<VertexData, uint32_t> uniqueVertices{}; 
  for(const auto& shape : shapes){
    MeshAsset asset;
    asset.name = shape.name;

    vertices.clear();
    indeces.clear();
    GeoSurface surface;
    surface.startIndex = (uint32_t)indeces.size();
    surface.count = (uint32_t)shape.mesh.indices.size();
    asset.surfaces.push_back(surface);

    for(const auto& index : shape.mesh.indices){
      VertexData vertex;

      vertex.position.x = attrib.vertices[3*size_t(index.vertex_index)+0];
      vertex.position.y = attrib.vertices[3*size_t(index.vertex_index)+1];
      vertex.position.z = attrib.vertices[3*size_t(index.vertex_index)+2];

      if(index.normal_index >= 0){
        vertex.normal.x = attrib.normals[3*size_t(index.normal_index)+0];
        vertex.normal.y = attrib.normals[3*size_t(index.normal_index)+1];
        vertex.normal.z = attrib.normals[3*size_t(index.normal_index)+2];
      }

      if(index.texcoord_index >= 0){
        vertex.uv_x = attrib.texcoords[2*size_t(index.texcoord_index)+0];
        vertex.uv_y = attrib.texcoords[2*size_t(index.texcoord_index)+1];
      }

      if(!attrib.colors.empty()){
        vertex.colour.r = attrib.colors[3*size_t(index.vertex_index)+0];
        vertex.colour.g = attrib.colors[3*size_t(index.vertex_index)+1];
        vertex.colour.b = attrib.colors[3*size_t(index.vertex_index)+2];
      }

      if(uniqueVertices.count(vertex) == 0){
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }

      indeces.push_back(uniqueVertices[vertex]);
    }

    constexpr bool showNormals = true;
    if(showNormals){
      for(VertexData& vertex : vertices){
        auto corrected = (vertex.normal+glm::vec3(1,1,1))/2.f;
        vertex.colour = glm::vec4(corrected, 1.f);
      }
    }
    asset.buffers = engine->uploadMesh(vertices, indeces);
    assets.emplace_back(std::make_shared<MeshAsset>(std::move(asset)));
  }
  return assets;
}
}
