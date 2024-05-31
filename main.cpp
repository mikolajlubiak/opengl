#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "shader.h"

struct input_variables {
  float mix;
  float i, j, k;
};

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const uint32_t GRID_SIZE = 100;
const uint32_t GRIDS_NUM = 5;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, input_variables &input);
void game_of_live(std::array<std::array<bool, GRID_SIZE>, GRID_SIZE> &grid);

int main() {
  // init glfw
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // create window
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "me learning opengl", NULL, NULL);

  if (window == NULL) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // init opengl
  if (!gladLoadGL()) {
    std::cerr << "Failed to initialize GLAD\n";
    glfwTerminate();
    return -1;
  }

  // init shader program
  Shader shader("shader.vert", "shader.frag");

  // variables
  input_variables input = {0.5f, 0.0f, 0.0f, 0.0f};

  std::array<std::array<std::array<bool, GRID_SIZE>, GRID_SIZE>, GRIDS_NUM>
      grids{};

  // game of life
  grids[0][0][1] = true;
  grids[0][1][3] = true;
  grids[0][2][0] = true;
  grids[0][2][1] = true;
  grids[0][2][4] = true;
  grids[0][2][5] = true;
  grids[0][2][6] = true;

  // vertex data
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

  // init vertex buffers
  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  // vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // vertex pos
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // texture attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // create textures
  unsigned int texture1;
  glGenTextures(1, &texture1);
  unsigned int texture2;
  glGenTextures(1, &texture2);

  glBindTexture(GL_TEXTURE_2D, texture1);

  // texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // load image data
  int width1, height1, nrChannels1;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
      stbi_load("container.jpg", &width1, &height1, &nrChannels1, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    // generate mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "Failed to load texture\n";
    return -1;
  }

  glBindTexture(GL_TEXTURE_2D, texture2);

  // texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // load image data
  int width2, height2, nrChannels2;
  data = stbi_load("awesomeface.png", &width2, &height2, &nrChannels2, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    // generate mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "Failed to load texture\n";
    return -1;
  }
  stbi_image_free(data);

  // bind textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture2);

  // set shader values
  shader.use();
  shader.setInt("texture1", 0);
  shader.setInt("texture2", 1);

  // opengl state machine
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  // matrices
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 proj = glm::mat4(1.0f);

  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -20.0f));
  proj = glm::perspective(glm::radians(45.0f),
                          static_cast<float>(SCR_WIDTH / SCR_HEIGHT), 0.1f,
                          100.0f);
  float time = glfwGetTime();
  float old_time;

  float timer = 1.0f;

  // render loop
  while (!glfwWindowShouldClose(window)) {
    // input
    glfwPollEvents();
    processInput(window, input);

    // vector and matrix manipulation
    old_time = time;
    time = glfwGetTime();

    timer -= time - old_time;
    if (timer < 0.0f) {
      timer = 1.0f;
      for (uint32_t i = 0; i < GRIDS_NUM; i++) {
        game_of_live(grids[i]);
      }
    }

    // setting shader values
    shader.setFloat("mix_val", input.mix);
    shader.setMat4("view", view);
    shader.setMat4("proj", proj);

    // render
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (uint32_t i = 0; i < GRIDS_NUM; i++) {
      for (unsigned int j = 0; j < GRID_SIZE; j++) {
        for (unsigned int k = 0; k < GRID_SIZE; k++) {
          if (grids[i][j][k]) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(
                model, {j * 2 - 3.0f + input.j, i * 3 - 3.5f + input.i, k * 2});
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
          }
        }
      }
    }

    // draw frame
    glfwSwapBuffers(window);
  }

  // free resources
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwTerminate();

  return 0;
}

void processInput(GLFWwindow *window, input_variables &input) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    if (input.mix < 1.0f)
      input.mix += 0.01f;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    if (input.mix > 0.0f)
      input.mix -= 0.01f;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    input.i -= 0.01f;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    input.i += 0.01f;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    input.j += 0.01f;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    input.j -= 0.01f;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void game_of_live(std::array<std::array<bool, GRID_SIZE>, GRID_SIZE> &grid) {
  std::array<std::array<bool, GRID_SIZE>, GRID_SIZE> temp_grid;
  std::copy(std::begin(grid), std::end(grid), std::begin(temp_grid));

  for (int a = 1; a < GRID_SIZE - 1; a++) {
    for (int b = 1; b < GRID_SIZE - 1; b++) {
      int alive = 0;
      for (int c = -1; c < 2; c++) {
        for (int d = -1; d < 2; d++) {
          if (!(c == 0 && d == 0)) {
            if (temp_grid[a + c][b + d]) {
              ++alive;
            }
          }
        }
      }
      if (alive < 2) {
        grid[a][b] = false;
      } else if (alive == 3) {
        grid[a][b] = true;
      } else if (alive > 3) {
        grid[a][b] = false;
      }
    }
  }
}
