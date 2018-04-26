#include "AppClass.h"
using namespace Simplex;


void Application::FireTorpedo() {
	// Add torpedo and retreive unique id
	m_pEntityMngr->AddEntity(m_sTorpedoObjLoc, "NA", "Torpedo");
	string s_id = m_pEntityMngr->GetUniqueID();
	uint u_id = m_pEntityMngr->GetEntityIndex(s_id);
	m_pEntityMngr->SetName("torpedo");

	// User player matrix to set new torpedo matrix
	matrix4 playerMat = m_pEntityMngr->GetModelMatrix(m_uPlayerId);

	m_pEntityMngr->SetModelMatrix(playerMat * glm::translate(vector3(0.0f, -3.0f, 5.0f)), u_id);
	//m_pEntityMngr->UsePhysicsSolver();

	// Add torpedo index to torpedo list
	m_lTorpedoList.push_back(u_id);

}

void Application::GenMines(uint amount) {

	string s_id;
	uint u_id;
	vector3 v3Position;
	matrix4 m4Model;

	for (uint i = 0; i <= amount; i++) {
		m_pEntityMngr->AddEntity("Submarine\\mine.obj", "mine", "Mine");
		
		v3Position = vector3(glm::sphericalRand(50.0f));
		v3Position.y += 50.0f;

		m4Model = glm::translate(v3Position) *  glm::rotate(IDENTITY_M4, -55.0f, AXIS_Z);

		

		m_pEntityMngr->SetModelMatrix(m4Model);
		s_id = m_pEntityMngr->GetUniqueID();
		u_id = m_pEntityMngr->GetEntityIndex(s_id);
		m_pEntityMngr->SetName("mine");
		m_lMineList.push_back(u_id);
	}

}

void Simplex::Application::ExplodeMine(uint a_uMineIndex)
{
	MyEntity* pEntity = m_pEntityMngr->GetEntity(a_uMineIndex);
	pEntity->SetTag("Destroy");
	float x, y, z, deg;
	matrix4 m4Model = pEntity->GetModelMatrix();
	string s_id;
	uint u_id;
	
	uint uShrapnelCount = rand() % (m_uMaxShrapnel - m_uMinShrapnel) + m_uMinShrapnel;
	String sModelBase = "Submarine\\shrap_0";
	String sModelEnd = ".obj";
	String sModelFull;

	for (uint i = 0; i < uShrapnelCount; i++) {
		x = rand() % 360;
		y = rand() % 360;
		z = rand() % 360;
		deg = rand() % 360;

		// Spawn shrapnel and set matrix
		sModelFull = sModelBase + std::to_string(i % 3) + sModelEnd;
		m4Model *= glm::rotate(IDENTITY_M4, deg, vector3(x, y, z));
		m_pEntityMngr->AddEntity(sModelFull, "shrapnel", "Shrapnel");
		m_pEntityMngr->SetModelMatrix(m4Model);
		m_pEntityMngr->UsePhysicsSolver();

		// Add to list
		s_id = m_pEntityMngr->GetUniqueID();
		u_id = m_pEntityMngr->GetEntityIndex(s_id);
		m_lShrapnelList.push_back(u_id);

		// Apply explosion force
		x = rand() % 360;
		y = rand() % 360;
		z = rand() % 360;
		m_pEntityMngr->ApplyForce(vector3(x, y, z));
	}
}

void Simplex::Application::ExplodeAllMines(void)
{
	for (auto i : m_lMineList) {
		ExplodeMine(i);
	}
}



void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Nick, Nick, and Noah";

	// Seed random
	srand(time(NULL));

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 3.0f, 13.0f), //Position
		vector3(0.0f, 3.0f, 12.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light(0 is reserved for global light)

	// Create Player
	m_pEntityMngr->AddEntity("Submarine\\sub.obj", "player_sub", "Player");
	m_pEntityMngr->SetModelMatrix(glm::translate(m_v3Sub));
	m_uPlayerId = m_pEntityMngr->GetEntityIndex("player_sub");
	//m_pEntityMngr->UsePhysicsSolver();

	// Make Some initial mines
	GenMines(60);

	m_pEntityMngr->Update();
	m_pRoot = new Octree(m_uOctantLevels, m_uOctantIdealCount);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	// Move Submarine
	m_pCameraMngr->SetTarget(m_v3Sub);
	matrix4 m4Sub = glm::translate(m_v3Sub) * ToMatrix4(m_qSub) * ToMatrix4(m_qArcBall);
	m_pEntityMngr->SetModelMatrix(m4Sub, m_uPlayerId);

	// Check all Mines for collision
	MyRigidBody::PRigidBody* lCollisions;
	uint uCollisionCount;
	MyEntity* pEntity;
	bool bShouldDestroy = false;
	String sOtherTag;
	for (auto i : m_lMineList) {
		pEntity = m_pEntityMngr->GetEntity(i);
		// Ensure this mine has not already been marked for destruction
		bShouldDestroy = (pEntity->GetTag() == "Destroy");
		
		// If not, double check using collisions
		if (!bShouldDestroy) {
			lCollisions = pEntity->GetColliderArray();
			uCollisionCount = pEntity->GetCollidingCount();

			for (uint x = 0; x < uCollisionCount; x++) {
				sOtherTag = lCollisions[x]->GetTag();
				if (sOtherTag != "Player" && sOtherTag != "Mine") {
					bShouldDestroy = true;
					ExplodeMine(i);
					lCollisions[x]->SetTag("Destroy");
				}
			}
		}

		// Finally, Destroy if needed
		if (bShouldDestroy) {
			m_lToDelete.push_back(i);
		}

	}

	// Update Torpedos
	matrix4 m4Torpedo;
	for (auto i : m_lTorpedoList) {

		// Mark all 
		if (m_pEntityMngr->GetLifeTime(i) > m_uTorpedoLife ||
				m_pEntityMngr->GetRigidBody(i)->GetTag() == "Destroy") {
			m_lToDelete.push_back(i);
		}
		else {
			//Move Torpedos Forward
			m4Torpedo = m_pEntityMngr->GetModelMatrix(i);
			m4Torpedo *= glm::translate(IDENTITY_M4, vector3(0.0f, 0.0f, 0.5f));
			m_pEntityMngr->SetModelMatrix(m4Torpedo, i);
		}
	}

	// Destroy Expired Shrapnel
	for (auto i : m_lShrapnelList) {
		if (m_pEntityMngr->GetLifeTime(i) > m_uShrapnelLife ||
			m_pEntityMngr->GetRigidBody(i)->GetTag() == "Destroy") {
			m_lToDelete.push_back(i);
		}
	}


	// Remove entities marked for deletion
	for (uint i = 0; i < m_lToDelete.size(); i++) {
		m_pEntityMngr->RemoveEntity(m_lToDelete[i]);
	}

	// Rebuild Lists
	String s_Name;
	m_lToDelete.clear();
	m_lTorpedoList.clear();
	m_lMineList.clear();
	m_lShrapnelList.clear();
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++) {
		s_Name = m_pEntityMngr->GetEntity(i)->GetName();
		if (s_Name == "torpedo") {
			m_lTorpedoList.push_back(i);
		}
		else if (s_Name == "mine") {
			m_lMineList.push_back(i);
		}
		else if (s_Name == "shrapnel") {
			m_lShrapnelList.push_back(i);
		}
	}


	//Update Entity Manager
	m_pEntityMngr->Update();
	
	SafeDelete(m_pRoot);
	m_pRoot = new Octree(m_uOctantLevels, m_uOctantIdealCount);

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//display octree
	if (m_bDisplayOctree) {
		m_pRoot->Display();
	}

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList("Water.jpg");

	//draw ground
	matrix4 groundMat = glm::translate(IDENTITY_M4, vector3(0.0f, -0.6f, 0.0f));
	groundMat = glm::rotate(groundMat, -90.0f, vector3(90, 0, 0));
	groundMat = glm::scale(groundMat, vector3(1000, 1000, 1));
	m_pMeshMngr->AddPlaneToRenderList(groundMat, vector3(0.59f, 0.6f, 0.52f));

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}