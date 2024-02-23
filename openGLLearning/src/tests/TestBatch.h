#pragma once

#include "Test.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"

#include <memory>

namespace Test {
	class TestBatch : public Test {
	public:
		TestBatch();
		~TestBatch();

		void OnUpdate(float deltaTime, GLFWwindow*& window) override;
		void OnRenderer() override;
		void OnImGuiRenderer() override;

	private:

		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_LightShader;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<Model> m_Soldier;
		std::unique_ptr<Shader> m_WhiteShader;


		std::unique_ptr<Camera> m_Camera;

		glm::mat4 m_Proj, m_View;
		glm::vec3 m_TextureColorMode;
		glm::vec3 m_TextureGridMode;

		int m_RowCount;
		int m_ColCount;
		float m_Distance;
	};
}