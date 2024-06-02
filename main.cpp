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

class lrnOpenGL {
private:
  // settings
  constexpr static const unsigned int SCR_WIDTH = 1280;
  constexpr static const unsigned int SCR_HEIGHT = 720;
  constexpr static const uint32_t GRID_SIZE = 100;
  constexpr static const uint32_t GRIDS_NUM = 5;

  // window
  GLFWwindow *window;

  // input
  float mix = 0.5f;
  glm::vec3 camera_pos{0.0f};

  // game of life
  std::array<std::array<std::array<bool, GRID_SIZE>, GRID_SIZE>, GRIDS_NUM>
      grids{};

  // camera vertices
  constexpr static const glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
  constexpr static const glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
  constexpr static const float camera_speed = 5.0f;

  // time
  float time = glfwGetTime();
  float old_time;
  float delta_time;
  float timer = 1.0f;

  // matrices
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view;
  glm::mat4 proj;

  // shader program
  Shader shader;

  // opengl state machine
  unsigned int VBO, VAO;
  unsigned int texture1, texture2;

  // vertex data
  constexpr static const float vertices[] = {
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // init opengl
    if (!gladLoadGL()) {
      std::cerr << "Failed to initialize GLAD\n";
      glfwTerminate();
      return 2;
    }

    shader.init("shader.vert", "shader.frag");

    // game of life
    grids[0][0][1] = true;
    grids[0][1][3] = true;
    grids[0][2][0] = true;
    grids[0][2][1] = true;
    grids[0][2][4] = true;
    grids[0][2][5] = true;
    grids[0][2][6] = true;

    // init vertex buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // vertex pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // create textures
    glGenTextures(1, &texture1);
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
      return 3;
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
      return 3;
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

    proj = glm::perspective(glm::radians(45.0f),
                            static_cast<float>(SCR_WIDTH / SCR_HEIGHT), 0.1f,
                            100.0f);

    return 0;
  }

  void render_loop() {
    while (!glfwWindowShouldClose(window)) {
      // input
      glfwPollEvents();
      processInput();

      // time
      old_time = time;
      time = glfwGetTime();
      delta_time = time - old_time;

      // vector and matrix manipulation
      view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);

      // update grid
      timer -= delta_time;
      if (timer < 0.0f) {
        timer = 1.0f;
        for (uint32_t i = 0; i < GRIDS_NUM; i++) {
          game_of_live(grids[i]);
        }
      }

      // setting shader values
      shader.setFloat("mix_val", mix);
      shader.setMat4("view", view);
      shader.setMat4("proj", proj);

      // render
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      for (uint32_t i = 0; i < GRIDS_NUM; i++) {
        for (unsigned int j = 0; j < GRID_SIZE; j++) {
          for (unsigned int k = 0; k < GRID_SIZE; k++) {
            if (grids[i][j][k]) {
              glm::mat4 model = glm::mat4(1.0f);
              model = glm::translate(model, {j * 2, i * 3, k * 2});
              shader.setMat4("model", model);

              glDrawArrays(GL_TRIANGLES, 0, 36);
            }
          }
        }
      }

      // draw frame
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
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      if (mix < 1.0f)
        mix += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      if (mix > 0.0f)
        mix -= 0.01f;

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
};

int main() {
  lrnOpenGL demo;
  demo.run();
}
