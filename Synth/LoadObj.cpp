#include "LoadObj.hpp"
#include "OBJ_Loader.h"

void LoadObjFile(std::string filename, vector<Quadric>& quadrics)
{
	objl::Loader Loader;
	bool loadout = Loader.LoadFile(filename);
	if (loadout)
	{
		for (unsigned int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = Loader.LoadedMeshes[i];

			for (unsigned int j = 0; j < curMesh.Indices.size(); j += 3)
			{
				cyPoint3d q1 = { curMesh.Vertices[curMesh.Indices[j]].Position.X, curMesh.Vertices[curMesh.Indices[j]].Position.Y, curMesh.Vertices[curMesh.Indices[j]].Position.Z };
				cyPoint3d q2 = { curMesh.Vertices[curMesh.Indices[j + 1]].Position.X, curMesh.Vertices[curMesh.Indices[j + 1]].Position.Y, curMesh.Vertices[curMesh.Indices[j + 1]].Position.Z };
				cyPoint3d q3 = { curMesh.Vertices[curMesh.Indices[j + 2]].Position.X, curMesh.Vertices[curMesh.Indices[j + 2]].Position.Y, curMesh.Vertices[curMesh.Indices[j + 2]].Position.Z };
				cyPoint2d t1 = { curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.Y };
				cyPoint2d t2 = { curMesh.Vertices[curMesh.Indices[j + 1]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j + 1]].TextureCoordinate.Y };
				cyPoint2d t3 = { curMesh.Vertices[curMesh.Indices[j + 2]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j + 2]].TextureCoordinate.Y };
				cyPoint3d c = { curMesh.MeshMaterial.Kd.X, curMesh.MeshMaterial.Kd.Y, curMesh.MeshMaterial.Kd.Z };
				c = c.IsZero() ? palette[(10) % palette.size()] : c;

				//rotVec(q1, up, -animParam2 * 50);
				//rotVec(q2, up, -animParam2 * 50);
				//rotVec(q3, up, -animParam2 * 50);
				quadrics.push_back(planeFromPoints(q1 / 0.5, q2 / 0.5, q3 / 0.5, c, t1, t2, t3, Image(), Image(), 0.5, 1.8));
			}
		}
	}
}