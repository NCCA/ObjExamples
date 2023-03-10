#include <ngl/Obj.h>
#include <ngl/Util.h>
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
  bool isTri= std::all_of(std::begin(faces),std::end(faces),[](auto f){
     return  f.m_vert.size() == 3;
  });
  if( !isTri)
  {
    std::cout<<"Mesh is not a triangle \n";
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
  for(auto f : faces)
  {
    ngl::Vec3 center;
    size_t currentVertIndex=newMesh.getNumVerts();
    for(auto v : f.m_vert)
    {
      center+=verts[v];
    }
    center/=f.m_vert.size();
    newMesh.addVertex(center);
    ngl::Vec3 newUV;
    size_t currentUVIndex=newMesh.getNumTexCords();
    for(auto u : f.m_uv)
    {
      newUV+=uv[u];
    }
    newUV/=f.m_vert.size();
    newMesh.addUV(newUV);
    // now we are going to find the half way points of the triangle
    // for ease let's extract the points we need.
    auto p1Index=f.m_vert[0];
    auto p1=verts[p1Index];
    auto uv1Index=f.m_uv[0];
    auto uv1=uv[uv1Index];
    // Original 2
    auto p2Index=f.m_vert[1];
    auto p2=verts[p2Index];
    auto uv2Index=f.m_uv[1];
    auto uv2=uv[uv2Index];
    // Original 3
    auto p3Index=f.m_vert[2];
    auto p3=verts[p3Index];
    auto uv3Index=f.m_uv[2];
    auto uv3=uv[uv3Index];
    // Now get the half points / uv's using lerp
    auto h1=lerp(p1,p2,0.5f);
    auto huv1=lerp(uv1,uv2,0.5f);
    auto h2=lerp(p2,p3,0.5f);
    auto huv2=lerp(uv2,uv3,0.5f);
    auto h3=lerp(p3,p1,0.5f);
    auto huv3=lerp(uv3,uv1,0.5f);
    // these need to be added to the lists and index values
    // remembered for the faces
    newMesh.addVertex(h1); //currentVertIndex+1
    newMesh.addVertex(h2); //currentVertIndex+2
    newMesh.addVertex(h3); //currentVertIndex+3
    newMesh.addUV(huv1); // currentUVIndex+1
    newMesh.addUV(huv2); // currentUVIndex+2
    newMesh.addUV(huv3); // currentUVIndex+3
    // create a lookup table for our index values 
    std::array<size_t,7> vertIndices={
      p1Index,currentVertIndex+1,p2Index,currentVertIndex+2,
      p3Index,currentVertIndex+3,p1Index
     };
    std::array<size_t,7> uvIndices={
      uv1Index,currentUVIndex+1,uv2Index,currentUVIndex+2,
      uv3Index,currentUVIndex+3,uv1Index
    };
    // for each index in the table form a new tri face
    for(size_t i=0; i<6; ++i)
    {
      ngl::Face newFace;
      newFace.m_vert.push_back(vertIndices[i]);
      newFace.m_vert.push_back(vertIndices[i+1]);
      newFace.m_vert.push_back(currentVertIndex); // center
      newFace.m_uv.push_back(uvIndices[i]);
      newFace.m_uv.push_back(uvIndices[i+1]);
      newFace.m_uv.push_back(currentUVIndex); // center
      newFace.m_norm.push_back(f.m_norm[0]);
      newFace.m_norm.push_back(f.m_norm[1]);
      newFace.m_norm.push_back(f.m_norm[2]);
      newMesh.addFace(newFace);
    }
  }

  newMesh.save("triTesselated.obj");
  return EXIT_SUCCESS;
}