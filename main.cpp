#include "cgmath.h"		// slee's Simple Math Library.
#include "cgut.h"		// slee's OpenGL Utility.
#include "trackball.h"	// Virtual Trackball.
#include "planet.h"
#include "animation.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//*************************************
// Global Constants.
static const char* window_name = "2014314636 Yul's Planets";
static const char* vert_shader_path = "../bin/shaders/planet.vert";
static const char* frag_shader_path = "../bin/shaders/planet.frag";

//*************************************
// Common Structures.
vec3 default_eye = vec3(400, 0, 200);
vec3 default_at = vec3(0, 0, 0);
vec3 default_up = vec3(0, 0, 1);
struct camera
{
	vec3	eye = default_eye;
	vec3	at = default_at;
	vec3	up = default_up;
	mat4	view_matrix = mat4::look_at(eye, at, up);

	float	fovy = PI / 2.0f; // must be in radian
	float	aspect;
	float	dnear = 100.0f;
	float	dfar = -100.0f;
	mat4	projection_matrix;
};

//*************************************
// Window Objects.
GLFWwindow* window = nullptr;
ivec2		window_size = ivec2(1280, 720);	// Initial Window Size.
//cg_default_window_size(); 

//*************************************
// OpenGL Objects.
GLuint	program = 0;		// ID Holder for GPU Program.
GLuint	vertex_array = 0;	// ID Holder for Vertex Array Object.

//*************************************
// Global Variables.
int		frame = 0;			// Index of Rendering Frames.
float	common_time = 0.0f;	// Save Common Time for Rendering.
#ifndef GL_ES_VERSION_2_0
bool	b_wireframe = false;
#endif
bool	b_shift = false;
bool	b_ctrl = false;
bool	b_tracking = false;
bool	b_zooming = false;
bool	b_moving = false;

//*************************************
// Scene Objects.
camera		cam;
trackball	tb;

//*************************************
// Holder of Vertices and Indices of a Unit Circle.
std::vector<vertex>	unit_planet_vertices;		// Host-side Vertices.
std::vector<vertex>	unit_satellite_vertices;	// Host-side Vertices.
std::vector<vertex>	unit_total_vertices;		// Host-side Vertices.

//*************************************
void update()
{
	// Update Global Simulation Parameter
	common_time = float(glfwGetTime()) * 0.4f;

	// Tricky Aspect Correction Matrix for Non-Square Window.
	float aspect = window_size.x / float(window_size.y);
	mat4 aspect_matrix =
	{
		min(1 / aspect,1.0f), 0, 0, 0,
		0, min(aspect,1.0f),   0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	// Update Projection Matrix.
	cam.aspect = window_size.x / float(window_size.y);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect, cam.dnear, cam.dfar);

	// Update Uniform Variables in Vertex/Fragment Shaders.
	GLint uloc;
	uloc = glGetUniformLocation(program, "view_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
	uloc = glGetUniformLocation(program, "projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);
	uloc = glGetUniformLocation(program, "aspect_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, aspect_matrix);
}

void render()
{
	//GLint uloc;
	// Clear Screen (with Background Color) and Clear Depth Buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Notify GL that we use our own Program.
	glUseProgram(program);

	// Bind Vertex Array.
	glBindVertexArray(vertex_array);

	for (int i = 0; i < PLANET_COUNT; i++)
	{
		// Update the Uniform Model Matrix and Render.
		planet_list[i].update(common_time);
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, planet_list[i].model_matrix);

		// Pass Variables to '.vert' File.
		glDrawElements(GL_TRIANGLES, 3 * (NUM_TESS + 1 + 22) * (NUM_TESS + 1), GL_UNSIGNED_INT, nullptr);
	}
	for (int i = 0; i < SATELLITE_COUNT; i++)
	{
		int planet_enum = satellite_list[i].planet_enum;
		// Update the Uniform Model Matrix and Render.
		satellite_list[i].update(common_time, planet_list[planet_enum].center);
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, satellite_list[i].model_matrix);

		// Pass Variables to '.vert' File.
		glDrawElements(GL_TRIANGLES, 3 * (NUM_TESS + 1) * (NUM_TESS + 1), GL_UNSIGNED_INT, nullptr);
	}

	// Swap Front and Back Buffers, and Display to Screen.
	glfwSwapBuffers(window);
}

void reshape(GLFWwindow* window, int width, int height)
{
	// Set Current Viewport in Pixels (win_x, win_y, win_width, win_height)
	// Viewport: the Window Area that are affected by Rendering 
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}

void print_help()
{
	printf("[help]\n");
	printf("- press ESC or 'q' to terminate the program\n");
	printf("- press F1 or 'h' to see help\n");
	printf("- press 'w' to toggle wireframe\n");
	printf("- press Home to reset camera\n");
	printf("- press Pause to pause the simulation\n");
	printf("\n");
	printf("+ Alpha!!\n");
	printf("- None... Only Moon\n");
	printf("\n");
}

void update_vertex_buffer(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID Holder for Vertex Buffer.
	static GLuint index_buffer = 0;		// ID Holder for Index Buffer.

	// Clear and Create New Buffers.
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// Check Exceptions.
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// Create Buffers.
	std::vector<uint> indices;
	for (uint n = 0; n < PLANET_COUNT; n++)
	{
		uint planet_n = (3 * (NUM_TESS + 1) * (NUM_TESS + 1)) * n;
		planet_n = 0;
		printf("%u\n", planet_n);
		// Circle through XY-Frame.
		for (uint x_k = 0; x_k < NUM_TESS; x_k++)
		{
			uint left = x_k * (NUM_TESS / 2 + 1 + 11), right = (x_k + 1) * (NUM_TESS / 2 + 1 + 11);
			// Circle through Lower Edge.
			for (uint z_k = 0; z_k < NUM_TESS / 4; z_k++)
			{
				indices.push_back(planet_n + left + z_k);
				indices.push_back(planet_n + right + z_k);
				indices.push_back(planet_n + right + z_k + 1);
				indices.push_back(planet_n + left + z_k + 1);
				indices.push_back(planet_n + left + z_k);
				indices.push_back(planet_n + right + z_k + 1);
			}
			// Pilar;
			for (uint z_k = NUM_TESS / 4; z_k < NUM_TESS / 4 + 11; z_k++)
			{
				indices.push_back(planet_n + left + z_k);
				indices.push_back(planet_n + right + z_k);
				indices.push_back(planet_n + right + z_k + 1);
				indices.push_back(planet_n + left + z_k + 1);
				indices.push_back(planet_n + left + z_k);
				indices.push_back(planet_n + right + z_k + 1);
			}
			// Circle through Lower Edge.
			for (uint z_k = NUM_TESS / 4 + 11; z_k < NUM_TESS / 2 + 11; z_k++)
			{
				indices.push_back(planet_n + left + z_k);
				indices.push_back(planet_n + right + z_k);
				indices.push_back(planet_n + right + z_k + 1);
				indices.push_back(planet_n + left + z_k + 1);
				indices.push_back(planet_n + left + z_k);
				indices.push_back(planet_n + right + z_k + 1);
			}
		}
	}

	// Generation of Vertex Buffer: Use Vertices as it is.
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Geneation of Index Buffer.
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// Generate Vertex Array Object, which is Mandatory for OpenGL 3.3 and Higher.
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
		}
		else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)
		{
			b_shift = true;
		}
		else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)
		{
			b_ctrl = true;
		}
		else if (key == GLFW_KEY_HOME)
		{
			cam.eye = default_eye;
			cam.at = default_at;
			cam.up = default_up;
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
		}
		else if (key == GLFW_KEY_PAUSE)
			b_pause = !b_pause;
	}
	else
	{
		b_shift = false;
		b_ctrl = false;
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 temp = vec2(float(pos.x), float(pos.y));
		if (action == GLFW_PRESS)			tb.move_begin(temp);
		else if (action == GLFW_RELEASE)	tb.move_end();
	}
	else if (b_ctrl)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
			vec2 temp = vec2(float(pos.x), float(pos.y));
			if (action == GLFW_PRESS)			tb.move_begin(temp);
			else if (action == GLFW_RELEASE)	tb.move_end();
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
			if (action == GLFW_PRESS)			tb.zoom_begin(float(pos.y));
			else if (action == GLFW_RELEASE)	tb.zoom_end();
		}
	}
	else if (b_shift)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
			if (action == GLFW_PRESS)			tb.zoom_begin(float(pos.y));
			else if (action == GLFW_RELEASE)	tb.zoom_end();
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 temp = vec2(float(pos.x), float(pos.y));
		if (action == GLFW_PRESS)			tb.track_begin(temp);
		else if (action == GLFW_RELEASE)
		{
			if (tb.is_tracking())		tb.track_end();
			else if (tb.is_zooming())	tb.zoom_end();
			else if (tb.is_moving())	tb.move_end();
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		if (action == GLFW_PRESS)			tb.zoom_begin(float(pos.y));
		else if (action == GLFW_RELEASE)
		{
			if (tb.is_tracking())		tb.track_end();
			else if (tb.is_zooming())	tb.zoom_end();
			else if (tb.is_moving())	tb.move_end();
		}
	}
}

void motion(GLFWwindow* window, double x, double y)
{
	if (!tb.is_tracking() && !tb.is_zooming() && !tb.is_moving()) return;
	else if (tb.is_tracking())
	{
		vec2 temp = tb.track_update(vec2(float(x), float(y)));
		vec3 u_frame;
		vec3 v_frame;
		vec3 n_frame;
		float distance = length(cam.eye - cam.at);

		u_frame = vec3(cam.view_matrix._11, cam.view_matrix._12, cam.view_matrix._13);
		n_frame = vec3(cam.view_matrix._31, cam.view_matrix._32, cam.view_matrix._33);
		cam.eye = cam.at + (u_frame * sin(temp.x/100) + n_frame * cos(temp.x/100)) * distance;
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

		v_frame = vec3(cam.view_matrix._21, cam.view_matrix._22, cam.view_matrix._23);
		n_frame = vec3(cam.view_matrix._31, cam.view_matrix._32, cam.view_matrix._33);
		cam.eye = cam.at + (v_frame * sin(-temp.y/100) + n_frame * cos(-temp.y/100)) * distance;
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
	}
	else if (tb.is_zooming())
	{
		vec3 n_frame = vec3(cam.view_matrix._31, cam.view_matrix._32, cam.view_matrix._33);
		float distance = length(cam.eye - cam.at) / 50;
		float diff = tb.zoom_update(float(y));

		if (diff < 0.0f)
			cam.eye += n_frame * distance;
		else if (diff > 0.0f)
			cam.eye -= n_frame * distance;
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
	}
	else if (tb.is_moving())
	{
		vec2 temp = tb.move_update(vec2(float(x), float(y)));
		float distance = length(cam.eye - cam.at) / 500;
		vec3 u_frame = vec3(cam.view_matrix._11, cam.view_matrix._12, cam.view_matrix._13);
		vec3 v_frame = vec3(cam.view_matrix._21, cam.view_matrix._22, cam.view_matrix._23);
		cam.eye += (u_frame * -temp.x + v_frame * temp.y) * distance;
		cam.at += (u_frame * -temp.x * 2.0f + v_frame * temp.y) * distance;
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
	}
}

bool user_init()
{
	// Log Hotkeys.
	print_help();

	// Init GL States.
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);	// Set Clear Color.
	glEnable(GL_CULL_FACE);										// Turn on Backface Culling.
	glEnable(GL_DEPTH_TEST);									// Turn on Depth Tests.

	create_planet_buffer();
	create_satellite_buffer();
	load_planet_textures();
	load_satellite_textures();

	unit_planet_vertices = std::move(create_planet_vertices());
	unit_satellite_vertices = std::move(create_satellite_vertices());
	unit_total_vertices.insert(std::end(unit_total_vertices),
		std::begin(unit_planet_vertices), std::end(unit_planet_vertices));
	unit_total_vertices.insert(std::end(unit_total_vertices),
		std::begin(unit_satellite_vertices), std::end(unit_satellite_vertices));
	update_vertex_buffer(unit_total_vertices);

	return true;
}

void user_finalize()
{
}

int main(int argc, char* argv[])
{
	// Create Window and Initialize OpenGL Extensions.
	if (!(window = cg_create_window(window_name, window_size.x, window_size.y))) { glfwTerminate(); return 1; }
	if (!cg_init_extensions(window)) { glfwTerminate(); return 1; }	// Version and Extensions.

	// Initializations and Validations.
	if (!(program = cg_create_program(vert_shader_path, frag_shader_path))) { glfwTerminate(); return 1; }	// Create and Compile Shaders/Program.
	if (!user_init()) { printf("Failed to user_init()\n"); glfwTerminate(); return 1; }						// User Initialization.

	// Register Event Callbacks.
	glfwSetWindowSizeCallback(window, reshape);	// Callback for Window Resizing Events.
	glfwSetKeyCallback(window, keyboard);			// Callback for Keyboard Events.
	glfwSetMouseButtonCallback(window, mouse);	// Callback for Mouse Click Inputs.
	glfwSetCursorPosCallback(window, motion);		// Callback for Mouse Movement.

	// Enters Rendering/Event Loop.
	for (frame = 0; !glfwWindowShouldClose(window); frame++)
	{
		glfwPollEvents();	// Polling and Processing of Events.
		update();			// Per-Frame Update.
		render();			// Per-Frame Render.
	}

	// Normal Termination.
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
