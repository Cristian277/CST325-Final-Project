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
	// Initialize the context
	if (!glfwInit()) { //cancels the application if it can not open the window
		std::cout << "glfwInit(...) failed\n";
		abort();
	}

	// Ask for a core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window
	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL); //setting the window pointer to be width 640, height 480, and title hello world
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

				} else if (type == "vn") {
					//its a vertex position
					GLfloat x;
					GLfloat y;
					GLfloat z;
					line_stream >> x;//converts first line into float
					line_stream >> y;
					line_stream >> z;
					normals_vec.push_back(glm::vec3(x, y, z));

				} else if (type == "vt") {

					GLfloat u, v;
					line_stream >> u;
					line_stream >> v;
					texcoords.push_back(glm::vec2(u, v));

					//std::cout << "its a vertex texcoord\n";
				}
				else if (type == "f") {
					//std::cout << "its a face\n";
					std::string face;
					for (int i = 0; i < 3; ++i) {

						line_stream >> face;

						std::replace(face.begin(), face.end(), '/', ' ');
						std::istringstream face_stream(face);

						size_t position_index;
						size_t texcoord_index;
						size_t normal_index;

						face_stream >> position_index;
						face_stream >> texcoord_index;
						face_stream >> normal_index;

						glm::vec3 position = positions.at(position_index - 1);
						glm::vec2 texcoord = texcoords.at(texcoord_index - 1);
						glm::vec3 normals = normals_vec.at(normal_index - 1);

						vertices.push_back(position.x);
						vertices.push_back(position.y);
						vertices.push_back(position.z);
						vertices.push_back(texcoord.x);
						vertices.push_back(texcoord.y);
						vertices.push_back(normals.x);
						vertices.push_back(normals.y);
						vertices.push_back(normals.z);


					}


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
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* vertices.size(),&vertices[0], GL_STATIC_DRAW);
			}

		}
		

		// Tell the GPU how to interpret our existing vertex data
		{
			// Create a Vertex Array Object to hold the settings
			glGenVertexArrays(1, &model.vao);

			// Tell OpenGL that we want to set this as the current vertex array
			glBindVertexArray(model.vao);

			GLsizei stride = 8 * sizeof(GLfloat);

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

GLuint compile_shader() {  

	const char* vertex_shader_src =
		"#version 330 core\n"
		"layout (location = 0) in vec3 pos;\n" //POSITION IN OUR ATTRIBUTES LOCATION/INDEX 0
		"layout (location = 1) in vec2 texcoords;\n" //POSITION/INDEX 1
		"layout (location = 2) in vec3 normal;\n"
		"out vec2 Texcoords;\n"
		"out vec3 Normal;\n"
		"out vec3 world_space_position;\n"
		"out vec3 world_space_camera_position;\n"
		"uniform vec2 offset;\n"
		"uniform mat4 camera_from_world;\n"
		"uniform mat4 view_from_camera;\n"
		"uniform mat4 world_from_model;\n"

		"void main() {\n"
			"Texcoords = texcoords;\n"
			"Normal = mat3(transpose(inverse(world_from_model)))*normal;\n" //sets the vec3 normal to the vec3 normal layout position attribute
		"   gl_Position = view_from_camera*camera_from_world *world_from_model* vec4(pos,1.0);\n" //1 at the end is for matrix mult
		"	world_space_position = vec3(world_from_model * vec4(pos,1.0));"
		"	mat4 world_from_camera = inverse(camera_from_world);\n"
		"	world_space_camera_position = vec3(world_from_camera * vec4(0.0,0.0,0.0,1.0));\n"
		"}\n";

	const char* fragment_shader_src =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"uniform vec4 color;\n"
		"in vec2 Texcoords;\n"
		"in vec3 Normal;\n" //make a in vec3 Normal while the top is out
		"in vec3 world_space_position;\n"
		"in vec3 world_space_camera_position;\n"
		"uniform sampler2D tex;\n"

		"void main() {\n" //color is the name of the vec 4 (r,g,b,a)
			
			//Light settings
			"vec3 light_dir = vec3(1.0,1.0,1.0);\n"
			"vec3 light_color = vec3(0.4,0.0,0.2);\n"
			"vec3 specular_color = 0.2 * vec3(1.0,1.0,1.0);\n"
			"vec3 normal = normalize(Normal);\n"

			//Ambient lighting
			"vec3 ambient = vec3(0.2,0.0,0.2);\n"

			//"vec2 uvs = vec2(gl_FragCoord) / 100.0;\n"
			//"float fog = gl_FragCoord.z/gl_FragCoord.w;\n"
			
			//Diffuse lighting
			"float diffuse_intensity = max(dot(normal,light_dir),0.0);\n" //doesn't take negative numbers
			"vec3 diffuse = diffuse_intensity * light_color;\n"
			
			//Specular lighting

			"vec3 view_dir = normalize(world_space_camera_position - world_space_position);\n"
			"vec3 reflect_direction = reflect(-light_dir,normal);\n"
			"float specular_intensity = pow(max(dot(view_dir,reflect_direction),0.0),4.0);\n"
			"vec3 specular = specular_intensity * specular_color;\n"

			//Final output
			"FragColor = vec4(ambient + diffuse + specular,1.0);\n"
			//"FragColor = vec4(0.5 * world_space_position + vec3(1.0),1.0);\n"

		"}\n";

	// Define some vars
	const int MAX_ERROR_LEN = 512; //making this sort of like a size for the infoLog array below 
	GLint success;
	GLchar infoLog[MAX_ERROR_LEN];

	// Compile the vertex shader
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL); //passing in vertex shader src we made earlier
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
	glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
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
	glDeleteShader(vertex_shader); //deletes vertex and fragment shaders 
	glDeleteShader(fragment_shader);

	// Enable the shader program here since we only have one
	glUseProgram(shader_program);

	return shader_program;
}


//we could use the uniforms here to create the offset while still using the same triangle
//template 


GLuint load_textures(){

	glActiveTexture(GL_TEXTURE0);

	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load("distressed-texture-2.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return texture;
}

void render_scene(GLFWwindow* window, Model model, GLuint shader_program, Camera camera, std::vector<Particle>*particles) {

	static float red = 0.0f;
	static float dir = 1.0f;

	static float incrementer = 0.0f;
	static float dir2 = 1.0;

	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if (dir > 0.0) {
		red += 0.001f;
		if (red > 1.0) {
			dir = -1.0;
		}
	}
	else {
		red -= 0.001f;
	}
	if (red < 0.0) {
		dir = 1.0;
	}

	GLint world_to_camera_location = glGetUniformLocation(shader_program, "camera_from_world");
	GLint color_location = glGetUniformLocation(shader_program, "color");
	GLint offset_location = glGetUniformLocation(shader_program, "offset");
	GLint texture_location = glGetUniformLocation(shader_program, "sampler2Dtex");
	GLuint tex_location = glGetUniformLocation(shader_program, "tex");
	GLint view_from_camera_location = glGetUniformLocation(shader_program, "view_from_camera"); //newest location

	//4x4 matrix filled with floats
	glUniformMatrix4fv(
		world_to_camera_location,
		1, // count
		GL_FALSE, // transpose
		glm::value_ptr(camera.camera_from_world)
	);

	glUniformMatrix4fv(
		view_from_camera_location,
		1, // count
		GL_FALSE, // transpose
		glm::value_ptr(camera.view_from_camera(window))
	);

	for (int i = 0; i < particles->size(); ++i) {

		Particle* particle = &(*particles)[i];

		particle->world_from_model = glm::translate(
			particle->world_from_model,
			particle->velocity
		);

		particle->world_from_model=glm::rotate(
			particle->world_from_model,
			0.001f,
			glm::vec3(1.0f, 1.0f, 0.0f)
		);

		GLint world_from_model_location = glGetUniformLocation(shader_program, "world_from_model");
		glUniformMatrix4fv(
			world_from_model_location,
			1, // count
			GL_FALSE, // transpose
			glm::value_ptr(particle->world_from_model)
		);
		
		model.draw();

	}

	// Display the results on screen
	glfwSwapBuffers(window);

}

void cleanup(GLuint shader_program, Model model, GLuint tex) {//takes in window pointer into the argument
	// Call glfw terminate here
	glDeleteTextures(1, &tex);
	glDeleteProgram(shader_program);
	model.cleanup();
	glfwTerminate(); //this terminates the glfw library after we are done
}

int main(void) {

	GLFWwindow* window = initialize_glfw(); //a pointer object for window that equals the glfw function
	GLuint shader_program = compile_shader();
	GLuint tex = load_textures();

	std::string objectFileName="monkey-normals.obj";
	//std::string objectFileName2 = "cube.obj";

	//these are all uninitialized at the start but when they are passed into the function then 
	//the values are changed because they are by reference into the functions above.

	Model model = Model::load(objectFileName);

	std::vector<Particle>particles;

	particles.push_back(Particle(
		glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, 0.0f)),
		glm::vec3(0.0f, 0.0f, 0.0f)
	));

	//calls load geometry and we pass in vao,vbo,and vertex_count by reference
	
	Camera camera; // init to the identity matrix
	camera.camera_from_world = glm::translate(camera.camera_from_world, 
		glm::vec3(0.0f, 0.0f, -3.0f));

	//need to create a view_from_camera and pass it into the render scene 
	//and make the view_from camera equal to the function inside the camera struct

	while (!glfwWindowShouldClose(window)) {

		//camera from world is being changed here before it is being passed into the render_scene
		render_scene(window, model, shader_program,camera,&particles);

		glfwPollEvents();
	}

	cleanup(shader_program,model,tex);

	return 0;
}