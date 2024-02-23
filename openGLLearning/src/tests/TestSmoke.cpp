#include "TestSmoke.h"

#include "Renderer.h"
#include "imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "InputManager.h"
#include <cmath>

glm::vec3 findIntersection(glm::vec3 planeNormal, glm::vec3 pointOnPlane, glm::vec3 cameraPosition, glm::vec3 directionVector) {
    float denom = directionVector.x * planeNormal.x + directionVector.y * planeNormal.y + directionVector.z * planeNormal.z;

    float t = 0;
    if (fabs(denom) > 1e-6) {
        float t = -(cameraPosition.x * planeNormal.x + cameraPosition.y * planeNormal.y + cameraPosition.z * planeNormal.z) / denom;
    }
    else
        return glm::vec3(0.0, 0.0, 0.0);

    //float t = -(planeNormal.x * pointOnPlane.x + planeNormal.y * pointOnPlane.y + planeNormal.z * pointOnPlane.z) /
    //    (planeNormal.x * directionVector.x + planeNormal.y * directionVector.y + planeNormal.z * directionVector.z);

    glm::vec3 intersectionPoint;
    intersectionPoint.x = cameraPosition.x + (t * directionVector.x);
    intersectionPoint.y = cameraPosition.y + (t * directionVector.y);
    intersectionPoint.z = cameraPosition.z + (t * directionVector.z);
    return intersectionPoint;
}


Test::TestSmoke::TestSmoke() :
    m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)),
    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -100.0))),
    m_TextureColorMode(glm::vec3(0, 0, 0)),
    m_TextureGridMode(glm::vec3(1.2f, 1.0f, 2.0f)),
    m_XCount(30),
    m_YCount(10),
    m_ZCount(30),
    m_Distance(1.0f)
{
    float positions[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int indices[36];
    for (int i = 0; i < 36; i++)
    {
        indices[i] = i;
    }

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); // it defines how opengl blend alpha pixels

    m_VAO = std::make_unique<VertexArray>();

    m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 36 * 6 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    m_VAO->AddBuffer(*m_VertexBuffer, layout);

    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 36);

    m_Shader = std::make_unique<Shader>("res/shaders/SmokeShader.hlsl");
    m_Shader->Bind();

    m_Shader->SetUniform1i("u_XCount", m_XCount);
    m_Shader->SetUniform1i("u_YCount", m_YCount);
    m_Shader->SetUniform1i("u_ZCount", m_ZCount);
    m_Shader->SetUniform1f("u_Distance", m_Distance);

    m_WhiteShader = std::make_unique<Shader>("res/shaders/WhiteSingleShader.hlsl");

    m_Camera = std::make_unique<Camera>();
    InputManager::GetInstance()->Start(m_Camera.get());
}

Test::TestSmoke::~TestSmoke()
{
}

void Test::TestSmoke::OnUpdate(float deltaTime, GLFWwindow*& window)
{
    m_Camera->ProcessKeyboardInput(deltaTime, window);
    //m_Camera->ProcessMouseInput();
    m_View = m_Camera->GetView();
}

void Test::TestSmoke::OnRenderer()
{
    Renderer renderer; //that's ok because Rendered does not have internal state

    //m_Texture->Bind(); // di default in slot 0

    glm::vec3 planeNormal = { 0, 1, 0 };
    glm::vec3 pointOnPlane = { 1, 0, 1 };
    glm::vec3 cameraFront = m_Camera->GetFront();
    glm::vec3 cameraPosition = m_Camera->GetPos();

    glm::vec3 intersectInPlane = findIntersection(planeNormal, pointOnPlane, cameraPosition, cameraFront);

    
     {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, intersectInPlane);
        //model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
        glm::mat4 mvp = m_Proj * m_View * model;
        m_WhiteShader->Bind(); // it is done also in renderer.draw but it is necessary here to set the uniform
        m_WhiteShader->SetUniformMat4f("u_View", m_View);
        m_WhiteShader->SetUniformMat4f("u_Projection", m_Proj);
        m_WhiteShader->SetUniformMat4f("u_MVP", mvp);

        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_WhiteShader);
    }
    
 
    { // cube in the origin
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_TextureColorMode);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_WhiteShader->Bind(); // it is done also in renderer.draw but it is necessary here to set the uniform
        m_WhiteShader->SetUniformMat4f("u_MVP", mvp);

        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_WhiteShader);
    }
    /* in
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_TranslationA);
        model = glm::scale(model, glm::vec3(0.5,0.5,0.5));
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->Bind(); // it is done also in renderer.draw but it is necessary here to set the uniform
        m_Shader->SetUniformMat4f("u_Model", model);
        m_Shader->SetUniformMat4f("u_MVP", mvp);

        renderer.DrawInstanced(*m_VAO, *m_IndexBuffer, *m_Shader, m_XCount * m_YCount * m_ZCount);
    }
    */
    
}

void Test::TestSmoke::OnImGuiRenderer()
{
    ImGui::SliderFloat3("Translation A", &m_TextureColorMode.x, -960.0f/2.0f, 960.0f/2.0f);
    ImGui::SliderFloat3("Translation B", &m_TextureGridMode.x, -10.0f, 10.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
