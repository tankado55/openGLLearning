#include "Test.h"
#include "imgui.h"

namespace Test {


	TestMenu::TestMenu(Test*& currentTestpointer)
		: m_CurrentTest(currentTestpointer)
	{
	}

	void TestMenu::OnImGuiRenderer()
	{
		for (auto& test : m_Tests)
		{
			if (ImGui::Button(test.first.c_str()))
			{
				m_CurrentTest = test.second();
			}
		}
		m_CurrentTest = m_Tests[0].second();
	}
}