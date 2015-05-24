#define GLFW_DLL
#define GLFW_INCLUDE_GLU
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")

// ========== 전역 그래픽스 오브젝트 ==========
// 셰이더 프로그램 오브젝트
GLuint gShaderProgram;

// 정점 버퍼 오브젝트
GLuint gVertexBufferObject;
std::vector<glm::vec3> gVertices;

// ========== 함수 선언 ==========
const std::string ReadStringFromFile(const std::string& filename);
GLuint CreateShader(GLenum shaderType, const std::string& source);
bool CheckShader(GLuint shader);
bool CheckProgram(GLuint program);

bool InitApp();
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void WindowSizeChangeCallback(GLFWwindow* window, int width, int height);
void Render();


int main() {
	// GLFW 윈도우 핸들러
	GLFWwindow* window = NULL;

	// 에러 핸들러 등록
	glfwSetErrorCallback(ErrorCallback);

	// GLFW 초기화
	if (!glfwInit())
		exit(EXIT_FAILURE);

	// 윈도우 생성
	window = glfwCreateWindow(640, 480, "Tutorial 01: Basic OpenGL", NULL, NULL);
	if (!window) {
		// 만약 윈도우 생성에 실패했다면 어플리케이션 종료
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// 컨텍스트 생성
	glfwMakeContextCurrent(window);

	// swap 간격 조정
	glfwSwapInterval(1);

	// 키 핸들러 등록
	glfwSetKeyCallback(window, KeyCallback);
	// 윈도우 사이즈 변경 핸들러 등록
	glfwSetWindowSizeCallback(window, WindowSizeChangeCallback);

	// GLEW 초기화
	glewInit();

	// 어플리케이션의 초기화
	if (!InitApp()) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// 주 렌더링 루프. 윈도우가 종료되기 전까지 반복한다.
	while (!glfwWindowShouldClose(window)) {
		// 렌더링
		Render();

		// 렌더 버퍼 교체. (그린 결과를 디스플레이하는 명령)
		glfwSwapBuffers(window);

		// 윈도우 이벤트 (키 스트로크 등) 폴링.
		glfwPollEvents();
	}

	// 윈도우 제거
	glfwDestroyWindow(window);

	// GLFW 종료
	glfwTerminate();

	return 0;
}


bool InitApp() {
	// ---------- OpenGL 설정 초기화 ----------
	// 클리어 색상(배경색) 지정
	glClearColor(1.f, 1.f, 1.f, 1.f);

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);


	// ---------- 셰이더 생성 및 컴파일 ----------
	// 셰이더 파일 읽기
	std::string vertShaderSource = ReadStringFromFile("BasicShader.glvs");
	std::string fragShaderSource = ReadStringFromFile("BasicShader.glfs");

	// 셰이더 오브젝트 생성
	GLuint vertShaderObj = CreateShader(GL_VERTEX_SHADER, vertShaderSource);
	GLuint fragShaderObj = CreateShader(GL_FRAGMENT_SHADER, fragShaderSource);

	// 셰이더 프로그램 오브젝트 생성
	gShaderProgram = glCreateProgram();

	// 셰이더 프로그램에 버텍스 및 프래그먼트 셰이더 등록
	glAttachShader(gShaderProgram, vertShaderObj);
	glAttachShader(gShaderProgram, fragShaderObj);

	// 셰이더 프로그램과 셰이더 링킹(일종의 컴파일) 그리고 확인
	glLinkProgram(gShaderProgram);
	if (!CheckProgram(gShaderProgram)) {
		glDeleteProgram(gShaderProgram);
		return false;
	}

	// 사용된 셰이더 떼어냄
	glDetachShader(gShaderProgram, vertShaderObj);
	glDetachShader(gShaderProgram, fragShaderObj);

	// 셰이더 삭제
	glDeleteShader(vertShaderObj);
	glDeleteShader(fragShaderObj);


	// ---------- 정점 어트리뷰트 버퍼 생성 ----------
	// 정점 정의
	gVertices = {
		glm::vec3(-0.5f, -0.5f, 0.f), glm::vec3(0.5f, -0.5f, 0.f), glm::vec3(0.5f, 0.5f, 0.f)
	};

	// 정점 버퍼 생성
	glGenBuffers(1, &gVertexBufferObject);

	// 정점 버퍼 바인딩 및 데이터 등록
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, gVertices.size()*sizeof(glm::vec3), &gVertices[0], GL_STATIC_DRAW);

	return true;
}

const std::string ReadStringFromFile(const std::string& filename) {
	std::ifstream f(filename);

	if (!f.is_open())
		return "";

	return std::string(std::istreambuf_iterator<char>(f),
		std::istreambuf_iterator<char>());
}

GLuint CreateShader(GLenum shaderType, const std::string& source) {
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0)
		return 0;

	// set shader source
	const char* raw_str = source.c_str();
	glShaderSource(shader, 1, &raw_str, NULL);

	// compile shader object
	glCompileShader(shader);

	// check compilation error
	if (!CheckShader(shader)){
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

bool CheckShader(GLuint shader) {
	GLint state;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
	if (GL_FALSE == state){
		int infologLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 1){
			int charsWritten = 0;
			char *infoLog = new char[infologLength];
			glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
			std::cout << infoLog << std::endl;
			delete[] infoLog;
		}
		return false;
	}
	return true;
}

bool CheckProgram(GLuint program) {
	GLint state;
	glGetProgramiv(program, GL_LINK_STATUS, &state);
	if (GL_FALSE == state) {
		int infologLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 1){
			int charsWritten = 0;
			char *infoLog = new char[infologLength];
			glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
			std::cout << infoLog << std::endl;
			delete[] infoLog;
		}
		return false;
	}
	return true;
}

void ErrorCallback(int error, const char* description) {
	std::cout << description << std::endl;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void WindowSizeChangeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Render() {
	// 버퍼 지우기
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// gShaderProgram의 셰이더 프로그램을 사용할것임
	glUseProgram(gShaderProgram);

	// 버퍼를 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
	// location 0 정점 속성을 활성화 (셰이더 코드 참조)
	glEnableVertexAttribArray(0);
	// location 0 정점 속성의 포인터
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, NULL);

	// 정점을 그림
	glDrawArrays(GL_TRIANGLES, 0, gVertices.size());

	// 바인딩 해제
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// 활성화된 정점 속성 비활성화
	glDisableVertexAttribArray(0);

	// 셰이더 프로그램 사용 중지
	glUseProgram(0);
}