#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "shader.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mix = 0.5f;

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

  // vertex data
  float vertices[] = {
      // positions        // texture coords
      0.5f,  0.5f,  0.0f, 2.0f, 2.0f, // top right
      0.5f,  -0.5f, 0.0f, 2.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 2.0f  // top left
  };

  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  // init vertex buffers
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  // vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

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

  // matrices
  glm::mat4 trans = glm::mat4(1.0f);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    // input
    glfwPollEvents();
    processInput(window);

    // vector and matrix manipulation
    float time = glfwGetTime();
    trans = glm::rotate(trans, static_cast<float>(glfwGetTime() / 1000),
                        glm::vec3(0.0, 0.0, 1.0));

    // setting shader values
    shader.setFloat("mix_val", mix);
    unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    // render
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // swap buffers
    glfwSwapBuffers(window);
  }

  // free resources
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();

  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    if (mix < 1.0f)
      mix += 0.01f;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    if (mix > 0.0f)
      mix -= 0.01f;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
