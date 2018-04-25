#include "Octree.h"
using namespace Simplex;

Octree::Octree(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	m_pRoot = this;

	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uLevel = 0;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	Init();
	for (uint i = 0; i < m_uMaxLevel; i++) {
		Subdivide();
	}
}

Octree::Octree(vector3 a_v3Center, float a_fSize)
{
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
}

void Octree::Init(void)
{
	m_pEntityMngr->ClearDimensionSetAll();
	vector3 m_v3Min = glm::vec3(std::numeric_limits<float>::max());
	vector3 m_v3Max = glm::vec3(std::numeric_limits<float>::min());

	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++) {
		m_EntityList.push_back(i);
		m_pEntityMngr->AddDimension(i, m_uID);

		vector3 e_v3Max = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal();
		vector3 e_v3Min = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal();

		if (e_v3Max.x > m_v3Max.x) {
			m_v3Max = glm::vec3(e_v3Max.x, m_v3Max.y, m_v3Max.z);
		}
		if (e_v3Max.y > m_v3Max.y) {
			m_v3Max = glm::vec3(m_v3Max.x, e_v3Max.y, m_v3Max.z);
		}
		if (e_v3Max.z > m_v3Max.z) {
			m_v3Max = glm::vec3(m_v3Max.x, m_v3Max.y, e_v3Max.z);
		}
		if (e_v3Min.x < m_v3Min.x) {
			m_v3Min = glm::vec3(e_v3Min.x, m_v3Min.y, m_v3Min.z);
		}
		if (e_v3Min.y < m_v3Min.y) {
			m_v3Min = glm::vec3(m_v3Min.x, e_v3Min.y, m_v3Min.z);
		}
		if (e_v3Min.z < m_v3Min.z) {
			m_v3Min = glm::vec3(m_v3Min.x, m_v3Min.y, e_v3Min.z);
		}
	}
	if (m_pEntityMngr->GetEntityCount() > 0) {
		m_v3Center = glm::vec3(
			(m_v3Max.x + m_v3Min.x) / 2,
			(m_v3Max.y + m_v3Min.y) / 2,
			(m_v3Max.z + m_v3Min.z) / 2
		);
		m_fSize = glm::max(
			(m_v3Max.x - m_v3Min.x), 
			glm::max(
			(m_v3Max.y - m_v3Min.y),
			(m_v3Max.z - m_v3Min.z)));
	}
	else {
		m_v3Center = glm::vec3(0);
		m_fSize = 20.0f;
	}
}
void Octree::Display(vector3 a_v3Color)
{
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center) * glm::scale(glm::vec3(m_fSize)), a_v3Color);
	if (m_uChildren == 8) {
		for (uint i = 0; i < 8; i++) {
			m_pChild[i]->Display();
		}
	}
}

void Simplex::Octree::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (m_uID == a_nIndex) {
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center) * glm::scale(glm::vec3(m_fSize)), a_v3Color);
		return;
	}
	if (m_uChildren == 8) {
		for (uint i = 0; i < 8; i++) {
			m_pChild[i]->Display(a_nIndex);
		}
	}
}

void Octree::Subdivide(void)
{
	if (m_uChildren != 8) {
		if (m_EntityList.size() > m_pRoot->m_uIdealEntityCount) {
			DoSubdivide();
		}
	}
	else {
		for (uint i = 0; i < m_lChild.size(); i++) {
			m_lChild[i]->Subdivide();
		}
	}
}

void Simplex::Octree::Update(void)
{
}

uint Simplex::Octree::GetOctantCount(void)
{
	return m_uOctantCount;
}

void Octree::DoSubdivide(void)
{
	//top front right
	m_pChild[0] = new Octree(glm::vec3(
		m_v3Center.x + m_fSize / 4,
		m_v3Center.y + m_fSize / 4,
		m_v3Center.z + m_fSize / 4
	), m_fSize / 2);

	//bottom front right
	m_pChild[1] = new Octree(glm::vec3(
		m_v3Center.x + m_fSize / 4,
		m_v3Center.y - m_fSize / 4,
		m_v3Center.z + m_fSize / 4
	), m_fSize / 2);

	//bottom front left
	m_pChild[2] = new Octree(glm::vec3(
		m_v3Center.x - m_fSize / 4,
		m_v3Center.y - m_fSize / 4,
		m_v3Center.z + m_fSize / 4
	), m_fSize / 2);

	//top front left
	m_pChild[3] = new Octree(glm::vec3(
		m_v3Center.x - m_fSize / 4,
		m_v3Center.y + m_fSize / 4,
		m_v3Center.z + m_fSize / 4
	), m_fSize / 2);

	//top back right
	m_pChild[4] = new Octree(glm::vec3(
		m_v3Center.x + m_fSize / 4,
		m_v3Center.y + m_fSize / 4,
		m_v3Center.z - m_fSize / 4
	), m_fSize / 2);

	//bottom back right
	m_pChild[5] = new Octree(glm::vec3(
		m_v3Center.x + m_fSize / 4,
		m_v3Center.y - m_fSize / 4,
		m_v3Center.z - m_fSize / 4
	), m_fSize / 2);

	//bottom back left
	m_pChild[6] = new Octree(glm::vec3(
		m_v3Center.x - m_fSize / 4,
		m_v3Center.y - m_fSize / 4,
		m_v3Center.z - m_fSize / 4
	), m_fSize / 2);

	//top back left
	m_pChild[7] = new Octree(glm::vec3(
		m_v3Center.x - m_fSize / 4,
		m_v3Center.y + m_fSize / 4,
		m_v3Center.z - m_fSize / 4
	), m_fSize / 2);

	for (uint i = 0; i < 8; i++) {
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		++m_pRoot->m_uOctantCount;
		m_pChild[i]->m_uID = m_pRoot->m_uOctantCount;
		if (m_pChild[i]->IsColliding()) {
			m_lChild.push_back(m_pChild[i]);
		};
	}
	m_uChildren = 8;
}

vector3 Octree::GetMaxGlobal(void)
{
	return glm::vec3(
		m_v3Center.x + m_fSize / 2,
		m_v3Center.y + m_fSize / 2,
		m_v3Center.z + m_fSize / 2
	);
}

vector3 Octree::GetMinGlobal(void)
{
	return glm::vec3(
		m_v3Center.x - m_fSize / 2,
		m_v3Center.y - m_fSize / 2,
		m_v3Center.z - m_fSize / 2
	);
}

bool Octree::IsColliding(void)
{
	bool found = false;

	vector3 m_v3Max = GetMaxGlobal();
	vector3 m_v3Min = GetMinGlobal();

	for (uint i = 0; i < m_pParent->m_EntityList.size(); i++) {
		uint entityID = m_pParent->m_EntityList[i];

		vector3 e_v3Max = m_pEntityMngr->GetRigidBody(entityID)->GetMaxGlobal();
		vector3 e_v3Min = m_pEntityMngr->GetRigidBody(entityID)->GetMinGlobal();

		if (e_v3Max.x > m_v3Min.x && e_v3Min.x < m_v3Max.x) {
			if (e_v3Max.y > m_v3Min.y && e_v3Min.y < m_v3Max.y) {
				if (e_v3Max.z > m_v3Min.z && e_v3Min.z < m_v3Max.z) {

					m_EntityList.push_back(entityID);
					m_pEntityMngr->AddDimension(entityID, m_uID);
					m_pEntityMngr->RemoveDimension(entityID, m_pParent->m_uID);
					found = true;
				}
			}
		}
	}
	return found;
}
Octree & Simplex::Octree::operator=(Octree const & other)
{
	return *this;
}

Simplex::Octree::~Octree(void)
{
	//delete[] m_pChild;
}
