#pragma once

namespace Test {

	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRenderer() {}
		virtual void OnImGuiRenderer() {}
	};
}