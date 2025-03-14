#include "util_structs.hpp"
#include <cstddef>
#include <exception>
#include <format>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION
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
    throw std::exception("Failed to open obj file");
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
    surface.startIndex = indeces.size();
    surface.count = shape.mesh.indices.size();
    asset.surfaces.push_back(surface);
    for(size_t i = 0; i < shape.mesh.indices.size();i++){
      uint32_t vIdx = shape.mesh.indices[i].vertex_index;
      uint32_t nIdx = shape.mesh.indices[i].normal_index;
      uint32_t tIdx = shape.mesh.indices[i].texcoord_index;
      indeces.push_back(vIdx);
      VertexData vertex{};

      vertex.position.x = attrib.vertices[vIdx+0];
      vertex.position.y = attrib.vertices[vIdx+1];
      vertex.position.z = attrib.vertices[vIdx+2];

      if(nIdx >= 0){
        vertex.normals.x = attrib.normals[nIdx+0];
        vertex.normals.y = attrib.normals[nIdx+1];
        vertex.normals.z = attrib.normals[nIdx+2];
      }

      if(tIdx >= 0){
        vertex.uv_x = attrib.texcoords[tIdx+0];
        vertex.uv_y = attrib.texcoords[tIdx+1];
      }

      vertex.colour.x = attrib.colors[vIdx+0];
      vertex.colour.y = attrib.colors[vIdx+1];
      vertex.colour.z = attrib.colors[vIdx+2];
      vertices.push_back(vertex);
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
