#include "gltfLoader.hpp"
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <iostream>

std::optional<std::vector<std::shared_ptr<mvk::MeshAsset>>> mvk::loadGltfMesh(mvk::vkEngine* engine, std::filesystem::path filePath){
  std::cout << "Loading gltf mesh\n";

  //LOADING DATA FROM FILE  
  auto dataBuffer = fastgltf::GltfDataBuffer::FromPath(filePath);
  if (dataBuffer.error() != fastgltf::Error::None) {
    // The file couldn't be loaded, or the buffer could not be allocated.
    std::cout << std::format("Failed to open glTF file: {} \n", filePath.string());
    return {};
  }

  constexpr auto options = fastgltf::Options::LoadExternalBuffers;

  fastgltf::Asset gltf;
  fastgltf::Parser parser;

  auto load = parser.loadGltfBinary(dataBuffer.get(), filePath.parent_path(), options);
  if(load) gltf = std::move(load.get());
  else {
    std::cout << std::format("Failed to load glTF: {} \n", fastgltf::to_underlying(load.error()));
    return {};
  }

  std::vector<std::shared_ptr<MeshAsset>> meshes;

  std::vector<uint32_t> indeces;
  std::vector<VertexData> vertices;

  for(fastgltf::Mesh& mesh : gltf.meshes){
    MeshAsset newMesh;
    newMesh.name = mesh.name;

    indeces.clear();
    vertices.clear();

    for(auto&& p : mesh.primitives){
      GeoSurface newSurface;

      newSurface.startIndex = (uint32_t)indeces.size(); 
      newSurface.count = (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

      size_t initial_vtx = vertices.size();
      //LOAD INDECES
      {
        fastgltf::Accessor& indexAccessor = gltf.accessors[p.indicesAccessor.value()];
        indeces.resize(indeces.size()+indexAccessor.count);

        fastgltf::iterateAccessor<std::uint32_t>(gltf, indexAccessor,
                                                 [&](std::uint32_t idx){
                                                 indeces.push_back(idx+initial_vtx);
                                                 }
                                                 );
      }

      //LOAD VERTICES
      {
      fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->accessorIndex];
      vertices.resize(vertices.size() + posAccessor.count);

      fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
          [&](glm::vec3 v, size_t index) {
              mvk::VertexData newvtx;
              newvtx.position = v;
              newvtx.normals = { 1, 0, 0 };
              newvtx.colour = glm::vec4 { 1.f };
              newvtx.uv_x = 0;
              newvtx.uv_y = 0;
              vertices[initial_vtx + index] = newvtx;
            }, dataBuffer);
      }

      auto normals = p.findAttribute("NORMAL");
      if(normals != p.attributes.end()){
        fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).accessorIndex],
          [&](glm::vec3 v, size_t index) {
            vertices[initial_vtx + index].normals = v;
          });
      }

      // load UVs
      auto uv = p.findAttribute("TEXCOORD_0");
      if (uv != p.attributes.end()) {
        fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).accessorIndex],
          [&](glm::vec2 v, size_t index) {
            vertices[initial_vtx + index].uv_x = v.x;
            vertices[initial_vtx + index].uv_y = v.y;
          });
      }

      // load vertex colors
      auto colors = p.findAttribute("COLOR_0");
      if (colors != p.attributes.end()) {
        fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).accessorIndex],
          [&](glm::vec4 v, size_t index) {
            vertices[initial_vtx + index].colour = v;
          });
      }
      newMesh.surfaces.push_back(newSurface);

    }
    constexpr bool showNormals = true;
    if(showNormals){
      for(VertexData& vtx : vertices){
        vtx.colour = glm::vec4(vtx.normals, 1.f);
      }
    }
    newMesh.buffers = engine->uploadMesh(vertices, indeces);
    meshes.emplace_back(std::make_shared<MeshAsset>(std::move(newMesh)));
  }
  return meshes;
}
