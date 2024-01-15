#pragma once

#include "Test.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

namespace Test {

	class TestTexture2D : public Test
	{
	public:
		TestTexture2D();
		~TestTexture2D();

		void OnUpdate(float deltaTime) override;
		void OnRenderer() override;
		void OnImGuiRenderer() override;

	private:
		
		VertexArray m_VAO;
		IndexBuffer m_IndexBuffer;
		Shader shader;

		glm::vec3 m_TranslationA;
		glm::vec3 m_TranslationB;

	};
}