#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "source_files/camera.h"
#include "source_files/shader.h"
#include "source_files/model.h"
#include "source_files/Szachownica.h"
#include "source_files/Game.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(3.5f, 12.0f, -8.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -45.0f);
// Forward declare game pointer for callbacks
static Game *g_game = nullptr;

// mouse button callback for selecting fields
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;
    if (!g_game) return;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // compute NDC
    float xndc = (2.0f * (float)xpos) / (float)SCR_WIDTH - 1.0f;
    float yndc = 1.0f - (2.0f * (float)ypos) / (float)SCR_HEIGHT;

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 inv = glm::inverse(projection * view);

    glm::vec4 nearPoint = inv * glm::vec4(xndc, yndc, -1.0f, 1.0f);
    glm::vec4 farPoint = inv * glm::vec4(xndc, yndc, 1.0f, 1.0f);
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    glm::vec3 rayOrigin = camera.Position;
    glm::vec3 rayDir = glm::normalize(glm::vec3(farPoint) - glm::vec3(nearPoint));

    // intersect with plane y=0
    if (fabs(rayDir.y) < 1e-6f) return; // parallel
    float t = - (rayOrigin.y) / rayDir.y;
    if (t <= 0) return;
    glm::vec3 intersect = rayOrigin + rayDir * t;

    int col = static_cast<int>(floor(intersect.x));
    int row = static_cast<int>(floor(intersect.z));
    if (col < 0 || col > 7 || row < 0 || row > 7) return;

    int fieldId = g_game->coordsToFieldId(row, col);
    if (fieldId >= 0)
    {
        g_game->handleFieldClick(fieldId);
    }
}

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Warcaby", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // no free camera movement: only mouse clicks are used for game interaction
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader;
    ourShader.compileShader("source_files/shaders/vertex.glsl","source_files/shaders/fragment.glsl");
    ourShader.createProgram();
    // create board (loads 64 cubes)
    Szachownica board;
    Game game;
    g_game = &game;
    const glm::vec3 boardCenter(3.5f, 0.0f, 3.5f);
    camera.SetPosition(glm::vec3(3.5f, 12.0f, 15.0f));
    camera.LookAt(boardCenter);
    // register mouse button callback to handle clicks
    glfwSetMouseButtonCallback(window, mouse_button_callback);
   
    

    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // update camera rotation when turn changes
        static PlayerTurn lastTurn = game.getCurrentTurn();
        if (game.getCurrentTurn() != lastTurn)
        {
            lastTurn = game.getCurrentTurn();
            if (lastTurn == PlayerTurn::WHITE)
            {
                camera.SetPosition(glm::vec3(3.5f, 12.0f, 15.0f));
                camera.LookAt(boardCenter);
            }
            else
            {
                camera.SetPosition(glm::vec3(3.5f, 12.0f, -8.0f));
                camera.LookAt(boardCenter);
            }
        }

        // render
        // ------
        //!//////////////////////////////////////////////////////////////////////////////////////
        //glClearColor(0.65f, 0.60f, 0.20f, 1.0f);
        glClearColor(0.22f, 0.01f, 0.54f, 1.0f);
        //!//////////////////////////////////////////////////////////////////////////////////////
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        ourShader.setMat4("model", model);
        
        // draw board (cubes)
        board.Draw(ourShader);
        // draw pieces from game state
        board.DrawPieces(game, ourShader);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // camera movement disabled: view is fixed above the board to support two-player checkers.
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

