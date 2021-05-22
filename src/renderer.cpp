#include "renderer.h"
#include <glm/ext.hpp>
#include <vector>
#include <iostream>

static const char* vertex_shader_text =
"#version 430\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"
"uniform mat4 model;\n"
"layout(location = 0) in vec3 vPos;\n"
"layout(location = 1) in vec3 vCol;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = proj * view * model * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 430\n"
"in vec3 color;\n"
"uniform int Pass;\n"
"uniform float Weight[5];\n"
"uniform float focal_distance = 8.0;\n"
"uniform float focal_range = 10.0;\n"
"layout(binding = 0) uniform sampler2D Texture0;\n"
"layout(binding = 1) uniform sampler2D HighRes;\n"
"layout(location = 0) out vec4 FragColor;\n"

"vec4 pass1() {\n"
"   //return FragColor = vec4(color, 1.0);\n"
"   float Z = gl_FragCoord.z / gl_FragCoord.w;\n"
"   float blur = clamp(abs(Z - focal_distance) / focal_range, 0.0, 1.0);\n"
"   return FragColor = vec4(color, blur);\n"
"}\n"

"vec4 pass2() {\n"
    "ivec2 pix = ivec2(gl_FragCoord.xy);\n"
    "vec4 sum = texelFetch(Texture0, pix, 0) * Weight[0];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, 1)) * Weight[1];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, -1)) * Weight[1];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, 2)) * Weight[2];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, -2)) * Weight[2];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, 3)) * Weight[3];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, -3)) * Weight[3];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, 4)) * Weight[4];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, -4)) * Weight[4];\n"
    "return sum;\n"
"}\n"

"vec4 pass3() {\n"
    "ivec2 pix = ivec2(gl_FragCoord.xy);\n"
    "vec4 sum = texelFetch(Texture0, pix, 0) * Weight[0];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(1, 0)) * Weight[1];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(-1, 0)) * Weight[1];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(2, 0)) * Weight[2];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(-2, 0)) * Weight[2];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(3, 0)) * Weight[3];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(-3, 0)) * Weight[3];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(4, 0)) * Weight[4];\n"
    "sum += texelFetchOffset(Texture0, pix, 0, ivec2(-4, 0)) * Weight[4];\n"
    "vec4 fullRes = texelFetch(HighRes, pix, 0);\n"
    "return fullRes + fullRes.a * (sum - fullRes);\n"
    "return sum;\n"
"}\n"

"void main()\n"
"{\n"
    "if( Pass == 1 ) FragColor = pass1();\n"
    "else if (Pass == 2) FragColor = pass2();\n"
    "else if (Pass == 3) FragColor = pass3();\n"
"}\n";

static const GLfloat g_vertex_buffer_data[] = {
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

static const GLfloat g_color_buffer_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
};

RenderData Renderer::sd;

void Renderer::Init()
{
  sd.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(sd.vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(sd.vertex_shader);

  GLint isCompiled = 0;
  glGetShaderiv(sd.vertex_shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetShaderiv(sd.vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> infoLog(maxLength);
    glGetShaderInfoLog(sd.vertex_shader, maxLength, &maxLength, &infoLog[0]);

    glDeleteShader(sd.vertex_shader);

    std::cout << infoLog.data();
  }

  sd.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(sd.fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(sd.fragment_shader);

  isCompiled = 0;
  glGetShaderiv(sd.fragment_shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetShaderiv(sd.fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> infoLog(maxLength);
    glGetShaderInfoLog(sd.fragment_shader, maxLength, &maxLength, &infoLog[0]);

    glDeleteShader(sd.fragment_shader);

    std::cout << infoLog.data();
  }

  sd.program = glCreateProgram();
  glAttachShader(sd.program, sd.vertex_shader);
  glAttachShader(sd.program, sd.fragment_shader);
  glLinkProgram(sd.program);

  GLint isLinked = 0;
  glGetProgramiv(sd.program, GL_LINK_STATUS, (int*)&isLinked);
  if (isLinked == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetProgramiv(sd.program, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(sd.program, maxLength, &maxLength, &infoLog[0]);

    // We don't need the program anymore.
    glDeleteProgram(sd.program);


    //glDeleteShader(id);

    std::cout << infoLog.data();
  }
  glUseProgram(sd.program);

  sd.view_location = glGetUniformLocation(sd.program, "view");
  sd.proj_location = glGetUniformLocation(sd.program, "proj");
  sd.model_location = glGetUniformLocation(sd.program, "model");
  sd.pass_location = glGetUniformLocation(sd.program, "Pass");
  sd.weight_location = glGetUniformLocation(sd.program, "Weight");
  sd.focal_dist_loc = glGetUniformLocation(sd.program, "focal_distance");
  sd.focal_range_loc = glGetUniformLocation(sd.program, "focal_range"); 

  glGenVertexArrays(1, &sd.va);
  glBindVertexArray(sd.va);

  glGenBuffers(1, &sd.vb);
  glBindBuffer(GL_ARRAY_BUFFER, sd.vb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glGenBuffers(1, &sd.color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, sd.color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, sd.color_buffer);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // Setup FBO--------------------------------------------------------------------------------------
  // Generate and bind the framebuffer
  glGenFramebuffers(1, &sd.renderFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, sd.renderFBO);

  // Create the texture object
  glGenTextures(1, &sd.renderTex);
  glBindTexture(GL_TEXTURE_2D, sd.renderTex);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  // Bind the texture to the FBO
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sd.renderTex, 0);

  // Create the depth buffer
  GLuint depthBuf;
  glGenRenderbuffers(1, &depthBuf);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);

  // Bind the depth buffer to the FBO
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_RENDERBUFFER, depthBuf);

  // Set the targets for the fragment output variables
  GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, drawBuffers);

  // Unbind the framebuffer, and revert to default framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Generate and bind the framebuffer
  glGenFramebuffers(1, &sd.intermediateFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, sd.intermediateFBO);

  // Create the texture object
  glGenTextures(1, &sd.intermediateTex);
  glActiveTexture(GL_TEXTURE0);  // Use texture unit 0
  glBindTexture(GL_TEXTURE_2D, sd.intermediateTex);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  // Bind the texture to the FBO
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sd.intermediateTex, 0);

  // Set the targets for the fragment output variables
  glDrawBuffers(1, drawBuffers);

  // Unbind the framebuffer, and revert to default framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //-----------------------------------------------------------------------------------------------
  // Array for full-screen quad
  GLfloat verts[] = {
      -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
      -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
  };
  GLfloat tc[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
      0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
  };

  // Set up the buffers

  GLuint handle[2];
  glGenBuffers(2, handle);

  glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
  glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
  glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

  // Set up the vertex array object

  glGenVertexArrays(1, &sd.fsQuad);
  glBindVertexArray(sd.fsQuad);

  glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
  glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
  glEnableVertexAttribArray(0);  // Vertex position

  glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
  glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
  glEnableVertexAttribArray(2);  // Texture coordinates

  glBindVertexArray(0);

  float weights[5], sum, sigma2 = 20.0f;

  // Compute and sum the weights
  weights[0] = gauss(0, sigma2);
  sum = weights[0];
  for (int i = 1; i < 5; i++) {
    weights[i] = gauss(float(i/10), sigma2);
    sum += 2 * weights[i];
  }

  // Normalize the weights and set the uniform
  for (int i = 0; i < 5; i++) {
    weights[i] = weights[i] / sum;
    //prog.setUniform(uniName.str().c_str(), val);
  }
  glUniform1fv(sd.weight_location, 5, weights);
}

void Renderer::StartDOF()
{
  glUniform1i(sd.pass_location, 1);

  glBindFramebuffer(GL_FRAMEBUFFER, sd.renderFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
}

void Renderer::EndDOF()
{
  // Pass 2
  glUniform1i(sd.pass_location, 2);

  glBindFramebuffer(GL_FRAMEBUFFER, sd.intermediateFBO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sd.renderTex);

  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);

  glm::mat4 ident = glm::identity<glm::mat4>();

  glUniformMatrix4fv(sd.view_location, 1, GL_FALSE, (const GLfloat*)&ident);
  glUniformMatrix4fv(sd.proj_location, 1, GL_FALSE, (const GLfloat*)&ident);
  glUniformMatrix4fv(sd.model_location, 1, GL_FALSE, (const GLfloat*)&ident);

  // Render the full-screen quad
  glBindVertexArray(sd.fsQuad);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Pass 3
  glUniform1i(sd.pass_location, 3);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sd.intermediateTex);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, sd.renderTex);

  glClear(GL_COLOR_BUFFER_BIT);

  /*model = mat4(1.0f);
  view = mat4(1.0f);
  projection = mat4(1.0f);
  setMatrices();*/

  // Render the full-screen quad
  glBindVertexArray(sd.fsQuad);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

float Renderer::gauss(float x, float sigma2)
{
  double coeff = 1.0 / (glm::two_pi<double>() * sigma2);
  double expon = -(x * x) / (2.0 * sigma2);
  return (float)(coeff * exp(expon));
}

void Renderer::DrawBox(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation)
{
  glBindVertexArray(sd.va);
  glm::mat4 model(1.0f);
  //glm::mat4 model;
  model = glm::scale(model, scale);
  model = glm::translate(model, position);
  model = glm::rotate(model, rotation.x, { 1.0f, 0.0f, 0.0f });
  model = glm::rotate(model, rotation.y, { 0.0f, 1.0f, 0.0f });
  model = glm::rotate(model, rotation.z, { 0.0f, 0.0f, 1.0f });
  

  glUniformMatrix4fv(sd.model_location, 1, GL_FALSE, (const GLfloat*)&model);
  glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

