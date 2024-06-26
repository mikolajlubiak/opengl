#include <glad/glad.h>

#include "shader.hpp"

#include <GLFW/glfw3.h>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <iostream>

class lrnOpenGL {
private:
  // settings
  constexpr static uint32_t SCR_WIDTH = 1280;
  constexpr static uint32_t SCR_HEIGHT = 720;

  // window
  GLFWwindow *window;

  // camera vertices
  glm::vec3 camera_pos = glm::vec3(-10.0f, 10.0f, -5.0f);
  glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
  constexpr static glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
  constexpr static float camera_speed = 5.0f;

  // mouse
  float last_x = SCR_WIDTH / 2.0f;
  float last_y = SCR_HEIGHT / 2.0f;
  float yaw = 0.0f, pitch = 0.0f;
  bool first_mouse_movement = true;
  float fov = 45.0f;

  // time
  float time = glfwGetTime();
  float old_time;
  float delta_time;
  float timer = 0.2f;

  // matrices
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;

  // shader program
  Shader shader;

  // opengl state machine
  uint32_t VBO, VAO;

  // Conway's Game of Life
  constexpr static uint32_t GRID_SIZE = 50;
  constexpr static uint32_t GRID_NUM = 20;
  std::array<std::array<std::array<bool, GRID_SIZE>, GRID_SIZE>, GRID_NUM>
      grids{};

  // cube
  constexpr static float cube[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
      0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
      0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
      0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
      0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

  uint8_t init() {
    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // create window
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "me learning opengl", NULL,
                              NULL);

    if (window == NULL) {
      std::cerr << "Failed to create GLFW window\n";
      glfwTerminate();
      return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // init opengl
    if (!gladLoadGL()) {
      std::cerr << "Failed to initialize GLAD\n";
      glfwTerminate();
      return 2;
    }

    // Conway's Game of Life
    std::array<std::array<bool, GRID_SIZE>, GRID_SIZE> grid{};

    // Place a glider at position (1,2), (2,3), (3,4), (4,5), (3,6)
    grid[1][2] = true;
    grid[2][3] = true;
    grid[3][4] = true;
    grid[4][5] = true;
    grid[3][6] = true;

    // Place a blinker oscillator at position (20,30), (21,30), (22,30)
    grid[20][30] = true;
    grid[21][30] = true;
    grid[22][30] = true;

    // Place a lightweight spaceship at position (40,10), (41,11), (42,12),
    // (43,13), (44,14), (45,15)
    grid[40][10] = true;
    grid[41][11] = true;
    grid[42][12] = true;
    grid[43][13] = true;
    grid[44][14] = true;
    grid[45][15] = true;

    grids[0] = grid;

    // init shaders
    shader.init("shader.vert", "shader.frag");

    // vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    // object VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // opengl state machine
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // setting object shader values
    shader.use();

    // material
    shader.setVec3("material.ambient", glm::vec3(0.3f));
    shader.setVec3("material.diffuse", glm::vec3(0.9f));
    shader.setVec3("material.specular", glm::vec3(0.75f));
    shader.setFloat("material.shininess", 8.0f);

    // directional light
    shader.setVec3("light.direction", 0.2f, -1.0f, 0.5f);
    shader.setVec3("light.ambient", glm::vec3(0.75f));
    shader.setVec3("light.diffuse", glm::vec3(1.0f));
    shader.setVec3("light.specular", glm::vec3(1.0f));

    return 0;
  }

  void render_loop() {
    while (!glfwWindowShouldClose(window)) {
      // clear color and depth buffers
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // input
      glfwPollEvents();
      processInput();

      // time
      old_time = time;
      time = glfwGetTime();
      delta_time = time - old_time;

      // Conway's Game of Life
      timer -= delta_time;
      if (timer < 0.0f) {
        timer = 0.2f;
        game_of_life();
      }

      // vector and matrix manipulation
      view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);

      projection = glm::perspective(glm::radians(fov),
                                    static_cast<float>(SCR_WIDTH) / SCR_HEIGHT,
                                    0.1f, 100.0f);

      // camera
      shader.setVec3("camera_pos", camera_pos);

      // matrices
      shader.setMat4("view", view);
      shader.setMat4("projection", projection);

      // draw object
      for (uint32_t i = 0; i < GRID_NUM; i++) {
        for (uint32_t j = 0; j < GRID_SIZE; j++) {
          for (uint32_t k = 0; k < GRID_SIZE; k++) {
            if (grids[i][j][k]) {
              model = glm::mat4(1.0f);
              model = glm::translate(model, {j, i * -1.0f, k});
              shader.setMat4("model", model);
              shader.setMat3("normal_matrix",
                             glm::transpose(glm::inverse(model)));

              glDrawArrays(GL_TRIANGLES, 0, 36);
            }
          }
        }
      }

      // render frame
      glfwSwapBuffers(window);
    }
  }

  void free_resources() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
  }

public:
  void run() {
    if (init() != 0) {
      std::cerr << "ERROR\n";
      return;
    }

    render_loop();

    free_resources();
  }

private:
  void processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      camera_pos += camera_front * camera_speed * delta_time;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      camera_pos -= camera_front * camera_speed * delta_time;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) *
                    camera_speed * delta_time;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) *
                    camera_speed * delta_time;
  }

  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height) {
    glViewport(0, 0, width, height);
  }

  static void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    lrnOpenGL *ths =
        reinterpret_cast<lrnOpenGL *>(glfwGetWindowUserPointer(window));

    if (ths->first_mouse_movement) {
      ths->last_x = xpos;
      ths->last_y = ypos;
      ths->first_mouse_movement = false;
    }

    float xoffset = xpos - ths->last_x;
    float yoffset = ths->last_y - ypos;
    ths->last_x = xpos;
    ths->last_y = ypos;

    constexpr const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    ths->yaw += xoffset;
    ths->pitch += yoffset;

    if (ths->pitch > 89.0f)
      ths->pitch = 89.0f;
    if (ths->pitch < -89.0f)
      ths->pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(ths->yaw)) * cos(glm::radians(ths->pitch));
    direction.y = sin(glm::radians(ths->pitch));
    direction.z = sin(glm::radians(ths->yaw)) * cos(glm::radians(ths->pitch));
    ths->camera_front = glm::normalize(direction);
  }

  static void scroll_callback(GLFWwindow *window, double xoffset,
                              double yoffset) {
    lrnOpenGL *ths =
        reinterpret_cast<lrnOpenGL *>(glfwGetWindowUserPointer(window));

    ths->fov -= static_cast<float>(yoffset * 2.0f);
    if (ths->fov < 1.0f)
      ths->fov = 1.0f;
    if (ths->fov > 90.0f)
      ths->fov = 90.0f;
  }

  void game_of_life() {
    std::array<std::array<std::array<bool, GRID_SIZE>, GRID_SIZE>, GRID_NUM>::iterator it = grids.begin();
    std::advance(it, -1);
    std::copy(it, it+GRID_NUM, grids.begin());

    std::array<std::array<bool, GRID_SIZE>, GRID_SIZE> temp_grid;

    for (uint32_t i = 0; i < GRID_SIZE; ++i) {
      for (uint32_t j = 0; j < GRID_SIZE; ++j) {
        uint32_t aliveNeighbors = 0;

        for (uint32_t dx = -1; dx <= 1; ++dx) {
          for (uint32_t dy = -1; dy <= 1; ++dy) {
            uint32_t x = i + dx;
            uint32_t y = j + dy;

            if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE &&
                (dx != 0 || dy != 0)) {
              aliveNeighbors += grids[-1][x][y];
            }
          }
        }

        if (grids[-1][i][j] == 1 && (aliveNeighbors < 2 || aliveNeighbors > 3))
          temp_grid[i][j] = 0;
        else if (grids[-1][i][j] == 0 && aliveNeighbors == 3)
          temp_grid[i][j] = 1;
        else
          temp_grid[i][j] = grids[-1][i][j];
      }
    }

    grids[0] = temp_grid;
  }
};

int main() {
  lrnOpenGL demo;
  demo.run();
}
