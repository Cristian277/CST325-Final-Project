#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>

GLFWwindow* initialize_glfw() {
	if (!glfwInit()) {
		std::cout << "glfwInit(...) failed\n";
		abort();
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window
	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World >:)", NULL, NULL); //setting the window pointer to be width 640, height 480, and title hello world
	if (!window) {
		std::cout << "glfwCreateWindow(...) failed\n";
		glfwTerminate();
		abort();
	}

	// Make the context current
	glfwMakeContextCurrent(window);

	// Load the function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);

	return window; //returns window pointer type glfwwindow
}

struct Model {

	GLuint vao;
	GLuint vbo; //unsigned int that can only be positive and we are creating a vertex array object and vertex buffer object
	GLsizei vertex_count;  //an int used for size

	void draw() {
		glBindVertexArray(this->vao);
		glDrawArrays(GL_TRIANGLES, 0, this->vertex_count);
	}

	void cleanup() {
		glDeleteVertexArrays(1, &this->vao);
		glDeleteBuffers(1, &this->vbo);
	}

	static Model load(std::string objectFileName) {
		// Send the vertex data to the GPU
		Model model;

		std::fstream file(objectFileName);
		if (!file) {

			std::cout << "could not find model file";
			abort();

		}

		std::string line;
		std::vector<glm::vec3>positions;
		std::vector<glm::vec2>texcoords;
		std::vector<GLfloat> vertices;
		std::vector<glm::vec3>normals_vec;

		while (std::getline(file, line)) {
			std::istringstream line_stream(line);
			std::string type;
			line_stream >> type;

			if (type == "v") {
				//its a vertex position
				GLfloat x;
				GLfloat y;
				GLfloat z;
				line_stream >> x;//converts first line into float
				line_stream >> y;
				line_stream >> z;
				positions.push_back(glm::vec3(x, y, z));

			}
			else if (type == "vn") {
				//its a vertex position
				GLfloat x;
				GLfloat y;
				GLfloat z;
				line_stream >> x;//converts first line into float
				line_stream >> y;
				line_stream >> z;
				normals_vec.push_back(glm::vec3(x, y, z));

			}
			else if (type == "vt") {

				GLfloat u, v;
				line_stream >> u;
				line_stream >> v;
				texcoords.push_back(glm::vec2(u, v));

				//std::cout << "its a vertex texcoord\n";
			}
			else if (type == "f") {
				//std::cout << "its a face\n";
				std::string face;

				line_stream >> face;
				std::replace(face.begin(), face.end(), '/', ' ');
				std::istringstream face_stream0(face);

				size_t position_index0;
				size_t texcoord_index0;
				size_t normal_index0;
				face_stream0 >> position_index0;
				face_stream0 >> texcoord_index0;
				face_stream0 >> normal_index0;

				glm::vec3 position0 = positions.at(position_index0 - 1);
				glm::vec2 texcoord0 = texcoords.at(texcoord_index0 - 1);
				glm::vec3 normals0 = normals_vec.at(normal_index0 - 1);

				line_stream >> face;
				std::replace(face.begin(), face.end(), '/', ' ');
				std::istringstream face_stream1(face);

				size_t position_index1;
				size_t texcoord_index1;
				size_t normal_index1;
				face_stream1 >> position_index1;
				face_stream1 >> texcoord_index1;
				face_stream1 >> normal_index1;

				glm::vec3 position1 = positions.at(position_index1 - 1);
				glm::vec2 texcoord1 = texcoords.at(texcoord_index1 - 1);
				glm::vec3 normals1 = normals_vec.at(normal_index1 - 1);

				line_stream >> face;
				std::replace(face.begin(), face.end(), '/', ' ');
				std::istringstream face_stream2(face);

				size_t position_index2;
				size_t texcoord_index2;
				size_t normal_index2;
				face_stream2 >> position_index2;
				face_stream2 >> texcoord_index2;
				face_stream2 >> normal_index2;

				glm::vec3 position2 = positions.at(position_index2 - 1);
				glm::vec2 texcoord2 = texcoords.at(texcoord_index2 - 1);
				glm::vec3 normals2 = normals_vec.at(normal_index2 - 1);

				glm::vec3 edge0 = position1 - position0;
				glm::vec3 edge1 = position2 - position0;
				glm::vec2 delta_texcoords0 = texcoord1 - texcoord0;
				glm::vec2 delta_texcoords1 = texcoord2 - texcoord0;

				float f = 1.0f / (delta_texcoords0.x * delta_texcoords1.y - delta_texcoords1.x * delta_texcoords0.y);

				glm::vec3 tangent;
				tangent.x = f * (delta_texcoords1.y * edge0.x - delta_texcoords0.y * edge1.x);
				tangent.y = f * (delta_texcoords1.y * edge0.y - delta_texcoords0.y * edge1.y);
				tangent.z = f * (delta_texcoords1.y * edge0.z - delta_texcoords0.y * edge1.z);
				tangent = glm::normalize(tangent);


				glm::vec3 bitangent;
				bitangent.x = f * (-delta_texcoords1.x * edge0.x - delta_texcoords0.x * edge1.x);
				bitangent.y = f * (-delta_texcoords1.x * edge0.y - delta_texcoords0.x * edge1.y);
				bitangent.z = f * (-delta_texcoords1.x * edge0.z - delta_texcoords0.x * edge1.z);
				bitangent = glm::normalize(bitangent);


				//1
				vertices.push_back(position0.x);
				vertices.push_back(position0.y);
				vertices.push_back(position0.z);
				vertices.push_back(texcoord0.x);
				vertices.push_back(texcoord0.y);
				vertices.push_back(normals0.x);
				vertices.push_back(normals0.y);
				vertices.push_back(normals0.z);

				vertices.push_back(tangent.x);
				vertices.push_back(tangent.y);
				vertices.push_back(tangent.z);
				vertices.push_back(bitangent.x);
				vertices.push_back(bitangent.y);
				vertices.push_back(bitangent.z);

				//2
				vertices.push_back(position1.x);
				vertices.push_back(position1.y);
				vertices.push_back(position1.z);
				vertices.push_back(texcoord1.x);
				vertices.push_back(texcoord1.y);
				vertices.push_back(normals1.x);
				vertices.push_back(normals1.y);
				vertices.push_back(normals1.z);

				vertices.push_back(tangent.x);
				vertices.push_back(tangent.y);
				vertices.push_back(tangent.z);
				vertices.push_back(bitangent.x);
				vertices.push_back(bitangent.y);
				vertices.push_back(bitangent.z);

				//3
				vertices.push_back(position2.x);
				vertices.push_back(position2.y);
				vertices.push_back(position2.z);
				vertices.push_back(texcoord2.x);
				vertices.push_back(texcoord2.y);
				vertices.push_back(normals2.x);
				vertices.push_back(normals2.y);
				vertices.push_back(normals2.z);

				vertices.push_back(tangent.x);
				vertices.push_back(tangent.y);
				vertices.push_back(tangent.z);
				vertices.push_back(bitangent.x);
				vertices.push_back(bitangent.y);
				vertices.push_back(bitangent.z);
			}
		}

		{
			model.vertex_count = vertices.size();

			// Use OpenGL to store it on the GPU
			{
				// Create a Vertex Buffer Object on the GPU
				glGenBuffers(1, &model.vbo);
				// Tell OpenGL that we want to set this as the current buffer
				glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
				// Copy all our data to the current buffer!
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
			}
		}
		// Tell the GPU how to interpret our existing vertex data
		{
			// Create a Vertex Array Object to hold the settings
			glGenVertexArrays(1, &model.vao);

			// Tell OpenGL that we want to set this as the current vertex array
			glBindVertexArray(model.vao);

			GLsizei stride = 14 * sizeof(GLfloat);

			// Tell OpenGL the settings for the current 0th vertex array!
			glVertexAttribPointer(
				0, // INDEX
				3, // size
				GL_FLOAT, // type
				GL_FALSE, // normalized
				stride, // stride (how far to the next repetition)
				(void*)0 // first component
			);


			// Enable the 0th vertex attrib array!
			glEnableVertexAttribArray(0); //PUSHING INTO LIST

			glVertexAttribPointer(
				1, // INDEX
				2, // size
				GL_FLOAT, // type
				GL_FALSE, // normalized
				stride, // stride (how far to the next repetition)
				(void*)(3 * sizeof(GLfloat)) // first component
			);
			glEnableVertexAttribArray(1); //PUSHING INTO LIST SECOND

			glVertexAttribPointer(
				2, // INDEX
				3, // size
				GL_FLOAT, // type
				GL_FALSE, // normalized
				stride, // stride (how far to the next repetition)
				(void*)(5 * sizeof(GLfloat)) // first component
			);
			glEnableVertexAttribArray(2); //PUSHING INTO LIST THIRD

			//tangent
			glVertexAttribPointer(
				3, // INDEX
				3, // size
				GL_FLOAT, // type
				GL_FALSE, // normalized
				stride, // stride (how far to the next repetition)
				(void*)(8 * sizeof(GLfloat)) // first component
			);
			glEnableVertexAttribArray(3);

			//bitangent
			glVertexAttribPointer(
				4, // INDEX
				3, // size
				GL_FLOAT, // type
				GL_FALSE, // normalized
				stride, // stride (how far to the next repetition)
				(void*)(11 * sizeof(GLfloat)) // first component
			);
			glEnableVertexAttribArray(4);

			return model;
		}
	}
};
struct Camera {

	glm::mat4 camera_from_world = glm::mat4(1);
	float fov = 60.0f;
	float near = 0.1f;
	float far = 1000.0f;

	glm::mat4 view_from_camera(GLFWwindow* window) {

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		glViewport(0.0f, 0.0f, width, height);
		return glm::perspective(
			glm::radians(this->fov),
			(float)width / (float)height,
			this->near,
			this->far
		);
	}
};

struct Particle {
	glm::mat4 world_from_model;
	glm::vec3 velocity;

	Particle(glm::mat4 world_from_model, glm::vec3 velocity) {
		this->world_from_model = world_from_model;
		this->velocity = velocity;
	}
};

GLuint compile_shader(const char* vertex_filename, const char* fragment_filename) {

	std::ifstream fragment_shader_file(fragment_filename);  // usually .vert for vertex shaders, .frag for fragment shaders 
	std::stringstream fragment_shader_stream;
	fragment_shader_stream << fragment_shader_file.rdbuf();
	std::string fragment_shader_string = fragment_shader_stream.str();
	const char* fragment_shader_c_string = fragment_shader_string.c_str();

	std::ifstream vertex_shader_file(vertex_filename);  // usually .vert for vertex shaders, .frag for fragment shaders 
	std::stringstream vertex_shader_stream;
	vertex_shader_stream << vertex_shader_file.rdbuf();
	std::string vertex_shader_string = vertex_shader_stream.str();
	const char* vertex_shader_c_string = vertex_shader_string.c_str();

	// Define some vars
	const int MAX_ERROR_LEN = 512; //making this sort of like a size for the infoLog array below 
	GLint success;
	GLchar infoLog[MAX_ERROR_LEN];

	// Compile the vertex shader
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertex_shader, 1, &vertex_shader_c_string, NULL); //passing in vertex shader src we made earlier
	glCompileShader(vertex_shader);

	// Check for errors
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, MAX_ERROR_LEN, NULL, infoLog);
		std::cout << "vertex shader compilation failed:\n" << infoLog << std::endl;
		abort();
	}

	// Compile the fragment shader
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_c_string, NULL);
	glCompileShader(fragment_shader);

	// Check for errors
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, MAX_ERROR_LEN, NULL, infoLog);
		std::cout << "fragment shader compilation failed:\n" << infoLog << std::endl;
		abort();
	}

	// Link the shaders
	GLuint shader_program = glCreateProgram();//sets shader program set to empty program object
	glAttachShader(shader_program, vertex_shader); //pass in a program (our shader_program),and vertex shader 
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	// Check for errors
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, MAX_ERROR_LEN, NULL, infoLog);
		std::cout << "shader linker failed:\n" << infoLog << std::endl;
		abort();
	}
	glDeleteShader(vertex_shader); 
	glDeleteShader(fragment_shader);

	return shader_program;
}


GLuint load_textures(GLenum active_texture, const char* filename) {

	GLuint tex;

	glGenTextures(1, &tex);
	glActiveTexture(active_texture);
	glBindTexture(GL_TEXTURE_2D, tex);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width = 0;
	int height = 0;
	GLubyte* pixels = stbi_load(filename, &width, &height, NULL, 3);

	if (pixels == NULL || width == 0 || height == 0) {
		abort();
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	return tex;
}

GLuint load_cubemap(
	GLenum active_texture,
	const char* left,
	const char* front,
	const char* right,
	const char* back,
	const char* top,
	const char* bottom
) {

	GLuint tex;

	glGenTextures(1, &tex);
	glActiveTexture(active_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	{
		int width = 0;
		int height = 0;
		GLubyte* pixels = stbi_load(right, &width, &height, NULL, 3);

		if (pixels == NULL || width == 0 || height == 0) {
			abort();
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	}

	{
		int width = 0;
		int height = 0;
		GLubyte* pixels = stbi_load(left, &width, &height, NULL, 3);

		if (pixels == NULL || width == 0 || height == 0) {
			abort();
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	}

	{
		int width = 0;
		int height = 0;
		GLubyte* pixels = stbi_load(top, &width, &height, NULL, 3);

		if (pixels == NULL || width == 0 || height == 0) {
			abort();
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}

	{
		int width = 0;
		int height = 0;
		GLubyte* pixels = stbi_load(bottom, &width, &height, NULL, 3);

		if (pixels == NULL || width == 0 || height == 0) {
			abort();
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}

	{
		int width = 0;
		int height = 0;
		GLubyte* pixels = stbi_load(front, &width, &height, NULL, 3);

		if (pixels == NULL || width == 0 || height == 0) {
			abort();
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}

	{
		int width = 0;
		int height = 0;
		GLubyte* pixels = stbi_load(back, &width, &height, NULL, 3);

		if (pixels == NULL || width == 0 || height == 0) {
			abort();
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}


	return tex;
}

void render_scene(
	GLFWwindow* window,
	std::vector<Model> *models, 
	GLuint shader_program, 
	GLuint sky_shader_program,
	Camera camera, 
	std::vector<Particle>* particles) {

	glClearColor(0.7f, 0.0f, 0.5f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthMask(GL_FALSE);

	{
		//SKY SHADER PROGRAM
		glUseProgram(sky_shader_program);

		GLint sky_box_location = glGetUniformLocation(sky_shader_program, "skybox");
		glUniform1i(sky_box_location, 3);

		GLint world_to_camera_location = glGetUniformLocation(sky_shader_program, "camera_from_world");
		GLint color_location = glGetUniformLocation(sky_shader_program, "color");
		GLint offset_location = glGetUniformLocation(sky_shader_program, "offset");
		GLint texture_location = glGetUniformLocation(sky_shader_program, "sampler2Dtex");

		GLint view_from_camera_location = glGetUniformLocation(sky_shader_program, "view_from_camera"); //newest location
		glUniformMatrix4fv(
			view_from_camera_location,
			1, 
			GL_FALSE, 
			glm::value_ptr(camera.view_from_camera(window))
		);

		glm::mat4 camera_from_world_no_translation = glm::mat4(glm::mat3(camera.camera_from_world));
		glUniformMatrix4fv(
			world_to_camera_location,
			1, 
			GL_FALSE, 
			glm::value_ptr(camera_from_world_no_translation)
		);

			Particle* particle = &(*particles)[0];

			particle->world_from_model = glm::translate(
				particle->world_from_model,
				particle->velocity
			);

			GLint world_from_model_location = glGetUniformLocation(sky_shader_program, "world_from_model");
			glUniformMatrix4fv(
				world_from_model_location,
				1,
				GL_FALSE, 
				glm::value_ptr(particle->world_from_model)
			);

			glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			position = particle->world_from_model * position;

			
			if (position.x > 2.0) {
				particle->velocity.x = -abs(particle->velocity.x);
			}
			else if (position.x < 0.0) {
				particle->velocity.x = abs(particle->velocity.x);
			}

			if (position.y > 2.0) {
				particle->velocity.y = -abs(particle->velocity.y);
			}
			else if (position.y < 0.0) {
				particle->velocity.y = abs(particle->velocity.y);
			}
			
			
			(*models)[0].draw();
	}

	//SHADER PROGRAM
	glDepthMask(GL_TRUE);

	glUseProgram(shader_program);

	GLint sky_box_location = glGetUniformLocation(shader_program, "skybox");
	glUniform1i(sky_box_location, 3);

	GLint color_location = glGetUniformLocation(shader_program, "color");
	GLint offset_location = glGetUniformLocation(shader_program, "offset");
	GLint texture_location = glGetUniformLocation(shader_program, "sampler2Dtex");

	GLuint diffuse_map_location = glGetUniformLocation(shader_program, "diffuse_map");
	glUniform1i(diffuse_map_location, 0);

	GLuint specular_map_location = glGetUniformLocation(shader_program, "specular_map");
	glUniform1i(specular_map_location, 1);

	GLuint normal_map_location = glGetUniformLocation(shader_program, "normal_map");
	glUniform1i(normal_map_location, 2);

	GLint view_from_camera_location = glGetUniformLocation(shader_program, "view_from_camera"); //newest location
	glUniformMatrix4fv(
		view_from_camera_location,
		1,
		GL_FALSE, 
		glm::value_ptr(camera.view_from_camera(window))
	);

	GLint world_to_camera_location = glGetUniformLocation(shader_program, "camera_from_world");
	glUniformMatrix4fv(
		world_to_camera_location,
		1,
		GL_FALSE, 
		glm::value_ptr(camera.camera_from_world)
	);

	for (int i = 0; i < particles->size(); ++i) {

		Particle* particle = &(*particles)[i];

		particle->world_from_model = glm::translate(
			particle->world_from_model,
			particle->velocity
		);

		GLint world_from_model_location = glGetUniformLocation(shader_program, "world_from_model");
		glUniformMatrix4fv(
			world_from_model_location,
			1,
			GL_FALSE, 
			glm::value_ptr(particle->world_from_model)
		);

		glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		position = particle->world_from_model * position;

		if (position.x > 2.0) {
			particle->velocity.x = -abs(particle->velocity.x);
		}
		else if (position.x < 0.0) {
			particle->velocity.x = abs(particle->velocity.x);
		}
		
		if (position.y > 2.0) {
			particle->velocity.y = -abs(particle->velocity.y);
		}
		else if (position.y < 0.0) {
			particle->velocity.y = abs(particle->velocity.y);
		}
		

		(*models)[i].draw();
	}
}

int main(void) {

	GLFWwindow* window = initialize_glfw();
	GLuint shader_program = compile_shader("vertex_shader.txt", "fragment_shader.txt");
	GLuint sky_shader_program = compile_shader("vertex_shader_sky.txt", "fragment_shader_sky.txt");

	std::vector<Model>models;
	models.push_back(Model::load("cube-normals.obj"));
	models.push_back(Model::load("plane.obj"));
	models.push_back(Model::load("monkey-normals.obj"));

	std::vector<GLuint> textures;

	textures.push_back(load_textures(GL_TEXTURE0, "PaintedMetal002_2K_Color.jpg"));
	textures.push_back(load_textures(GL_TEXTURE1, "PaintedMetal002_2K_Roughness.jpg"));
	textures.push_back(load_textures(GL_TEXTURE2, "PaintedMetal002_2K_Normal.jpg"));
	textures.push_back(load_cubemap(GL_TEXTURE3,
		"cubemap_sides/left.png",
		"cubemap_sides/front.png",
		"cubemap_sides/right.png",
		"cubemap_sides/back.png",
		"cubemap_sides/top.png",
		"cubemap_sides/bottom.png"
	));

	std::vector<Particle> particles;

	particles.push_back(Particle(
		glm::scale(
			glm::translate(
				glm::rotate(
					glm::mat4(1),
					0.3f,
					glm::vec3(0.0f, 1.0f, 0.0f)
				),
				glm::vec3(10.0f, 30.0f, 0.0f)
			),
			glm::vec3(3.0f, 3.0f, 3.0f)
		),//for flying up or down x y z
		glm::vec3(0.0f, 0.003f, 0.0f)
	));

	//LAST TWO OBJECTS NOT CUBE
	particles.push_back(Particle(
		glm::scale(
			glm::translate(
				glm::rotate(
					glm::mat4(1),
					0.3f,
					glm::vec3(0.0f, 1.0f, 0.0f)
				),
				glm::vec3(-10.0f, 30.0f, 0.0f)
			),
			glm::vec3(3.0f, 3.0f, 3.0f)
		),//xyz auto
		glm::vec3(0.0f, 0.006f, 0.0f)
	));
	
	particles.push_back(Particle(
		glm::scale(
			glm::translate(
				glm::rotate(
					glm::mat4(1),
					0.3f,
					glm::vec3(0.0f, 1.0f, 0.0f)
				),
				glm::vec3(0.0f, 30.0f, 0.0f)
			),
			glm::vec3(3.0f, 3.0f, 3.0f)
		),//xyz auto 
		glm::vec3(0.0f, 0.006f, 0.0f)
	));

	Camera camera;

	camera.camera_from_world = glm::translate(camera.camera_from_world,
		glm::vec3(0.0f,0.0f,-30.0f));

	//Initialize FrameBuffer
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//Get width and height on window
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	//INITIALIZ COLOR BUFFER
	GLuint fbo_color;
	glGenTextures(1, &fbo_color);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo_color);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_color, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//INITIALIZE COLOR BUFFER

	GLuint fbo_depth;

	glGenTextures(1, &fbo_depth);

	glActiveTexture(GL_TEXTURE1);

	glBindTexture(GL_TEXTURE_2D, fbo_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fbo_depth, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer incomplete!\n";
		abort();
	}

	while (!glfwWindowShouldClose(window)) {
		camera.camera_from_world = glm::rotate(
			camera.camera_from_world,
			0.002f,
			glm::vec3(0.0f, 1.0f, 0.0f)
			//rotation up/down,right/left,sideways
		);
		//FIRST RENDER
		//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		//glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//render_scene(window, &models, shader_program, sky_shader_program, camera, &particles);
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		render_scene(window, &models, shader_program, sky_shader_program, camera, &particles);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Cleanup
	glDeleteTextures(textures.size(), &textures[0]);
	glDeleteProgram(shader_program);
	glDeleteProgram(sky_shader_program);
	glDeleteFramebuffers(1, &fbo);
	for (auto i = 0; i < models.size(); ++i) {
		models[i].cleanup();
	}
	glfwTerminate();


	return 0;
}