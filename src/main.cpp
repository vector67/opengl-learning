#include "../include/glad/glad.h"
#include "../include/stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unistd.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;


int main() {
    stbi_set_flip_vertically_on_load(true);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWmonitor *primary = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(primary);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    std::cout << mode->refreshRate << std::endl;
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "My Title", nullptr, NULL);

//    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", primary, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSwapInterval(-10);
    glEnable(GL_DEPTH_TEST);

    Assimp::Importer importer;
    std::string path = "";
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);


    unsigned int diffuseMap = loadTexture("assets/container2.png");
    unsigned int specularMap = loadTexture("assets/container2_specular.png");
    Shader lightingShader("lighting.vertex.glsl", "lighting.fragment.glsl");
    lightingShader.use();
    lightingShader.setInt("material.diffuse",  0);
    lightingShader.setInt("material.specular",  1);
    lightingShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    Shader lightCubeShader("lightCube.vertex.glsl", "lightCube.fragment.glsl");

    unsigned int frameTotal = 0;
    float l = 0;
    int targetFrameRate = 30;

    glm::vec3 pointLightPositions[] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    Model ourModel("assets/backpack/backpack.obj");
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
        glBindTexture(GL_TEXTURE_2D, specularMap);

        lightingShader.use();
//        lightPos.x = sin(lastFrame)*3;
//        lightPos.y = cos(lastFrame)*3;
        lightingShader.setVec3("lightPos",  lightPos);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        lightingShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("view", view);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        lightingShader.setFloat("material.shininess", 32.0f);

        glm::vec3 diffuseColor = glm::vec3(1.0, 1.0, 1.0);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
        lightingShader.setVec3("dirLight.ambient",  ambientColor);
        lightingShader.setVec3("dirLight.diffuse",  diffuseColor);
        lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("dirLight.direction", glm::vec3(-0.2, -0.3, 0.0));
        lightingShader.setVec3("dirLight.direction", glm::vec3(-0.2, -0.3, 0.0));

        lightingShader.setVec3("spotLight.ambient",  ambientColor);
        lightingShader.setVec3("spotLight.diffuse",  diffuseColor);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

        lightingShader.setVec3("spotLight.position",  camera.Position);
        lightingShader.setVec3("spotLight.position",  camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setFloat("spotLight.cutOff",   glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff",   glm::cos(glm::radians(17.5f)));

        for (int i = 0; i < 4; i++) {
            char lightPropertyString[23]; // enough to hold all numbers up to 64-bits
            sprintf(lightPropertyString, "pointLights[%d].%s", i, "position");
            lightingShader.setVec3(lightPropertyString, pointLightPositions[i]);
            sprintf(lightPropertyString, "pointLights[%d].%s", i, "constant");
            lightingShader.setFloat(lightPropertyString, 1.0f);
            sprintf(lightPropertyString, "pointLights[%d].%s", i, "linear");
            lightingShader.setFloat(lightPropertyString, 0.20f);
            sprintf(lightPropertyString, "pointLights[%d].%s", i, "quadratic");
            lightingShader.setFloat(lightPropertyString, 0.22f);
            sprintf(lightPropertyString, "pointLights[%d].%s", i, "ambient");
            lightingShader.setVec3(lightPropertyString, glm::vec3(1.0, 1.0, 1.0));
            sprintf(lightPropertyString, "pointLights[%d].%s", i, "diffuse");
            lightingShader.setVec3(lightPropertyString, glm::vec3(1.0, 1.0, 1.0));
            sprintf(lightPropertyString, "pointLights[%d].%s", i, "specular");
            lightingShader.setVec3(lightPropertyString, glm::vec3(1.0, 1.0, 1.0));

        }
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        ourModel.Draw(lightingShader);


        lightCubeShader.use();
        for (int i = 0; i < 4; i++) {

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.02f));
            lightCubeShader.setMat4("projection", projection);
            lightCubeShader.setMat4("view", view);
            lightCubeShader.setMat4("model", model);
            lightCubeShader.setVec3("cubeColor", glm::vec3(0, 1.0, 0));
//            glBindVertexArray(lightCubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        double wait_time = 1.0 / (targetFrameRate);
        double curr_frame_time = glfwGetTime() - lastFrame;
        double dur = 1000.0 * ( wait_time - curr_frame_time );
        if( dur > 0 ) // ensures that we don't have a dur > 0.0 which converts to a durDW of 0.
        {
            usleep((int)dur*1000);
        }
        if ((int) glfwGetTime() != (int) lastFrame) {
            std::cout << "FPS: " << frameTotal << "   " << currentFrame << std::endl;
            frameTotal = 0;
        }
        frameTotal += 1;
    }


    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
