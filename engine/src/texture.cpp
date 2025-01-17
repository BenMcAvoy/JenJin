#define GLFW_INCLUDE_NONE

#include "jenjin/texture.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const char *imagePath, bool alpha) {
  spdlog::trace("Texture::Texture(\"{}\", {})", imagePath, alpha);

  stbi_set_flip_vertically_on_load(true);

  // Load and generate the texture
  glGenTextures(1, &ID);
  // All upcoming GL_TEXTURE_2D operations now have effect on
  // this texture object, similar to glfwMakeContextCurrent
  glBindTexture(GL_TEXTURE_2D, ID);

  // Set the texture wrapping parameters (if it's too small or too big)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Set texture filtering parameters for minification and magnification
  // (mipmapping)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Load the image, then create texture and generate the mipmaps
  int w, h, c;
  unsigned char *data = stbi_load(imagePath, &w, &h, &c, 0);

  if (data) {
    spdlog::debug("Loaded texture {}: {}x{} with {} channels", imagePath, w, h,
                  c);
    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, w, h, 0,
                 alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    const char *description = stbi_failure_reason();
    spdlog::error("Failed to load texture: {}", description);
  }

  stbi_image_free(data);

  this->imagePath = imagePath;
}

Texture::~Texture() {
  spdlog::trace("Texture::~Texture(\"{}\")", imagePath);
  glDeleteTextures(1, &ID);
}

void Texture::bind(int id) {
  glActiveTexture(GL_TEXTURE0 + id);
  glBindTexture(GL_TEXTURE_2D, ID);
}
