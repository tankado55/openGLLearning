#include "TestSmoke.h"
#include "Renderer.h"
#include "imgui.h"
#include "glm/glm.hpp"
#include "InputManager.h"
#include "TimeUpdater.h"



#include <glm/gtc/matrix_transform.hpp>
#include <cmath>



glm::vec3 rayPlaneIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 planePoint, glm::vec3 planeNormal) {
    float denominator = glm::dot(planeNormal, rayDirection);

    if (denominator == 0) {
        // Ray is parallel to the plane
        return { 0, 0, 0 };
    }

    float t = ((planePoint.x - rayOrigin.x) * planeNormal.x +
        (planePoint.y - rayOrigin.y) * planeNormal.y +
        (planePoint.z - rayOrigin.z) * planeNormal.z) / denominator;

    glm::vec3 intersectionPoint;
    intersectionPoint.x = rayOrigin.x + t * rayDirection.x;
    intersectionPoint.y = rayOrigin.y + t * rayDirection.y;
    intersectionPoint.z = rayOrigin.z + t * rayDirection.z;

    return intersectionPoint;
}



Test::TestSmoke::TestSmoke() :
    m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)),
    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0.0))),
    m_TranslationA(glm::vec3(0, 0, 0)),
    m_TextureGridMode(glm::vec3(1.2f, 1.0f, 2.0f)),
    m_XCount(30),
    m_YCount(10),
    m_ZCount(30),
    m_Distance(0.5f)
{
    m_Plane = std::make_unique<Model>("res/models/plane/plane.obj");
    m_PrototypeTexture = std::make_unique<Texture>("res/textures/tex_3.png");
    m_Plane->AddTexture(*m_PrototypeTexture, "texture_diffuse", 0);

    // Obstacle1
    m_Obstacle = std::make_unique<Model>("res/models/cube/cube.obj");
    m_ObstacleTexture = std::make_unique<Texture>("res/textures/tex_6.png");
    m_Obstacle->AddTexture(*m_ObstacleTexture, "texture_diffuse", 0);
    m_Obstacle->modelMatrix = glm::translate(m_Obstacle->modelMatrix, glm::vec3(0.0, 0.5, 0.0));
    m_Obstacle->modelMatrix = glm::scale(m_Obstacle->modelMatrix, glm::vec3(5.99, 2.99, 0.99));
    // Obstacle2
    m_Obstacle2 = std::make_unique<Model>("res/models/cube/cube.obj");
    m_Obstacle2->AddTexture(*m_ObstacleTexture, "texture_diffuse", 0);
    m_Obstacle2->modelMatrix = glm::translate(m_Obstacle2->modelMatrix, glm::vec3(0.0, 0.5, 3.0));
    m_Obstacle2->modelMatrix = glm::scale(m_Obstacle2->modelMatrix, glm::vec3(5.99, 0.99, 0.99));

    std::vector<Model*> sceneModels;
    sceneModels.push_back(m_Obstacle.get());
    sceneModels.push_back(m_Obstacle2.get());

    float positions[] = {
        // pos, norma, uv
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

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
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f
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

    m_VoxelGrid = std::make_unique<VoxelGrid>();

    m_SmokeShader = std::make_unique<Shader>("res/shaders/SmokeShader.hlsl");
    m_VoxelDebugShader = std::make_unique<Shader>("res/shaders/voxelDebugShader.hlsl");
    m_VoxelDebugShader->Bind();
    m_VoxelDebugShader->SetUniform1i("u_XCount", m_VoxelGrid->resolution.x);
    m_VoxelDebugShader->SetUniform1i("u_YCount", m_VoxelGrid->resolution.y);
    m_VoxelDebugShader->SetUniform1i("u_ZCount", m_VoxelGrid->resolution.z);
    m_PlaneShader = std::make_unique<Shader>("res/shaders/BasicPlaneShader.hlsl");
    m_SmokeShader->Bind();

    m_SmokeShader->SetUniform1i("u_XCount", m_VoxelGrid->resolution.x);
    m_SmokeShader->SetUniform1i("u_YCount", m_VoxelGrid->resolution.y);
    m_SmokeShader->SetUniform1i("u_ZCount", m_VoxelGrid->resolution.z);
    m_VoxelGrid->Bake(sceneModels);

    m_WhiteShader = std::make_unique<Shader>("res/shaders/WhiteSingleShader.hlsl");

    m_Camera = std::make_unique<Camera>();
    InputManager::GetInstance()->Start(m_Camera.get());
    m_Smoke = std::make_unique<SmokeGrenade>();

    //glEnable(GL_CULL_FACE);
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

    UpdateInputs(Timem::deltaTime);

    glm::vec3 planeNormal = { 0, 1, 0 };
    glm::vec3 pointOnPlane = { 1, 0, 1 };
    glm::vec3 cameraFront = m_Camera->GetFront();
    glm::vec3 cameraPosition = m_Camera->GetPos();

    glm::vec3 intersectInPlane = rayPlaneIntersection(cameraPosition, cameraFront, pointOnPlane, planeNormal);

    { // Plane
        glm::mat4 model = glm::mat4(1.0f);
        m_PlaneShader->Bind();
        m_PlaneShader->SetUniformMat4f("u_View", m_View);
        m_PlaneShader->SetUniformMat4f("u_Projection", m_Proj);
        m_PlaneShader->SetUniformMat4f("u_Model", model);

        m_Plane->Draw(*m_PlaneShader);
    }

    { // intersection cube
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, intersectInPlane);
        //model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
        m_WhiteShader->Bind(); // it is done also in renderer.draw but it is necessary here to set the uniform
        m_WhiteShader->SetUniformMat4f("u_View", m_View);
        m_WhiteShader->SetUniformMat4f("u_Projection", m_Proj);
        m_WhiteShader->SetUniformMat4f("u_Model", model);

        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_WhiteShader);
    }
    
 
    { // cube in the origin
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_TranslationA);
        m_WhiteShader->Bind(); // it is done also in renderer.draw but it is necessary here to set the uniform
        m_WhiteShader->SetUniformMat4f("u_View", m_View);
        m_WhiteShader->SetUniformMat4f("u_Projection", m_Proj);
        m_WhiteShader->SetUniformMat4f("u_Model", model);

        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_WhiteShader);
    }

    { // obstacles
        m_PlaneShader->Bind(); // it is done also in renderer.draw but it is necessary here to set the uniform
        m_PlaneShader->SetUniformMat4f("u_View", m_View);
        m_PlaneShader->SetUniformMat4f("u_Projection", m_Proj);
        m_PlaneShader->SetUniformMat4f("u_Model", m_Obstacle->GetModelMatrix());
        m_Obstacle->Draw(*m_PlaneShader);
        m_PlaneShader->SetUniformMat4f("u_Model", m_Obstacle2->GetModelMatrix());
        m_Obstacle2->Draw(*m_PlaneShader);
        
    }
    
    //{ //smoke
    //    double toPass = Timem::deltaTime;
    //    m_Smoke->Update(toPass);
    //    m_Smoke->Draw(*m_SmokeShader); // it only set the uniforms
    //    glm::mat4 model = glm::mat4(1.0);
    //    model = glm::scale(model, glm::vec3(m_VoxelGrid->voxelSize, m_VoxelGrid->voxelSize, m_VoxelGrid->voxelSize));
    //    model = model * m_VoxelGrid->modelMatrix;
    //    m_SmokeShader->Bind(); // it is done also in renderer.draw but it is necessary here to set the uniform
    //    m_SmokeShader->SetUniformMat4f("u_Model", model);
    //    m_SmokeShader->SetUniformMat4f("u_View", m_View);
    //    m_SmokeShader->SetUniformMat4f("u_Projection", m_Proj);
    //    m_SmokeShader->SetUniformVec3f("explosionPos", intersectInPlane);
    //    
    //
    //    renderer.DrawInstanced(
    //        *m_VAO,
    //        *m_IndexBuffer,
    //        *m_SmokeShader,
    //        m_VoxelGrid->voxelCount
    //    );
    //}

    { //voxel debugging
        m_VoxelGrid->Draw(*m_VoxelDebugShader); // it only set the uniforms
        glm::mat4 model = glm::mat4(1.0);
        model = glm::scale(model, glm::vec3(m_VoxelGrid->voxelSize));
        model = model * m_VoxelGrid->modelMatrix;
        m_VoxelDebugShader->Bind(); // it is done also in renderer.draw but it is necessary here to set the uniform
        m_VoxelDebugShader->SetUniformMat4f("u_Model", model);
        m_VoxelDebugShader->SetUniformMat4f("u_View", m_View);
        m_VoxelDebugShader->SetUniformMat4f("u_Projection", m_Proj);

        renderer.DrawInstanced(
            *m_VAO,
            *m_IndexBuffer,
            *m_VoxelDebugShader,
            m_VoxelGrid->voxelCount
        );
    }
    
    
}

void Test::TestSmoke::OnImGuiRenderer()
{
    ImGui::SliderFloat3("Translation A", &m_TranslationA.x, -960.0f/2.0f, 960.0f/2.0f);
    ImGui::SliderFloat3("Translation B", &m_TextureGridMode.x, -10.0f, 10.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void Test::TestSmoke::UpdateInputs(const double& deltaTime)
{
    GLFWwindow* window = InputManager::GetInstance()->GetWindow();

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
    {
        m_PrevLeftButtonState = m_LeftButtonState;
        m_LeftButtonState = true;
        if (m_PrevLeftButtonState == false && m_LeftButtonState == true)
        {
            glm::vec3 planeNormal = { 0, 1, 0 };
            glm::vec3 pointOnPlane = { 1, 0, 1 };
            glm::vec3 cameraFront = m_Camera->GetFront();
            glm::vec3 cameraPosition = m_Camera->GetPos();

            glm::vec3 intersectInPlane = rayPlaneIntersection(cameraPosition, cameraFront, pointOnPlane, planeNormal);
            m_VoxelGrid->ClearStatus();
            m_VoxelGrid->Flood(intersectInPlane, 5.0);
        }
    }
    else
    {
        m_PrevLeftButtonState = m_LeftButtonState;
        m_LeftButtonState = false;
    }
}
