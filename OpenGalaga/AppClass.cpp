#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 3.0f, 13.0f), //Position
		vector3(0.0f, 3.0f, 12.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light(0 is reserved for global light)

	//submarine
	m_pSub = new Model();
	m_pSub->Load("Submarine\\sub.obj");
	m_pSubRB = new MyRigidBody(m_pSub->GetVertexList());

	//mine
	m_pMine = new Model();
	m_pMine->Load("Submarine\\mine.obj");
	m_pMineRB = new MyRigidBody(m_pMine->GetVertexList());

	//torpedo
	m_pTorpedo = new Model();
	m_pTorpedo->Load("Submarine\\torpedo.obj");
	m_pTorpedoRB = new MyRigidBody(m_pTorpedo->GetVertexList());

}
void Application::Update(void)
{
	static float move;

	move += 0.1f;
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Set model matrix to the sub
	matrix4 mSub = glm::translate(m_v3Sub) * ToMatrix4(m_qSub) * ToMatrix4(m_qArcBall);
	m_pSub->SetModelMatrix(mSub);
	m_pSubRB->SetModelMatrix(mSub);
	m_pMeshMngr->AddAxisToRenderList(mSub);

	//Set model matrix to Mine
	matrix4 mMine = glm::translate(vector3(4.25f, 0.0f, 0.0f)) * glm::rotate(IDENTITY_M4, -55.0f, AXIS_Z);
	m_pMine->SetModelMatrix(mMine);
	m_pMineRB->SetModelMatrix(mMine);
	m_pMeshMngr->AddAxisToRenderList(mMine);

	//Set model matrix to Torpedo
	matrix4 mTorpedo = glm::translate(vector3(0.0f, -1.0f, move)) * glm::scale(glm::vec3(0.3f,0.3f,0.3f));
	m_pTorpedo->SetModelMatrix(mTorpedo);
	m_pTorpedoRB->SetModelMatrix(mTorpedo);
	m_pMeshMngr->AddAxisToRenderList(mTorpedo);

	bool bColliding = m_pSubRB->IsColliding(m_pMineRB);

	m_pSub->AddToRenderList();
	m_pSubRB->AddToRenderList();

	m_pMine->AddToRenderList();
	m_pMineRB->AddToRenderList();

	m_pTorpedo->AddToRenderList();
	m_pTorpedoRB->AddToRenderList();

	m_pMeshMngr->Print("Colliding: ");
	if (bColliding)
		m_pMeshMngr->PrintLine("YES!", C_RED);
	else {
		m_pMeshMngr->PrintLine("no", C_YELLOW);
	}
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList("Water.jpg");
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}

void Application::Release(void)
{

	//release the model
	SafeDelete(m_pSub);

	//release the rigid body for the model
	SafeDelete(m_pSubRB);

	//release the model
	//SafeDelete(m_pSteve);

	//release the rigid body for the model
	//SafeDelete(m_pSteveRB);

	//release GUI
	ShutdownGUI();
}