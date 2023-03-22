#include "AppClass.h"
using namespace std;


// Jack Shyshko - js4391@rit.edu
//
void Application::InitVariables(void)
{
	// We created a huge double array of where the cubes are supposed to be at
	vector<vector<int>> intArr;

	vector<int> tempInt = { 0,0,1,0,0,0,0,0,1,0,0 };
	intArr.push_back(tempInt);		// int intArr[1] = { 0,0,1,0,0,0,0,0,1,0,0 };

	vector<int> tempInt2 = { 0, 0, 0,1,0,0,0,1,0,0, 0 };
	intArr.push_back(tempInt2);	// int intArr[2] = { 0, 0, 0,1,0,0,0,1,0,0, 0 };
	
	vector<int> tempInt3 = { 0,0,1,1,1,1,1,1,1,0,0 };
	intArr.push_back(tempInt3);	// int intArr[3] = { 0,0,1,1,1,1,1,1,1,0,0 };
	
	vector<int> tempInt4 = { 0,0,1,1,1,1,1,1,1,0,0 };
	intArr.push_back(tempInt4);	// int intArr[4] = { 0,0,1,1,1,1,1,1,1,0,0 };
	
	vector<int> tempInt5 = { 1,0,1,1,1,1,1,1,1,0,1};
	intArr.push_back(tempInt5);	// int intArr[4] = { 0,0,1,1,1,1,1,1,1,0,0 }; 

	vector<int> tempInt6 = { 1,0,1,0,0,0,0,0,1,0,1 };
	intArr.push_back(tempInt6);	// int intArr[4] = { 0,0,1,1,1,1,1,1,1,0,0 };

	vector<int> tempInt7 = { 0,0,0,1,1,0,1,1,0,0,0 };
	intArr.push_back(tempInt7);	// int intArr[4] = { 0,0,1,1,1,1,1,1,1,0,0 };


	// create a set of cubes at locations based on where those 
	for (int i = 0; i < intArr.size(); i++) {
		for (int r = 0; r < intArr[i].size(); r++) {
			if (intArr[i][r] != 0) {
				MyMesh* tempMesh = new MyMesh();
				pMeshArr.push_back(tempMesh);
				pMeshArr[i]->GenerateCube(1.0f, C_BLACK);
				m4TranslateArr.push_back(glm::translate(IDENTITY_M4, vector3(i, r, 0.0f)));
			}
		}
	}

	// We created a huge double array of the different positions where the cubes are supposed to be at
	// Now we can go through and create 

	/*
	// populate a two-dimentional vector of Meshes, to create the sprite

	MyMesh* tempArr[]{nullptr,nullptr, new MyMesh(), nullptr, nullptr, nullptr, nullptr, nullptr, new MyMesh(), nullptr, nullptr};
	
	// this doesn't work, I don't understand why
	pMeshArr2[0].insert(pMeshArr2.end(), tempArr, tempArr+11);
	pMeshArr2[1].insert(NULL, nullptr, nullptr,MyMesh(), nullptr, nullptr, nullptr,MyMesh(), nullptr, nullptr, nullptr);
	pMeshArr2[2] = { NULL,NULL,MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),NULL,NULL};
	pMeshArr2[3] = { NULL,MyMesh(),MyMesh(),NULL,MyMesh(),MyMesh(),MyMesh(),NULL,MyMesh(),MyMesh(),};
	pMeshArr2[4] = { MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh()};
	pMeshArr2[5] = { MyMesh(),NULL,MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),MyMesh(),,MyMesh() };
	pMeshArr2[6] = { MyMesh(),NULL,MyMesh(),NULL,NULL,NULL,NULL,NULL,MyMesh(),NULL,MyMesh() };
	pMeshArr2[7] = { NULL,NULL,NULL,MyMesh(),MyMesh(),NULL,MyMesh(),MyMesh(),NULL,NULL,NULL};
	*/
	/*
	for (int i = 0; i < 11; i++) {
		pMeshArr.push_back(new MyMesh());
		pMeshArr[i]->GenerateCube(1.0f, C_BLACK);
		float xTranslate = -6;
		m4TranslateArr.push_back(glm::translate(IDENTITY_M4, vector3(xTranslate+i, 0, 0)));
	}
	for (int i = 0; i < 11; i++) {
		pMeshArr.push_back(new MyMesh());
		pMeshArr[i]->GenerateCube(1.0f, C_BLACK);
		float xTranslate = -6;

		m4TranslateArr.push_back(glm::translate(IDENTITY_M4, vector3(xTranslate + i, 0, 0)));
	}
	*/
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();

	for (int i = 0; i < pMeshArr.size(); i++) {
		pMeshArr[i]->Render(m4Projection, m4View, m4TranslateArr[i]);
	}
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}