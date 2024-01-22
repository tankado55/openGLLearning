#include "TestLight.h"

#include "Renderer.h"
#include "imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "InputManager.h"

Test::TestLight::TestLight():
    m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)),
    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -100.0))),
    m_TranslationA(glm::vec3(0, 0, 0)),
    m_TranslationB(glm::vec3(2, 0, 0))
{
    float positions[] = {
           -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
           -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
           -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
            0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
            0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
           -0.5f,  0.5f,  0.5f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
        5, 4, 6,
        6, 4, 7,
        1, 5, 6,
        6, 2, 1,
        0, 3, 7,
        7, 4, 0
    };

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); // it defines how opengl blend alpha pixels

    m_VAO = std::make_unique<VertexArray>();

    m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 8 * 5 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_VAO->AddBuffer(*m_VertexBuffer, layout);

    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 24);

    m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", 1.0f, 0.5f, 0.31f, 1.0f);
    m_LightShader = std::make_unique<Shader>("res/shaders/LightSource.hlsl");
    m_LightShader->Bind();
    m_LightShader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);

    m_Texture = std::make_unique<Texture>("res/textures/logo.png");
    //m_Shader->SetUniform1i("u_Texture", 0); // slot of the texture

    m_Camera = std::make_unique<Camera>();
    InputManager::GetInstance()->Start(m_Camera.get());
}

Test::TestLight::~TestLight()
{
}

void Test::TestLight::OnUpdate(float deltaTime, GLFWwindow*& window)
{
    m_Camera->ProcessKeyboardInput(deltaTime, window);
    //m_Camera->ProcessMouseInput();
    m_View = m_Camera->GetView();
}

void Test::TestLight::OnRenderer()
{
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    Renderer renderer; //that's ok because Rendered does not have internal state

    m_Texture->Bind(); // di default in slot 0

    {
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, m_TranslationA);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->Bind(); // it is done also in renderer.draw but it is necessary here to set the uniform
        m_Shader->SetUniformMat4f("u_MVP", mvp);

        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }
    {
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, m_TranslationB);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_LightShader->Bind(); // it is done also in renderer.draw but it is necessary here to set the uniform
        m_LightShader->SetUniformMat4f("u_MVP", mvp);

        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_LightShader);
    }
    
}

void Test::TestLight::OnImGuiRenderer()
{
    ImGui::SliderFloat3("Translation A", &m_TranslationA.x, -960.0f/2.0f, 960.0f/2.0f);
    ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0.0f, 960.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
