#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

	return window; //returns window pointer type glfwwindow
}

GLuint compile_shader() { //function type unsigned int can only be positive
	// Define shader sourcecode
	//declaring constant char pointer named vertext_shader_src that equals a string 
	//we are creating two shaders the vertex_shader and the fragment shader
	//vertex is working with the vertices and the fragment shader takes care of the color
	//after we create them we link them into one shader program 
	
	//we declare the uniforms inside the src string and then change them in the render scene
	//by returning the uniform and saving it into a variable and changing the values we could also
	//use loops to change position or color by going from 0.0-1.0f
	//if you name something "offset" in the source then we pass it in as an argument later on 

	const char* vertex_shader_src = 
		"#version 330 core\n"
		"layout (location = 0) in vec3 pos;\n"
		//"layout (location = 1) in vec2 texcoords;\n"
		//"out vec2 Texcoords;\n"
		//"in vec2 Texcoords;\n"
		//"uniform vec4 color;\n"
		//

		"uniform vec2 offset;\n"

		"void main() {\n"
		"   gl_Position = vec4(pos.x + offset.x, pos.y + offset.y, pos.z, 1.0);\n"
		"}\n";

	const char* fragment_shader_src =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"uniform vec4 color;\n"
		//"uniform sampler2Dtex;\n"
		//"

		"void main() {\n" //color is the name of the vec 4 (r,g,b,a)
		//"vec2uvs = vec2(gl_FragCoord)/100.00;\n"
		"   FragColor = color;\n"
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


void load_geometry(GLuint* vao, GLuint* vbo, GLsizei* vertex_count) {
	// Send the vertex data to the GPU
	{
		// Generate the data on the CPU
		GLfloat vertices[] = {
			0.0f, 0.5f, 0.0f, // top center
			0.0f, 0.0f, 0.0f, // bottom right
			0.5f, 0.0f, 0.0f, // bottom left

			/*
			0.0f, 0.5f, 0.0f, // top center
			0.5, 0.5f, 0.0f, // bottom right
			0.5f, 0.0f, 0.0f, // bottom left
			*/
		};
		*vertex_count = sizeof(vertices) / sizeof(vertices[0]);

		// Use OpenGL to store it on the GPU
		{
			// Create a Vertex Buffer Object on the GPU
			glGenBuffers(1, vbo);
			// Tell OpenGL that we want to set this as the current buffer
			glBindBuffer(GL_ARRAY_BUFFER, *vbo);
			// Copy all our data to the current buffer!
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		}
	}

	// Tell the GPU how to interpret our existing vertex data
	{
		// Create a Vertex Array Object to hold the settings
		glGenVertexArrays(1, vao);

		// Tell OpenGL that we want to set this as the current vertex array
		glBindVertexArray(*vao);

		// Tell OpenGL the settings for the current 0th vertex array!
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

		// Enable the 0th vertex attrib array!
		glEnableVertexAttribArray(0);
	}
}

//we could use the uniforms here to create the offset while still using the same triangle
//template 

void render_scene(GLFWwindow* window, GLsizei vertex_count, GLuint shader_program) {

	static float red = 0.0f;
	static float dir = 1.0f;

	static float incrementer = 0.0f;
	static float dir2 = 1.0;


	glClearColor(0.7f, 0.0f, 0.5f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);
	
	if (dir > 0.0) {
		red += 0.001f;
		if (red > 1.0) {
			dir = -1.0;
		}
	}
		else {
			red -= 0.001f;
		}if (red < 0.0) {
			dir = 1.0;
		}

	GLint color_location = glGetUniformLocation(shader_program, "color");
	GLint offset_location = glGetUniformLocation(shader_program, "offset");
	GLint texture_location = glGetUniformLocation(shader_program, "sampler2Dtex");

	glUniform4f(color_location, red, 0.0, 0.0, 1.0); //red, green, blue
	glUniform2f(offset_location, red, 0.5); //x and y coordinates
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);

	glUniform4f(color_location, 0.0, 0.0, red, 1.0); //blue triangle
	glUniform2f(offset_location, incrementer, red);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);
	

	// Display the results on screen
	glfwSwapBuffers(window);

}

void cleanup(GLFWwindow* window) {//takes in window pointer into the argument
	// Call glfw terminate here
	glfwTerminate(); //this terminates the glfw library after we are done
	
}

int main(void) {

	//initialize and empty shader program in the main the set it 
	//have to get the shader program by doing GLuint shade_program =shade_compiler();
	//pass this shade program into the render scene in the loop 
	//this is the exact shade_program that we created in the function
	//also create argument in the render scene function for shader_program so
	//render_scene(window,vertex_count,shader_program)
	 //sets shader_program to the return of compile_shader()
	 
	GLuint vao, vbo; //unsigned int that can only be positive and we are creating a vertex array object and vertex buffer object
	GLsizei vertex_count;  //an int used for size 
	GLFWwindow* window = initialize_glfw(); //a pointer object for window that equals the glfw function
	GLuint shader_program = compile_shader();

	//these are all uninitialized at thr start but when they are passed into the function then 
	//the values are changed because they are by reference into the functions above.

	compile_shader(); //calls compile_shader
	load_geometry(&vao,&vbo,&vertex_count); //calls load geometry and we pass in vao,vbo,and vertex_count by reference

	while (!glfwWindowShouldClose(window)){
		render_scene(window, vertex_count,shader_program);//calls render scene and passes in window pointer and vertex count
		glfwPollEvents();
	}

	cleanup(window);

	return 0;
}
