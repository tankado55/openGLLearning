#pragma once

#include "Test.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"
#include "VoxelGrid.h"

#include <memory>

namespace Test {
	class TestSmoke : public Test {
	public:
		TestSmoke();
		~TestSmoke();

		void OnUpdate(float deltaTime, GLFWwindow*& window) override;
		void OnRenderer() override;
		void OnImGuiRenderer() override;

	private:

		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_SmokeShader;
		std::unique_ptr<Shader> m_LightShader;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<Model> m_Soldier;
		std::unique_ptr<Shader> m_WhiteShader;
		std::unique_ptr<Shader> m_PlaneShader;
		std::unique_ptr<VoxelGrid> m_VoxelGrid;
		std::unique_ptr<Model> m_Plane;
		std::unique_ptr<Texture> m_PrototypeTexture;


		std::unique_ptr<Camera> m_Camera;

		glm::mat4 m_Proj, m_View;
		glm::vec3 m_TranslationA;
		glm::vec3 m_TextureGridMode;

		int m_XCount;
		int m_YCount;
		int m_ZCount;
		float m_Distance;
	};
}