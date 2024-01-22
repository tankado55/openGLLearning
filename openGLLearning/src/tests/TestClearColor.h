#pragma once

#include "Test.h"

namespace Test {

	class TestClearColor : public Test
	{
	public:
		TestClearColor();
		~TestClearColor();

		void OnUpdate(float deltaTime, GLFWwindow*& window) override;
		void OnRenderer() override;
		void OnImGuiRenderer() override;

	private:
		float m_ClearColor[4];
	};
}