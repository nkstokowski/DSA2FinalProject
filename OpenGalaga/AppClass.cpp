#include "AppClass.h"
using namespace Simplex;


void Application::fireTorpedo() {
	// Add torpedo and retreive unique id
	m_pEntityMngr->AddEntity(m_sTorpedoObjLoc);
	string id = m_pEntityMngr->GetUniqueID();

	// User player matrix to set new torpedo matrix
	matrix4 playerMat = m_pEntityMngr->GetModelMatrix(m_uPlayerId);
	m_pEntityMngr->SetModelMatrix(playerMat * glm::translate(vector3(0.0f, -1.0f, 5.0f)) * glm::scale(glm::vec3(0.3f, 0.3f, 0.3f)));

	// Add torpedo index to torpedo list
	m_lTorpedoList.push_back(m_pEntityMngr->GetEntityIndex(id));

}

void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Nick, Nick, and Noah";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 3.0f, 13.0f), //Position
		vector3(0.0f, 3.0f, 12.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light(0 is reserved for global light)

	m_pEntityMngr->AddEntity("Submarine\\sub.obj", "player_sub");
	m_pEntityMngr->SetModelMatrix(glm::translate(m_v3Sub));
	m_uPlayerId = m_pEntityMngr->GetEntityIndex("player_sub");

	m_pEntityMngr->AddEntity("Submarine\\mine.obj", "mine");
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(4.25f, 0.0f, 0.0f)) * glm::rotate(IDENTITY_M4, -55.0f, AXIS_Z));


	m_uOctantLevels = 1;
	m_pEntityMngr->Update();
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
	matrix4 m4Sub = glm::translate(m_v3Sub) * ToMatrix4(m_qSub) * ToMatrix4(m_qArcBall);
	m_pEntityMngr->SetModelMatrix(m4Sub, m_uPlayerId);

	//Move Torpedos Forward
	matrix4 m4Torpedo;
	for (auto i : m_lTorpedoList) {
		m4Torpedo = m_pEntityMngr->GetModelMatrix(i);
		m4Torpedo *= glm::translate(IDENTITY_M4, vector3(0.0f, 0.0f, 0.5f));
		m_pEntityMngr->SetModelMatrix(m4Torpedo, i);
	}



	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//display octree
	//m_pRoot->Display();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList("Water.jpg");

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