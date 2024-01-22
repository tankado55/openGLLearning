#pragma once

#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace Test {

	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime, GLFWwindow*& window) {}
		virtual void OnRenderer() {}
		virtual void OnImGuiRenderer() {}
	};

	class TestMenu : public Test
	{
	public:
		TestMenu(Test*& currentTestpointer);

		void OnImGuiRenderer() override;

		template<typename T>
		void RegisterTest(const std::string& name)
		{
			std::cout << "Registering test " << name << std::endl;
			m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
		}

	private:
		Test*& m_CurrentTest;
		std::vector <std::pair<std::string, std::function<Test*()>>> m_Tests;
	};
}