#define GLFW_DLL
#define GLFW_INCLUDE_GLU
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")

void InitApp() {
	// 클리어 색상(배경색) 지정
	glClearColor(1, 1, 1, 1);
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

int main() {
	// GLFW 윈도우 핸들러
	GLFWwindow* window = NULL;

	// 에러 핸들러 등록
	glfwSetErrorCallback(ErrorCallback);

	// GLFW 초기화
	if (!glfwInit())
		exit(EXIT_FAILURE);

	// 윈도우 생성
	window = glfwCreateWindow(640, 480, "Tutorial 00: Test", NULL, NULL);
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
	InitApp();

	// 주 렌더링 루프. 윈도우가 종료되기 전까지 반복한다.
	while (!glfwWindowShouldClose(window))
	{
		// 버퍼 지우기
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 여기에 Update/Render 코드 

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