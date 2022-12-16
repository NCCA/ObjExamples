#include <ngl/Obj.h>
#include <iostream>
#include <algorithm>

int main(int argc, char **argv)
{

  if(argc <2)
  {
    std::cerr<<"enter an obj file name\n";
    exit(EXIT_FAILURE);
  }
  ngl::Obj mesh(argv[1]);
  std::cout<<"Mesh Info\n";
  std::cout<<"Num Faces "<<mesh.getNumFaces()<<'\n';
  std::cout<<"Num Verts "<<mesh.getNumVerts()<<'\n';
  std::cout<<"Num Normals "<<mesh.getNumNormals()<<'\n';
  std::cout<<"Num UVs "<<mesh.getNumTexCords()<<'\n';
  // lets see if this mesh is a quad
  // first lets get the faces
  auto faces=mesh.getFaceList();
  // Now we can use all_of to see if we have a quad
  bool isQuad= std::all_of(std::begin(faces),std::end(faces),[](auto f){
     return  f.m_vert.size() == 4;
  });
  if( !isQuad)
  {
    std::cout<<"Mesh is not a quad \n";
    exit(EXIT_FAILURE);
  }
  // now get the rest of the mesh data
  auto verts=mesh.getVertexList();
  auto normals=mesh.getNormalList();
  auto uv=mesh.getUVList();
  ngl::Obj newMesh;
  for(auto v : verts)
  {
    newMesh.addVertex(v);
  }
  for(auto n : normals)
  {
    newMesh.addNormal(n);
  }
  for(auto u : uv)
  {
    newMesh.addUV(u);
  }
  // first we can copy the data from the old mesh to the new
  
  // 0---1
  // |   |
  // 2---3
  // given the layout above for the face we can create new triangles
  for(auto f : faces)
  {
    // we have 4 index values we can write directly
    ngl::Face f1;
    f1.m_vert.push_back(f.m_vert[0]);
    f1.m_vert.push_back(f.m_vert[1]);
    f1.m_vert.push_back(f.m_vert[3]);
    f1.m_norm.push_back(f.m_norm[0]);
    f1.m_norm.push_back(f.m_norm[1]);
    f1.m_norm.push_back(f.m_norm[3]);
    f1.m_uv.push_back(f.m_uv[0]);
    f1.m_uv.push_back(f.m_uv[1]);
    f1.m_uv.push_back(f.m_uv[3]);
    newMesh.addFace(f1);
    // f2
    ngl::Face f2;
    f2.m_vert.push_back(f.m_vert[2]);
    f2.m_vert.push_back(f.m_vert[1]);
    f2.m_vert.push_back(f.m_vert[3]);
    f2.m_norm.push_back(f.m_norm[2]);
    f2.m_norm.push_back(f.m_norm[1]);
    f2.m_norm.push_back(f.m_norm[3]);
    f2.m_uv.push_back(f.m_uv[2]);
    f2.m_uv.push_back(f.m_uv[1]);
    f2.m_uv.push_back(f.m_uv[3]);
    newMesh.addFace(f2);
  }

  newMesh.save("triCube.obj");
  return EXIT_SUCCESS;
}