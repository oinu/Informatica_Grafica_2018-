#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <iostream>
#include <cassert>

#include "GL_framework.h"
#include <vector>

#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
//variables to load an object:

std::vector< glm::vec3 > vertices;
std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals;


glm::vec3 sunPos,moonPos,lightPos;

extern bool loadOBJ(const char * path,
	std::vector < glm::vec3 > & out_vertices,
	std::vector < glm::vec2 > & out_uvs,
	std::vector < glm::vec3 > & out_normals
);



bool show_test_window = false;


bool light_moves = true;
void GUI() {
	bool show = true;
	ImGui::Begin("Simulation Parameters", &show, 0);

	// Do your GUI code here....
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate


		if (ImGui::Button("Toggle Light Move")) {
			light_moves = !light_moves;

		}


	}
	// .........................

	ImGui::End();

	// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 60), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}


float distanceCenter = 25;
float distanceCabin = (2*3.1415)/20;
float speedMultiplayer = 0.5;
glm::vec3 centerScene = glm::vec3(0.0f, 20, 0.0f);
glm::vec3 lightColor = glm::vec3(0.5f,0.5f,0.1f);
bool focusTrump = false;

//Exercise variables
int exercise = 1;
void SetExercise(int id)
{
	exercise = id;
}
int GetExercise()
{
	return exercise;
}

//Models variables
bool modelActivated = false;
void SetModelActivated(bool e)
{
	modelActivated = e;
}
bool GetModelActivated()
{
	return modelActivated;
}

//Change Camara
int camara = 0;
void SetCamara(int id)
{
	camara = id;
	if (camara > 3)camara = 0;
}
int GetCamara()
{
	return camara;
}


///////// fw decl
namespace ImGui {
	void Render();
}

namespace Cube {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}
namespace Cabin {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}

namespace Trump {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}

namespace Chicken {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}

namespace Wheel {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}

namespace Feet {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}

namespace Sphere {
	void setupSphere(glm::vec3 pos, float radius);
	void cleanupSphere();
	void updateSphere(glm::vec3 pos, float radius);
	void drawSphere();
	void drawSphere2();
}



////////////////

namespace RenderVars {
	const float FOV = glm::radians(65.f);
	const float zNear = 1.f;
	const float zFar = 500.f;

	glm::mat4 _projection;
	glm::mat4 _modelView;
	glm::mat4 _MVP;
	glm::mat4 _inv_modelview;
	glm::vec4 _cameraPoint;

	struct prevMouse {
		float lastx, lasty;
		MouseEvent::Button button = MouseEvent::Button::None;
		bool waspressed = false;
	} prevMouse;

	float panv[3] = { 0.f, -20.f, -50.f };
	float rota[2] = { 0.f, 0.f };
}
namespace RV = RenderVars;

void GLResize(int width, int height) {
	glViewport(0, 0, width, height);
	if(height != 0) RV::_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);
	else RV::_projection = glm::perspective(RV::FOV, 0.f, RV::zNear, RV::zFar);
}

void GLmousecb(MouseEvent ev) {
	if(RV::prevMouse.waspressed && RV::prevMouse.button == ev.button) {
		float diffx = ev.posx - RV::prevMouse.lastx;
		float diffy = ev.posy - RV::prevMouse.lasty;
		switch(ev.button) {
		case MouseEvent::Button::Left: // ROTATE
			RV::rota[0] += diffx * 0.005f;
			RV::rota[1] += diffy * 0.005f;
			break;
		case MouseEvent::Button::Right: // MOVE XY
			RV::panv[0] += diffx * 0.03f;
			RV::panv[1] -= diffy * 0.03f;
			break;
		case MouseEvent::Button::Middle: // MOVE Z
			RV::panv[2] += diffy * 0.05f;
			break;
		default: break;
		}
	} else {
		RV::prevMouse.button = ev.button;
		RV::prevMouse.waspressed = true;
	}
	RV::prevMouse.lastx = ev.posx;
	RV::prevMouse.lasty = ev.posy;
}

void GLinit(int width, int height) {
	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	RV::_projection = glm::perspective(RV::FOV, (float)width/(float)height, RV::zNear, RV::zFar);

	//Load Cube
	bool res = loadOBJ("cube.obj", vertices, uvs, normals);
	Cube::setupModel();
	vertices.clear();
	uvs.clear();
	normals.clear();

	//Load Trump
	res = loadOBJ("trump.obj", vertices, uvs, normals);
	Trump::setupModel();
	vertices.clear();
	uvs.clear();
	normals.clear();

	//Load Chicken
	res = loadOBJ("chicken.obj", vertices, uvs, normals);
	Chicken::setupModel();
	vertices.clear();
	uvs.clear();
	normals.clear();

	//Load Wheel
	res = loadOBJ("wheel.obj", vertices, uvs, normals);
	Wheel::setupModel();
	vertices.clear();
	uvs.clear();
	normals.clear();

	//Load Cabin
	res = loadOBJ("test_cabin_v3.obj", vertices, uvs, normals);
	Cabin::setupModel();
	vertices.clear();
	uvs.clear();
	normals.clear();

	//Load Feet Wheel
	res = loadOBJ("feet.obj", vertices, uvs, normals);
	Feet::setupModel();
	vertices.clear();
	uvs.clear();
	normals.clear();

	sunPos =  glm::vec3(40, 40, 0);
	lightPos = sunPos;
	moonPos -= sunPos;

	Sphere::setupSphere(sunPos, 1.0f);
}

void GLcleanup() {
	Cube::cleanupModel();
	Trump::cleanupModel();
	Chicken::cleanupModel();
	Wheel::cleanupModel();
	Cabin::cleanupModel();
	Sphere::cleanupSphere();
	Feet::cleanupModel();
}
int timer=0;
void GLrender(double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RV::_modelView = glm::mat4(1.f);	
	//Default Camara
	if (camara == 0)
	{
		RV::_modelView = glm::translate(RV::_modelView, glm::vec3(0.0f, -20.0f, -70.0f));
		RV::_modelView = glm::rotate(RV::_modelView, glm::radians(60.0f), glm::vec3(0.f, 1.f, 0.f));
	}

	//Shot-Countershot Camara
	else if (camara==1)
	{
		timer += currentTime;
		if (timer / 100 > 4)
		{
			timer = 0;
			focusTrump = !focusTrump;
		}

		glm::vec3 camaraTrumpChicken;
		if (focusTrump)
		{
			camaraTrumpChicken = glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin) - 1, distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin) + 20, 0);
			RV::_modelView = glm::lookAt(camaraTrumpChicken, glm::vec3(camaraTrumpChicken.x + 1, camaraTrumpChicken.y, camaraTrumpChicken.z), glm::vec3(0.0, 1.0, 0.0));
		}
		else
		{
			camaraTrumpChicken = glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin) + 1.5, distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin) + 19, 0);
			RV::_modelView = glm::lookAt(camaraTrumpChicken, glm::vec3(camaraTrumpChicken.x - 1, camaraTrumpChicken.y, camaraTrumpChicken.z), glm::vec3(0.0, 1.0, 0.0));
		}
	}

	//Lateral Camara
	else if (camara == 2)
	{
		glm::vec3 focusCabin= glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin), distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin) + 20, 0);
		RV::_modelView = glm::lookAt(glm::vec3(focusCabin.x,focusCabin.y, focusCabin.z-10), focusCabin, glm::vec3(0.0, 1.0, 0.0));
	}

	//Rotating God's Eye Camara - to do
	else if (camara == 3)
	{
		glm::mat4 matrix;
		glm::vec3 focusCabin = glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin), distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin) + 20, 0);
		glm::vec3 focusCamara = glm::vec3(focusCabin.x, focusCabin.y-10, focusCabin.z);
		focusCabin = glm::vec3(focusCabin.x, focusCabin.y+0.5, focusCabin.z);
		matrix = glm::translate(matrix, focusCabin);
		matrix = glm::rotate(matrix, (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
		glm::vec4 aux = glm::vec4(1.0);
		aux = matrix * aux;
		RV::_modelView = glm::lookAt(glm::vec3(aux.x,aux.y,aux.z), focusCamara, glm::vec3(0.0, 1.0, 0.0));
	}
	
	RV::_MVP = RV::_projection * RV::_modelView;


	//Sun Sphere
	if (light_moves)
	{
		sunPos = glm::vec3(1.5*distanceCenter*cos((float)currentTime*speedMultiplayer*0.5), (1.5*distanceCenter*sin((float)currentTime*speedMultiplayer*0.5)) + 20, 10);
	}
	Sphere::updateSphere(sunPos, 1.0f);
	Sphere::drawSphere();

	//Moon Sphere
	if (light_moves)
	{
		moonPos = glm::vec3((-1.5*distanceCenter)*cos((float)currentTime *(speedMultiplayer*0.5)-distanceCenter), ((-1.5*distanceCenter)*sin((float)currentTime *(speedMultiplayer*0.5) - distanceCenter)) + 20, 10);
	}
	Sphere::updateSphere(moonPos, 1.0f);
	Sphere::drawSphere2();

	//LightPos - Postion of the light
	//Day
	if (sunPos.y >= 15+centerScene.y)
	{
		lightPos = sunPos;
		lightColor = glm::vec3(0.7f, 0.7f, 0.3f);
	}

	else if (sunPos.y >= centerScene.y)
	{
		lightPos = sunPos;
		lightColor = glm::vec3(0.5f, 0.1f, 0.1f);
	}

	//Night
	else
	{
		lightPos = moonPos;
		lightColor = glm::vec3(0.1f, 0.1f, 0.5f);
	}

	//Transform and Draw Objects
	glm::mat4 model;
	if (exercise == 1)
	{
		if (!modelActivated)
		{
			//Cube Trump
			model = glm::mat4(1.0);
			model = glm::translate(model, centerScene);
			model = glm::translate(model, glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin) + 1, distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin) + 3,0));
			model = glm::scale(model, glm::vec3(0.7f));
			Cube::updateModel(model);
			Cube::drawModel();

			//Cube Chicken
			model = glm::mat4(1.0);
			model = glm::translate(model, centerScene);
			model = glm::translate(model, glm::vec3(glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin) - 1, distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin) + 3, 0)));
			model = glm::scale(model, glm::vec3(0.7f));
			Cube::updateModel(model);
			Cube::drawModel();

			//Cube Cabins
			for (int i = 0; i < 20; i++)
			{
				model = glm::mat4(1.0);
				model = glm::translate(model, centerScene);
				model = glm::translate(model, glm::vec3(glm::vec3(distanceCenter*cos((float)(currentTime*speedMultiplayer + distanceCabin*i)), distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin*i), 0)));
				model = glm::scale(model, glm::vec3(2.0f));
				Cube::updateModel(model);
				Cube::drawModel();
			}
		}
		else
		{
			//Trump
			model = glm::mat4(1.0);
			model = glm::translate(model, centerScene);
			model = glm::translate(model, glm::vec3(glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin) + 1, distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin) - 2.5, 0)));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.015f));
			Trump::updateModel(model);
			Trump::drawModel();

			//Chicken
			model = glm::mat4(1.0);
			model = glm::translate(model, centerScene);
			model = glm::translate(model, glm::vec3(glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin) - 0.5, distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin)-2, 0)));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.02f));
			Chicken::updateModel(model);
			Chicken::drawModel();

			//Wheel
			model = glm::mat4(1.0);
			model = glm::translate(model, centerScene);
			model = glm::rotate(model, (float)currentTime*speedMultiplayer, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(0.004f));
			Wheel::updateModel(model);
			Wheel::drawModel();

			//Feet Wheel
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.005f));
			Feet::updateModel(model);
			Feet::drawModel();

			for (int i = 0; i < 20; i++)
			{
				model = glm::mat4(1.0);
				model = glm::translate(model, centerScene);
				model = glm::translate(model, glm::vec3(glm::vec3(distanceCenter*cos((float)(currentTime*speedMultiplayer + distanceCabin*i)), distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin*i), 0)));
				model = glm::scale(model, glm::vec3(0.005f));
				Cabin::updateModel(model);
				Cabin::drawModel();
			}
		}
	}
	else if (exercise == 2)
	{
		if (!modelActivated)
		{
			//Cube Trump
			model = glm::mat4(1.0);
			model = glm::translate(model, centerScene);
			model = glm::translate(model, glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin) + 1, distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin) + 3, +1));
			model = glm::scale(model, glm::vec3(0.7f));
			Cube::updateModel(model);
			Cube::drawModel();

			//Cube Chicken
			model = glm::mat4(1.0);
			model = glm::translate(model, centerScene);
			model = glm::translate(model, glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin) - 1, distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin) + 3, +1));
			model = glm::scale(model, glm::vec3(0.7f));
			Cube::updateModel(model);
			Cube::drawModel();

			//Cube Cabins
			for (int i = 0; i < 20; i++)
			{
				model = glm::mat4(1.0);
				model = glm::translate(model, centerScene);
				model = glm::translate(model, glm::vec3(distanceCenter*cos((float)(currentTime*speedMultiplayer + distanceCabin*i)), distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin*i), 0));
				model = glm::scale(model, glm::vec3(2.0f));
				Cube::updateModel(model);
				Cube::drawModel();
			}
		}
		else
		{
			//Trump
			model = glm::mat4(1.0);
			model = glm::translate(model, centerScene);
			model = glm::translate(model, glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin) + 1.5, distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin)-3, -1));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.02f));
			Trump::updateModel(model);
			Trump::drawModel();

			//Chicken
			model = glm::mat4(1.0);
			model = glm::translate(model, centerScene);
			model = glm::translate(model, glm::vec3(distanceCenter*cos((float)currentTime*speedMultiplayer + distanceCabin) - 1, distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin) -3, -1));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.04f));
			Chicken::updateModel(model);
			Chicken::drawModel();

			//Wheel
			model = glm::mat4(1.0);
			model = glm::translate(model, centerScene);
			model = glm::rotate(model, (float)currentTime*speedMultiplayer, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(0.004f));
			Wheel::updateModel(model);
			Wheel::drawModel();

			//Feet Wheel
			model = glm::mat4(1.0);
			model = glm::translate(model, centerScene);
			model = glm::rotate(model, (float)currentTime*speedMultiplayer, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(0.004f));
			Feet::updateModel(model);
			Feet::drawModel();

			for (int i = 0; i < 20; i++)
			{
				model = glm::mat4(1.0);
				model = glm::translate(model, centerScene);
				model = glm::translate(model, glm::vec3(distanceCenter*cos((float)(currentTime*speedMultiplayer + distanceCabin*i)), distanceCenter *sin((float)currentTime*speedMultiplayer + distanceCabin*i), 0));
				model = glm::scale(model, glm::vec3(0.005f));
				Cabin::updateModel(model);
				Cabin::drawModel();
			}
		}
		
	}
	ImGui::Render();
}


//////////////////////////////////// COMPILE AND LINK
GLuint compileShader(const char* shaderStr, GLenum shaderType, const char* name="") {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderStr, NULL);
	glCompileShader(shader);
	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &res);
		char *buff = new char[res];
		glGetShaderInfoLog(shader, res, &res, buff);
		fprintf(stderr, "Error Shader %s: %s", name, buff);
		delete[] buff;
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}
void linkProgram(GLuint program) {
	glLinkProgram(program);
	GLint res;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &res);
		char *buff = new char[res];
		glGetProgramInfoLog(program, res, &res, buff);
		fprintf(stderr, "Error Link: %s", buff);
		delete[] buff;
	}
}

////////////////////////////////////////////////// SPHERE
namespace Sphere {
GLuint sphereVao;
GLuint sphereVbo;
GLuint sphereShaders[3];
GLuint sphereProgram;
float radius;

const char* sphere_vertShader =
"#version 330\n\
in vec3 in_Position;\n\
uniform mat4 mv_Mat;\n\
void main() {\n\
	gl_Position = mv_Mat * vec4(in_Position, 1.0);\n\
}";
const char* sphere_geomShader =
"#version 330\n\
layout(points) in;\n\
layout(triangle_strip, max_vertices = 4) out;\n\
out vec4 eyePos;\n\
out vec4 centerEyePos;\n\
uniform mat4 projMat;\n\
uniform float radius;\n\
vec4 nu_verts[4];\n\
void main() {\n\
	vec3 n = normalize(-gl_in[0].gl_Position.xyz);\n\
	vec3 up = vec3(0.0, 1.0, 0.0);\n\
	vec3 u = normalize(cross(up, n));\n\
	vec3 v = normalize(cross(n, u));\n\
	nu_verts[0] = vec4(-radius*u - radius*v, 0.0); \n\
	nu_verts[1] = vec4( radius*u - radius*v, 0.0); \n\
	nu_verts[2] = vec4(-radius*u + radius*v, 0.0); \n\
	nu_verts[3] = vec4( radius*u + radius*v, 0.0); \n\
	centerEyePos = gl_in[0].gl_Position;\n\
	for (int i = 0; i < 4; ++i) {\n\
		eyePos = (gl_in[0].gl_Position + nu_verts[i]);\n\
		gl_Position = projMat * eyePos;\n\
		EmitVertex();\n\
	}\n\
	EndPrimitive();\n\
}";
const char* sphere_fragShader_flatColor =
"#version 330\n\
in vec4 eyePos;\n\
in vec4 centerEyePos;\n\
out vec4 out_Color;\n\
uniform mat4 projMat;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform float radius;\n\
void main() {\n\
	vec4 diff = eyePos - centerEyePos;\n\
	float distSq2C = dot(diff, diff);\n\
	if (distSq2C > (radius*radius)) discard;\n\
	float h = sqrt(radius*radius - distSq2C);\n\
	vec4 nuEyePos = vec4(eyePos.xy, eyePos.z + h, 1.0);\n\
	vec4 nuPos = projMat * nuEyePos;\n\
	gl_FragDepth = ((nuPos.z / nuPos.w) + 1) * 0.5;\n\
	vec3 normal = normalize(nuEyePos - centerEyePos).xyz;\n\
	out_Color = vec4(color.xyz * dot(normal, (mv_Mat*vec4(0.0, 1.0, 0.0, 0.0)).xyz) + color.xyz * 0.3, 1.0 );\n\
}";

bool shadersCreated = false;
void createSphereShaderAndProgram() {
	if(shadersCreated) return;

	sphereShaders[0] = compileShader(sphere_vertShader, GL_VERTEX_SHADER, "sphereVert");
	sphereShaders[1] = compileShader(sphere_geomShader, GL_GEOMETRY_SHADER, "sphereGeom");
	sphereShaders[2] = compileShader(sphere_fragShader_flatColor, GL_FRAGMENT_SHADER, "sphereFrag");

	sphereProgram = glCreateProgram();
	glAttachShader(sphereProgram, sphereShaders[0]);
	glAttachShader(sphereProgram, sphereShaders[1]);
	glAttachShader(sphereProgram, sphereShaders[2]);
	glBindAttribLocation(sphereProgram, 0, "in_Position");
	linkProgram(sphereProgram);

	shadersCreated = true;
}
void cleanupSphereShaderAndProgram() {
	if(!shadersCreated) return;
	glDeleteProgram(sphereProgram);
	glDeleteShader(sphereShaders[0]);
	glDeleteShader(sphereShaders[1]);
	glDeleteShader(sphereShaders[2]);
	shadersCreated = false;
}

void setupSphere(glm::vec3 pos, float radius) {
	Sphere::radius = radius;
	glGenVertexArrays(1, &sphereVao);
	glBindVertexArray(sphereVao);
	glGenBuffers(1, &sphereVbo);

	glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, &pos, GL_DYNAMIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	createSphereShaderAndProgram();
}
void cleanupSphere() {
	glDeleteBuffers(1, &sphereVbo);
	glDeleteVertexArrays(1, &sphereVao);

	cleanupSphereShaderAndProgram();
}
void updateSphere(glm::vec3 pos, float radius) {
	glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
	float* buff = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	buff[0] = pos.x;
	buff[1] = pos.y;
	buff[2] = pos.z;
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	Sphere::radius = radius;
}
void drawSphere() {
	glBindVertexArray(sphereVao);
	glUseProgram(sphereProgram);
	glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RV::_MVP));
	glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RV::_modelView));
	glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "projMat"), 1, GL_FALSE, glm::value_ptr(RV::_projection));
	glUniform4f(glGetUniformLocation(sphereProgram, "color"), 0.6f, 0.6f, 0.1f, 1.f);
	glUniform1f(glGetUniformLocation(sphereProgram, "radius"), Sphere::radius);
	glDrawArrays(GL_POINTS, 0, 1);

	glUseProgram(0);
	glBindVertexArray(0);
}
void drawSphere2() {
	glBindVertexArray(sphereVao);
	glUseProgram(sphereProgram);
	glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RV::_MVP));
	glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RV::_modelView));
	glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "projMat"), 1, GL_FALSE, glm::value_ptr(RV::_projection));
	glUniform4f(glGetUniformLocation(sphereProgram, "color"), 0.1f, 0.1f, 0.5f, 1.f);
	glUniform1f(glGetUniformLocation(sphereProgram, "radius"), Sphere::radius);
	glDrawArrays(GL_POINTS, 0, 1);

	glUseProgram(0);
	glBindVertexArray(0);
}
}
/////////////// Cube /////////////////////
namespace Cube {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);


	
	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 moon;\n\
	out vec3 lDir;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
in vec3 lDir;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform vec3 lColor;\n\
void main() {\n\
    float difuse = dot(normalize(vert_Normal), mv_Mat*vec4(lDir.x, lDir.y, lDir.z, 0.0));\n\
	\n\
	//Toon Shading\n\
	//if(difuse<0.2)difuse=0.0;\n\
	//else if(difuse>=0.2 && difuse<=0.4)difuse=0.2;\n\
	//else if(difuse>=0.4 && difuse<0.5) difuse=0.4;\n\
	//else difuse=1.0;\n\
	\n\
	\n\
	out_Color = vec4(((color.xyz*difuse)+lColor*difuse)/2, 1.0 );\n\
}";
	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);
		
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);
	
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

	

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}
	void cleanupModel() {
	
		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {
	
		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), 1.0f, 1.0f, 1.0f, 0.f);
	
		glDrawArrays(GL_TRIANGLES, 0, 10000);


		glUseProgram(0);
		glBindVertexArray(0);

	}


}
////////////// Cabin /////////////////////
namespace Cabin {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);



	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 moon;\n\
	out vec3 lDir;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
in vec3 lDir;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform vec3 lColor;\n\
void main() {\n\
    float difuse = dot(normalize(vert_Normal), mv_Mat*vec4(lDir.x, lDir.y, lDir.z, 0.0));\n\
	\n\
	//Toon Shading\n\
	//if(difuse<0.2)difuse=0.0;\n\
	//else if(difuse>=0.2 && difuse<=0.4)difuse=0.2;\n\
	//else if(difuse>=0.4 && difuse<0.5) difuse=0.4;\n\
	//else difuse=1.0;\n\
	\n\
	\n\
	out_Color = vec4(((color.xyz*difuse)+lColor*difuse)/2, 1.0 );\n\
}";
	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);



		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}
	void cleanupModel() {

		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {

		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), 1.0f, 1.0f, 1.0f, 0.f);

		glDrawArrays(GL_TRIANGLES, 0, 10000);


		glUseProgram(0);
		glBindVertexArray(0);

	}


}
////////////// Trump /////////////////////
namespace Trump {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);



	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 moon;\n\
	out vec3 lDir;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
in vec3 lDir;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform vec3 lColor;\n\
void main() {\n\
    float difuse = dot(normalize(vert_Normal), mv_Mat*vec4(lDir.x, lDir.y, lDir.z, 0.0));\n\
	\n\
	//Toon Shading\n\
	//if(difuse<0.2)difuse=0.0;\n\
	//else if(difuse>=0.2 && difuse<=0.4)difuse=0.2;\n\
	//else if(difuse>=0.4 && difuse<0.5) difuse=0.4;\n\
	//else difuse=1.0;\n\
	\n\
	\n\
	out_Color = vec4(((color.xyz*difuse)+lColor*difuse)/2, 1.0 );\n\
}";
	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);



		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}
	void cleanupModel() {

		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {

		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), 1.0f, 1.0f, 1.0f, 0.f);

		glDrawArrays(GL_TRIANGLES, 0, 10000);


		glUseProgram(0);
		glBindVertexArray(0);

	}


}
////////////// Chicken /////////////////////
namespace Chicken {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);



	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 moon;\n\
	out vec3 lDir;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
in vec3 lDir;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform vec3 lColor;\n\
void main() {\n\
    float difuse = dot(normalize(vert_Normal), mv_Mat*vec4(lDir.x, lDir.y, lDir.z, 0.0));\n\
	\n\
	//Toon Shading\n\
	//if(difuse<0.2)difuse=0.0;\n\
	//else if(difuse>=0.2 && difuse<=0.4)difuse=0.2;\n\
	//else if(difuse>=0.4 && difuse<0.5) difuse=0.4;\n\
	//else difuse=1.0;\n\
	\n\
	\n\
	out_Color = vec4(((color.xyz*difuse)+lColor*difuse)/2, 1.0 );\n\
}";
	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);



		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}
	void cleanupModel() {

		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {

		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), 1.0f, 1.0f, 1.0f, 0.f);

		glDrawArrays(GL_TRIANGLES, 0, 100000);


		glUseProgram(0);
		glBindVertexArray(0);

	}


}
////////////// Wheel /////////////////////
namespace Wheel {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);



	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 moon;\n\
	out vec3 lDir;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
in vec3 lDir;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform vec3 lColor;\n\
void main() {\n\
    float difuse = dot(normalize(vert_Normal), mv_Mat*vec4(lDir.x, lDir.y, lDir.z, 0.0));\n\
	\n\
	//Toon Shading\n\
	//if(difuse<0.2)difuse=0.0;\n\
	//else if(difuse>=0.2 && difuse<=0.4)difuse=0.2;\n\
	//else if(difuse>=0.4 && difuse<0.5) difuse=0.4;\n\
	//else difuse=1.0;\n\
	\n\
	\n\
	out_Color = vec4(((color.xyz*difuse)+lColor*difuse)/2, 1.0 );\n\
}";
	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);



		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}
	void cleanupModel() {

		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {

		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), 1.0f, 1.0f, 1.0f, 0.f);

		glDrawArrays(GL_TRIANGLES, 0, 100000);


		glUseProgram(0);
		glBindVertexArray(0);

	}


}

////////////// Feet Whell /////////////////////
namespace Feet {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);



	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 moon;\n\
	out vec3 lDir;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
in vec3 lDir;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform vec3 lColor;\n\
void main() {\n\
    float difuse = dot(normalize(vert_Normal), mv_Mat*vec4(lDir.x, lDir.y, lDir.z, 0.0));\n\
	\n\
	//Toon Shading\n\
	//if(difuse<0.2)difuse=0.0;\n\
	//else if(difuse>=0.2 && difuse<=0.4)difuse=0.2;\n\
	//else if(difuse>=0.4 && difuse<0.5) difuse=0.4;\n\
	//else difuse=1.0;\n\
	\n\
	\n\
	out_Color = vec4(((color.xyz*difuse)+lColor*difuse)/2, 1.0 );\n\
}";
	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);



		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}
	void cleanupModel() {

		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {

		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), 1.0f, 1.0f, 1.0f, 0.f);

		glDrawArrays(GL_TRIANGLES, 0, 100000);


		glUseProgram(0);
		glBindVertexArray(0);

	}


}