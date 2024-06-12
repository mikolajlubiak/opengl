#include <glad/glad.h>

#include "model.hpp"
#include "shader.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

class lrnOpenGL {
private:
  // settings
  constexpr static uint32_t SCR_WIDTH = 1280;
  constexpr static uint32_t SCR_HEIGHT = 720;

  // window
  GLFWwindow *window;

  // camera vertices
  glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
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

  // matrices
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;

  // shader program
  Shader shader;
  Shader light_shader;

  // opengl state machine
  uint32_t VBO, light_VAO;

  // backpack model
  Model backpack;

  // cube
  constexpr static float cube[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

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

    shader.init("shader.vert", "shader.frag");
    light_shader.init("light_shader.vert", "light_shader.frag");

    backpack.loadModel("backpack/backpack.obj");

    // light VAO
    glGenVertexArrays(1, &light_VAO);
    glBindVertexArray(light_VAO);

    // vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // opengl state machine
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    stbi_set_flip_vertically_on_load(true);

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

      // vector and matrix manipulation
      model = glm::mat4(1.0f);
      view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);

      projection = glm::perspective(glm::radians(fov),
                                    static_cast<float>(SCR_WIDTH) / SCR_HEIGHT,
                                    0.1f, 100.0f);

      // light pos
      glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, 0.0f);
      light_pos.x = 1.0f + sin(time) * 2.0f;
      light_pos.y = sin(time / 2.0f) * 1.0f;

      // light color
      glm::vec3 light_color;
      light_color.x = sin(time * 2.0f);
      light_color.y = sin(time * 0.7f);
      light_color.z = sin(time * 1.3f);

      glm::vec3 diffuse_color = light_color * glm::vec3(0.5f);
      glm::vec3 ambient_color = diffuse_color * glm::vec3(0.2f);

      // static light
      glm::vec3 static_light_diffuse = glm::vec3(0.5f);
      glm::vec3 static_light_ambient = glm::vec3(0.05f);

      // setting object shader values
      shader.use();

      // material
      shader.setFloat("material.shininess", 32.0f);

      // directional light
      shader.setVec3("dir_light.direction", -0.2f, -1.0f, -0.3f);
      shader.setVec3("dir_light.ambient", glm::vec3(0.01f));
      shader.setVec3("dir_light.diffuse", glm::vec3(0.1f));
      shader.setVec3("dir_light.specular", 0.5f, 0.5f, 0.5f);

      // spot light
      shader.setVec3("spot_light.position", camera_pos);
      shader.setVec3("spot_light.direction", camera_front);
      shader.setVec3("spot_light.ambient", 0.0f, 0.0f, 0.0f);
      shader.setVec3("spot_light.diffuse", 1.0f, 1.0f, 1.0f);
      shader.setVec3("spot_light.specular", 1.0f, 1.0f, 1.0f);
      shader.setFloat("spot_light.constant", 1.0f);
      shader.setFloat("spot_light.linear", 0.09f);
      shader.setFloat("spot_light.quadratic", 0.032f);
      shader.setFloat("spot_light.cut_off", glm::cos(glm::radians(12.5f)));
      shader.setFloat("spot_light.outer_cut_off",
                      glm::cos(glm::radians(15.0f)));

      shader.setVec3("point_light.position", light_pos);
      shader.setVec3("point_light.ambient", ambient_color);
      shader.setVec3("point_light.diffuse", diffuse_color);
      shader.setVec3("point_light.specular", 1.0f, 1.0f, 1.0f);
      shader.setFloat("point_light.constant", 1.0f);
      shader.setFloat("point_light.linear", 0.09f);
      shader.setFloat("point_light.quadratic", 0.032f);

      // camera
      shader.setVec3("camera_pos", camera_pos);

      // matrices
      shader.setMat4("model", model);
      shader.setMat3("normal_matrix", glm::transpose(glm::inverse(model)));
      shader.setMat4("view", view);
      shader.setMat4("projection", projection);

      // draw object
      backpack.Draw(shader);

      // set light shader values
      light_shader.use();
      light_shader.setMat4("view", view);
      light_shader.setMat4("projection", projection);
      light_shader.setVec3("light_color", static_light_diffuse);

      // manipulate light model matrix
      model = glm::mat4(1.0f);
      model = glm::translate(model, light_pos + glm::vec3(0.0f, 0.0f, 3.0f));
      model = glm::scale(model, glm::vec3(0.2f));
      light_shader.setMat4("model", model);
      light_shader.setVec3("light_color", light_color);

      // draw light
      glBindVertexArray(light_VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      // render frame
      glfwSwapBuffers(window);
    }
  }

  void free_resources() {
    glDeleteVertexArrays(1, &light_VAO);
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
};

int main() {
  lrnOpenGL demo;
  demo.run();
}
