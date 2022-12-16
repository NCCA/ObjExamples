#include <ngl/Obj.h>
#include <iostream>
#include <algorithm>
#include <ngl/NGLStream.h>

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

  // now let's find the center
  for (auto f : faces)
  {
      ngl::Vec3 center;
      size_t currentVertIndex=f.m_vert.size();
      for(auto v : f.m_vert)
      {
        center+=verts[v];
      }
      center/=f.m_vert.size();

      ngl::Vec3 newUV;
      size_t currentUVIndex=f.m_uv.size();
      for(auto u : f.m_uv)
      {
        newUV+=uv[u];
      }
      newUV/=f.m_vert.size();
    

      std::cout<<"center is "<<center<<'\n';
      std::cout<<"New UV is "<<newUV<<'\n';
      newMesh.addVertex(center);
      newMesh.addUV(newUV);
      // Now we are going to grab adjacent face index values and form triangles with the center
      for(size_t i=0; i<f.m_vert.size(); ++i)
      {
        ngl::Face fn;
        fn.m_vert.push_back(f.m_vert[i]);
        fn.m_norm.push_back(f.m_norm[i]);
        fn.m_uv.push_back(f.m_uv[i]);
        // vert 2
        fn.m_vert.push_back(f.m_vert[i+1]);
        fn.m_norm.push_back(f.m_norm[i+1]);
        fn.m_uv.push_back(f.m_uv[i+1]);
        // vert 3
        fn.m_vert.push_back(currentVertIndex);
        fn.m_norm.push_back(f.m_norm[i+1]); // use same norm and uv
        fn.m_uv.push_back(currentUVIndex);
        newMesh.addFace(fn);
      }
      // Now add first to last

  }
  newMesh.save("triMesh.obj");
  return EXIT_SUCCESS;
}