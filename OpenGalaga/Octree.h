#ifndef __MYOCTANTCLASS_H_
#define __MYOCTANTCLASS_H_

#include "MyEntityManager.h"

namespace Simplex
{

	//System Class
	class Octree
	{
		uint m_uOctantCount; //will store the number of octants instantiated
		uint m_uMaxLevel;//will store the maximum level an octant can go to
		uint m_uIdealEntityCount; //will tell how many ideal Entities this object will contain

		uint m_uID = 0; //Will store the current ID for this octant
		uint m_uLevel = 0; //Will store the current level of the octant
		uint m_uChildren = 0;// Number of children on the octant (either 0 or 8)

		float m_fSize = 0.0f; //Size of the octant

		MeshManager* m_pMeshMngr = nullptr;//Mesh Manager singleton
		MyEntityManager* m_pEntityMngr = nullptr; //Entity Manager Singleton

		vector3 m_v3Center = vector3(0.0f); //Will store the center point of the octant
		vector3 m_v3Min = vector3(0.0f); //Will store the minimum vector of the octant
		vector3 m_v3Max = vector3(0.0f); //Will store the maximum vector of the octant

		Octree* m_pParent = nullptr;// Will store the parent of current octant
		Octree* m_pChild[8];//Will store the children of the current octant

		std::vector<uint> m_EntityList; //List of Entities under this octant (Index in Entity Manager)

		Octree* m_pRoot = nullptr;//Root octant
		std::vector<Octree*> m_lChild; //list of nodes that contain objects (this will be applied to root only)

	public:
		Octree(uint a_nMaxLevel = 2, uint a_nIdealEntityCount = 5);
		Octree(vector3 a_v3Center, float a_fSize);
		void Init(void);

		void Display(vector3 a_v3Color = C_YELLOW);
		void Display(uint a_nIndex, vector3 a_v3Color = C_YELLOW);
		void Subdivide(void);
		void Update(void);
		uint GetOctantCount(void);
		bool IsColliding(void);

		Octree& operator=(Octree const& other);
		~Octree(void);

	private: 
		void DoSubdivide(void);

		vector3 GetMaxGlobal(void);
		vector3 GetMinGlobal(void);
	};

}
#endif