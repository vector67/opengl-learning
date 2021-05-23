#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "filesystem.h"
#include "marchingCubes/CIsoSurface.h"
#include "marchingCubes/CIsoSurface.cpp"
#include "Mesh.h"
#include "textures/TextureUtility.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "random/pcg_random.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

unsigned int loadTexture(const char *path);

float scalarFieldFunction(float x, float y, float z);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = (float) SCR_WIDTH / 2.0;
float lastY = (float) SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(0.0f, 7.0f, 0.0f);

double targetFPS = 30.0;

int main() {

    int status, result, i;
    double sum;
    lua_State *L;

    /*
     * All Lua contexts are held in this structure. We work with it almost
     * all the time.
     */
    L = luaL_newstate();

    luaL_openlibs(L); /* Load Lua libraries */

    /* Load the file containing the script we are going to run */
    status = luaL_loadfile(L, "lua/script.lua");
    if (status) {
        /* If something went wrong, error message is at the top of */
        /* the stack */
        fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
        exit(1);
    }

    /*
     * Ok, now here we go: We pass data to the lua script on the stack.
     * That is, we first have to prepare Lua's virtual stack the way we
     * want the script to receive it, then ask Lua to run it.
     */
    lua_newtable(L);    /* We will pass a table */

    /*
     * To put values into the table, we first push the index, then the
     * value, and then call lua_rawset() with the index of the table in the
     * stack. Let's see why it's -3: In Lua, the value -1 always refers to
     * the top of the stack. When you create the table with lua_newtable(),
     * the table gets pushed into the top of the stack. When you push the
     * index and then the cell value, the stack looks like:
     *
     * <- [stack bottom] -- table, index, value [top]
     *
     * So the -1 will refer to the cell value, thus -3 is used to refer to
     * the table itself. Note that lua_rawset() pops the two last elements
     * of the stack, so that after it has been called, the table is at the
     * top of the stack.
     */
    for (i = 1; i <= 5; i++) {
        lua_pushnumber(L, i);   /* Push the table index */
        lua_pushnumber(L, i * 2); /* Push the cell value */
        lua_rawset(L, -3);      /* Stores the pair in the table */
    }

    /* By what name is the script going to reference our table? */
    lua_setglobal(L, "foo");

    /* Ask Lua to run our little script */
    result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result) {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        exit(1);
    }

    /* Get the returned value at the top of the stack (index -1) */
    sum = lua_tonumber(L, -1);

    printf("Script returned: %.0f\n", sum);

    lua_pop(L, 1);  /* Take the returned value out of the stack */
    lua_close(L);   /* Cya, Lua */





    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader("lighting.vertex.glsl", "lighting.fragment.glsl");
    Shader cubeShader("cube.vertex.glsl", "cube.fragment.glsl");
    Shader screenShader("screenShader.vertex.glsl", "screenShader.fragment.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
    };
    float planeVertices[] = {
            // positions          // texture Coords
            5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
            -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
            -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

            5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
            -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
            5.0f, -0.5f, -5.0f, 2.0f, 2.0f
    };
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();     //get the time...

    // Create a mesh from a scalar field
    auto *surface = new CIsoSurface<float>();
    int resolution = 100;
    float cubeSize = 2.9;
    int numCells = resolution + 1;

    float cellWidth = cubeSize / float(numCells);
    float halfWidth = cellWidth * resolution / 2;
    DirLight dirLight = {
            glm::vec3(-0.3f, -0.9f, -0.2),
            glm::vec3(0.5f),
            glm::vec3(1.0f),
            glm::vec3(2.0f)
    };
    PointLight pointLights[4];
    pointLights[0] = {
            glm::vec3(-5.0, 0.0, -5.0),
            1.0f,
            0.09,
            0.032,
            glm::vec3(0.05f),
            glm::vec3(0.8f),
            glm::vec3(1.0f)
    };
    pointLights[1] = {
            glm::vec3(-5.0, 0.0, 5.0),
            1.0f,
            0.09,
            0.032,
            glm::vec3(0.05f),
            glm::vec3(0.8f),
            glm::vec3(5.0f)
    };
    pointLights[2] = {
            glm::vec3(5.0, 0.0, 5.0),
            1.0f,
            0.09,
            0.032,
            glm::vec3(0.05f),
            glm::vec3(0.8f),
            glm::vec3(1.0f)
    };
    pointLights[3] = {
            glm::vec3(5.0, 0.0, -5.0),
            1.0f,
            0.09,
            0.032,
            glm::vec3(0.05f),
            glm::vec3(0.8f),
            glm::vec3(1.0f)
    };
    int cWidth = 400;
    int cHeight = 400;
    unsigned int dataSize = cWidth * cHeight * 3;
    auto *data = new unsigned char[dataSize];
    for (i = 0; i < dataSize; i++) {
        data[i] = Tex::ValueNoise_2D(i / (cWidth*3), (i / 3) % cHeight) * 255 * 0.5 + 255*0.5;
    }
    Texture textureDiffuse(cWidth, cHeight, "diffuse");
    textureDiffuse.init(data);

    Texture textureSpecular(cWidth, cHeight, "specular");
    textureSpecular.init(data);

    surface->GenerateSurface(scalarFieldFunction, 0, resolution, resolution, resolution, cellWidth, cellWidth,
                             cellWidth);
    POINT3D *points = surface->getVertices();
    VECTOR3D *normals = surface->getMPvec3DNormals();
    unsigned int numVertices = surface->getNumVertices();

    Vertex vertices[numVertices];

    for (i = 0; i < numVertices; i++) {
        Vertex v{};
        v.Position = glm::vec3(points[i][0], points[i][1], points[i][2]);
        v.Normal = glm::vec3(-normals[i][0], -normals[i][1], -normals[i][2]);
//        std::cout << i << " " << points[i][0] << "," << points[i][1] << "," << points[i][2] << " | "
//            << normals[i][0] << "," << normals[i][1] << "," << normals[i][2] <<std::endl;
        glm::vec3 sphereProjection = glm::normalize(v.Position);

//        v.TexCoords = glm::vec2(std::cos(i * 1.0 / numVertices * 3.1415926) / 2.0 + 0.5,
//                                std::sin(i * 1.0 / numVertices * 3.1415926) / 2.0 + 0.5);

        v.TexCoords = glm::vec2(0.5 + std::atan2(sphereProjection.x, sphereProjection.z)/(2*3.141592), 0.5 - asin(sphereProjection.y)/3.141592);
//        v.TexCoords = glm::vec2(i/numVertices, i/numVertices);
        vertices[i] = v;
    }

    unsigned int *indices = surface->getTriangleIndices();
    unsigned int numIndices = surface->getNumTriangles() * 3;
    auto **textures = new Texture *[2];
    textures[0] = &textureDiffuse;
    textures[1] = &textureSpecular;
    Mesh marchingCubesMesh(vertices, numVertices, indices, numIndices, textures, 2);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();     //get the time...
    std::cout << "Apparently, it's going to take you "
              << (end - start).count() / 1000000 << " milliseconds.\n";
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
    };
    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

    // load textures
    // -------------
    unsigned int cubeTexture = loadTexture(FileSystem::getPath("resources/textures/container.jpg").c_str());
    unsigned int floorTexture = loadTexture(FileSystem::getPath("resources/textures/metal.png").c_str());

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);
    cubeShader.setInt("texture1", 0);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // framebuffer configuration
    // -------------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
#ifdef __APPLE__
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH * 2, SCR_HEIGHT * 2, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
#ifdef __APPLE__
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH * 2, SCR_HEIGHT * 2);
#else
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH * 2,
                          SCR_HEIGHT * 2); // use a single renderbuffer object for both a depth AND stencil buffer.
#endif
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                              rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // draw as wireframe
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    std::cout << "error " << glGetError() << std::endl;

    long frameTotal = 0;
    // render loop
    // -----------
    GLuint query;
    glGenQueries(1, &query);

    glfwSwapInterval(0);
    int bufferingTime = 0;
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // render
        // ------
        // bind to framebuffer and draw scene as we normally would to color texture
        glBeginQuery(query, GL_TIME_ELAPSED);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        // make sure we clear the framebuffer's content
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        for (i = 0; i < 4; i++) {
            shader.setPointLight(i, pointLights[i]);
        }
        shader.setDirLight(dirLight);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);


        shader.setMat4("model", model);
        shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        shader.setVec3("viewPos", camera.Position);
        marchingCubesMesh.Draw(shader);

        // cubes
        cubeShader.use();
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(30));
        model = glm::translate(model, glm::vec3(.0f, 0.45f, 0.0f));
        cubeShader.setMat4("model", model);
        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);
        cubeShader.setVec3("objectColor", glm::vec3(0.5f, 0.8f, 0.6f));
        cubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        cubeShader.setVec3("lightPos", lightPos);
        cubeShader.setVec3("viewPos", camera.Position);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // cubes
//        cubeShader.use();
//        glBindVertexArray(cubeVAO);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, cubeTexture);
//        model = glm::mat4(1.0f);
////        model = glm::translate(model, glm::vec3(-5.0f, -5.0f, -5.0f));
////        model = glm::scale(model, glm::vec3(10));
//        cubeShader.setMat4("model", model);
//        cubeShader.setMat4("view", view);
//        cubeShader.setMat4("projection", projection);
//        cubeShader.setVec3("objectColor", glm::vec3(0.5f, 0.8f, 0.6f));
//        cubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
//        cubeShader.setVec3("lightPos", glm::vec3(0));
//        cubeShader.setVec3("viewPos", camera.Position);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
//        shader.setMat4("model", model);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        // floor
//        glBindVertexArray(planeVAO);
//        glBindTexture(GL_TEXTURE_2D, floorTexture);
//        shader.setMat4("model", glm::mat4(1.0f));
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        glBindVertexArray(0);

        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f,
                     1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D,
                      textureColorbuffer);    // use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        double previous = glfwGetTime();
        glfwSwapBuffers(window);
        bufferingTime += int((glfwGetTime() - previous) * 1000);
        glfwPollEvents();


        double wait_time = 1.0 / (targetFPS);
        double curr_frame_time = glfwGetTime() - lastFrame;
        double dur = 1000.0 * (wait_time - curr_frame_time);
        if (dur > 0) // ensures that we don't have a dur > 0.0 which converts to a durDW of 0.
        {
            usleep((int) dur * 1000);
        }
//        std::cout << int(curr_frame_time * 1000) << ":" << int(dur) << " ";
        if ((int) glfwGetTime() != (int) lastFrame) {
            std::cout << std::endl << "FPS: " << frameTotal << std::endl;
            if (frameTotal != 0) {
                std::cout << "Spent " << bufferingTime / frameTotal << "ms for swap buffers" << std::endl;
            }
            std::cout << "supposed to be sleeping for " << dur << "ms" << std::endl;
            bufferingTime = 0;
            frameTotal = 0;
        }
        frameTotal += 1;

    }

// optional: de-allocate all resources once they've outlived their purpose:
// ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);

    glfwTerminate();

    return 0;
}

float scalarFieldFunction(float x, float y, float z, ) {
    return std::sin(x * x) + std::atan2(y * y, z*x) + std::sin(z * z) - 0.5;
//    return fmax(fmax(abs(x) - 2, abs(y) - 2), abs(z) - 2);
}
//float scalarFieldFunction(float x, float y, float z) {
//    return x * x + y * y + z * z - 2;
//    return fmax(fmax(abs(x) - 2, abs(y) - 2), abs(z) - 2);
//}
// 2800ms
// 3597ms
// 3337ms
// 3318ms
//float scalarFieldFunction(float x, float y, float z) {
//    return x * x + y * y + z * z - 9;
//}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
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
unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
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
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
