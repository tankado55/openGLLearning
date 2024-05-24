// intersection cube
{ // intersection cube
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, intersectInPlane);
    model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    m_WhiteShader->Bind(); // it is done also in renderer.draw but it is necessary here to set the uniform
    m_WhiteShader->SetUniformMat4f("u_View", m_View);
    m_WhiteShader->SetUniformMat4f("u_Projection", m_Proj);
    m_WhiteShader->SetUniformMat4f("u_Model", model);

    float fogFactor = 0.0;
    float cameraToPixelDist = glm::length(intersectInPlane - m_Camera->GetPos());
    if (cameraToPixelDist > 500) cameraToPixelDist = 500;
    glm::vec3 rayDir = glm::vec3(glm::normalize(intersectInPlane - m_Camera->GetPos()));
    float stepSize = 0.1;
    for (int i = 0; i * stepSize < cameraToPixelDist; i++)
    {
        glm::vec3 worldPointToCheck = glm::vec3(m_Camera->GetPos()) + (rayDir * glm::vec3(i * stepSize));
        glm::vec4 localOrigin = m_VoxelGrid->GetToVoxelLocal() * glm::vec4(worldPointToCheck, 1.0);
        glm::vec3 resolution = m_VoxelGrid->GetResolution();
        int index1D = int(int(localOrigin.x) + (int(localOrigin.y) * resolution.x) + (int(localOrigin.z) * resolution.x * resolution.y));

        if (index1D < m_VoxelGrid->status.size())
        {
            float texelData = m_VoxelGrid->status[index1D];
            if (texelData >= 0.99)
            {
                fogFactor = 1.00;
                break;
            }
        }
    }
    m_WhiteShader->SetUniform1f("green", fogFactor);
    renderer.Draw(*m_VAO, *m_IndexBuffer, *m_WhiteShader);
    }