#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCircle(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color) {
	Release();
	Init();

	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	std::vector<vector3> vertex;
	GLfloat theta = 0;
	GLfloat delta = static_cast<GLfloat>(2 * PI / a_nSubdivisions);
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 temp = vector3(cos(theta) * a_fRadius, sin(theta) * a_fRadius, 0.0f);
		theta += delta;
		vertex.push_back(temp);
	}
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(ZERO_V3, vertex[i], vertex[(i + 1) % a_nSubdivisions]);
	}
	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3> vertex;
	GLfloat theta = 0;
	GLfloat delta = static_cast<GLfloat>(2 * PI / a_nSubdivisions);
	for (int i = 0; i < a_nSubdivisions; i++)				// creates base circle
	{
		vector3 temp = vector3(cos(theta) * a_fRadius, sin(theta) * a_fRadius, 0.0f);
		theta += delta;
		vertex.push_back(temp);
	}
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(ZERO_V3, vertex[i], vertex[(i + 1) % a_nSubdivisions]);
	}
	for (int i = 0; i < a_nSubdivisions; i++) {				// creates traingles from base circle to the top point
		AddTri(vertex[i], vector3(0, 0, a_fHeight), vertex[(i + 1) % a_nSubdivisions]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3> vertex;
	GLfloat theta = 0;
	GLfloat delta = static_cast<GLfloat>(2 * PI / a_nSubdivisions);
	for (int i = 0; i < a_nSubdivisions; i++)				// creates first circle
	{
		vector3 temp = vector3(cos(theta) * a_fRadius, sin(theta) * a_fRadius, 0.0f);
		theta += delta;
		vertex.push_back(temp);
	}
	std::vector<vector3> vertex2;					// create a second circle as a copy of first; doesn't work
	for (int i= 0; i < a_nSubdivisions; i++) {					// increase the z-value by height; doesn't work
		vector3 temp = vector3(cos(theta) * a_fRadius, sin(theta) * a_fRadius, a_fHeight);
		theta += delta;
		vertex2.push_back(temp);							// gonna do the same thing as 1st time
	}
	for (int i = 0; i < a_nSubdivisions; i++)	// make the circles
	{
		AddTri(vertex[(i + 1) % a_nSubdivisions], vertex[i], ZERO_V3);
		AddTri(vector3(0, 0, a_fHeight),/*(vertex2[i] + vertex2[(i + a_nSubdivisions / 2) % a_nSubdivisions]) / 2*/
			vertex2[i], vertex2[(i + 1) % a_nSubdivisions]);
	}

	for (int i = 0; i < a_nSubdivisions; i++) {
		AddQuad(vertex[i], vertex[(i + 1) % a_nSubdivisions], vertex2[i], vertex2[(i + 1) % a_nSubdivisions]);
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3> vertex;
	GLfloat theta = 0;
	GLfloat delta = static_cast<GLfloat>(2 * PI / a_nSubdivisions);
	for (int i = 0; i < a_nSubdivisions; i++)				// creates Top Outside circle
	{
		vector3 temp = vector3(cos(theta) * a_fOuterRadius, sin(theta) * a_fOuterRadius, 0.0f);
		theta += delta;
		vertex.push_back(temp);
	}
	std::vector<vector3> vertex2;					// create a Bottom Outside circle
	for (int i = 0; i < a_nSubdivisions; i++) {
		vector3 temp = vector3(cos(theta) * a_fOuterRadius, sin(theta) * a_fOuterRadius, a_fHeight);
		theta += delta;
		vertex2.push_back(temp);
	}
	std::vector<vector3> vertex3;
	for (int i = 0; i < a_nSubdivisions; i++)				// creates Inner Top
	{
		vector3 temp = vector3(cos(theta) * a_fInnerRadius, sin(theta) * a_fInnerRadius, 0.0f);
		theta += delta;
		vertex3.push_back(temp);
	}
	std::vector<vector3> vertex4;
	for (int i = 0; i < a_nSubdivisions; i++) {				// Inner Bottom
		vector3 temp = vector3(cos(theta) * a_fInnerRadius, sin(theta) * a_fInnerRadius, a_fHeight);
		theta += delta;
		vertex4.push_back(temp);
	}

	for (int i = 0; i < a_nSubdivisions; i++)	// make the Top Inner and Outer circles connect
	{
		AddQuad(vertex3[i], vertex3[(i + 1) % a_nSubdivisions], vertex[i], vertex[(i + 1) % a_nSubdivisions]);
		AddQuad( vertex2[i], vertex2[(i + 1) % a_nSubdivisions], vertex4[i], vertex4[(i + 1) % a_nSubdivisions]);
		AddQuad(vertex[i], vertex[(i + 1) % a_nSubdivisions], vertex2[i], vertex2[(i + 1) % a_nSubdivisions]);

		AddQuad(vertex4[i], vertex4[(i + 1) % a_nSubdivisions], vertex3[i], vertex3[(i + 1) % a_nSubdivisions]);	// inner cylinder
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3> vertex;
	GLfloat theta = 0;
	GLfloat delta = static_cast<GLfloat>(2 * PI / static_cast<float>(a_nSubdivisionsA));
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		vector3 temp = vector3(cos(theta) * a_fInnerRadius, sin(theta) * a_fInnerRadius, 0);
		theta += delta;
		vertex.push_back(temp);
	}

	std::vector<vector3> torusVertArr;						// contains all vertices; useless here, but I wanted to use it first time around
	std::vector<vector3> vertexTemp1;

	// we run the loop one more time to connect it back to the first point.

	for (uint s = 0; s < a_nSubdivisionsB+1; s++) {			// vertexTemp gets deleted after loop is over
		std::vector<vector3> vertexTemp2 = vertex;			// create a new list to store the vertexes
		matrix4 m4Transform;								// To connect them together
		float fTheta = (2*PI / static_cast<float>(a_nSubdivisionsB) * static_cast<float>(s));
		m4Transform = glm::rotate(IDENTITY_M4, fTheta, AXIS_Y);
		m4Transform = glm::translate(m4Transform, vector3(a_fOuterRadius, 0, 0));
		
		for (int i = 0; i < a_nSubdivisionsB; i++) {
			//torusVertArr[s*a_nSubdivisionsB+i] = m4Transform * vector4(vertex[i], 1.0f);
			torusVertArr.push_back(m4Transform * vector4(vertex[i], 1.0f));
			vertexTemp2[i] = m4Transform * vector4(vertex[i], 1.0f);
		}


		for (int i = 0; i < a_nSubdivisionsB; i++) {
			//AddTri(ZERO_V3, vertexTemp2[i], vertexTemp2[(i + 1) % a_nSubdivisionsB]);
			if (s >= 1)
				/*AddQuad(torusVertArr[((s - 1) * a_nSubdivisionsB + i) % a_nSubdivisionsB], torusVertArr[((s - 1) * a_nSubdivisionsB * i) % a_nSubdivisionsB],					// I tried using a singular array, but my math is off 
					torusVertArr[(s * a_nSubdivisionsB + i+1) % a_nSubdivisionsB], torusVertArr[(s * a_nSubdivisionsB * i+1) % a_nSubdivisionsB]);*/							// if I could implement it, the hole could, theoretically be fixed
				AddQuad(vertexTemp2[i % a_nSubdivisionsB], vertexTemp2[(i + 1) % a_nSubdivisionsB], vertexTemp1[i % a_nSubdivisionsB], vertexTemp1[(i + 1) % a_nSubdivisionsB]);
		}
		vertexTemp1 = vertexTemp2;
	}


	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color) {
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3> vertex;
	GLfloat theta = 0;
	GLfloat delta = static_cast<GLfloat>(2 * PI / static_cast<float>(a_nSubdivisions));
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 temp = vector3(cos(theta) * a_fRadius, sin(theta) * a_fRadius, 0);
		theta += delta;
		vertex.push_back(temp);
	}

	std::vector<vector3> vertexTemp1;

	for (uint s = 0; s < a_nSubdivisions; s++) {
		std::vector<vector3> vertexTemp2 = vertex;
		matrix4 m4Transform;					
		float fTheta = (2 * PI / static_cast<float>(a_nSubdivisions) * static_cast<float>(s));
		m4Transform = glm::rotate(IDENTITY_M4, fTheta, AXIS_Y);						

		for (int i = 0; i < a_nSubdivisions; i++) {
			vertexTemp2[i] = m4Transform * vector4(vertex[i], 1.0f);		// rotate the vertex array, but don't transform
		}


		for (int i = 0; i < a_nSubdivisions; i++) {
			AddTri(ZERO_V3, vertexTemp2[i], vertexTemp2[(i + 1) % a_nSubdivisions]);
			if (s >= 1)
				AddQuad(vertexTemp1[i % a_nSubdivisions], vertexTemp1[(i + 1) % a_nSubdivisions], vertexTemp2[i % a_nSubdivisions], vertexTemp2[(i + 1) % a_nSubdivisions]);
		}
		// I connected the points from the previous iteration to the next interation?
		// Why don't they show up?
		// The circles should connect, right?



		vertexTemp1 = vertexTemp2;

		// Adding information about color
		CompleteMesh(a_v3Color);
		CompileOpenGL3X();
	}
}