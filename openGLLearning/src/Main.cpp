#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>


int main(void)
{
    GLFWwindow* window;

    // Print OpenGL version
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    printf("OpenGL Version: %d.%d.%d\n", major, minor, revision);

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Set the OpenGL version and profile to use */
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "glew ERROR!!!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}