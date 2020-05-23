#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "block.h"

//*************************************
// global constants
static const char*	window_name = "team dreamlike";
static const char*	vert_shader_path = "shaders/dreamlike.vert";
static const char*	frag_shader_path = "shaders/dreamlike.frag";

GLuint	block_vertex_array = 0;	// ID holder for vertex array object
auto	blocks = std::move(create_blocks());

float	t = 0.0f;						

bool ctrl_pressed = 0;
bool shift_pressed = 0;
int	wire_mode = 0;

bool	m_tracking = false;
bool	p_tracking = false;
bool	z_tracking = false;

vec2	m0;				// the last mouse position
vec3	eye0, at0, up0; // the last eye at up position


//*************************************
// common structures
struct camera
{
	vec3	eye = vec3( 20,20,float(sqrt(800)) );
	vec3	at = vec3( 0, 0, 0 );
	vec3	up = vec3( 0, 0, 1 );
	mat4	view_matrix = mat4::look_at( eye, at, up );

	float	fovy = PI/4.0f; // must be in radian
	float	aspect;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix =
	{	1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
};

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program

//*************************************
// global variables
int		frame = 0;				// index of rendering frames

//*************************************
// scene objects
mesh*		pMesh = nullptr;
camera		cam;

// utility function
vec2 cursor_to_ndc(dvec2 cursor, ivec2 window_size)
{
	// normalize window pos to [0,1]^2
	vec2 npos = vec2(float(cursor.x) / float(window_size.x - 1),
		float(cursor.y) / float(window_size.y - 1));

	// normalize window pos to [-1,1]^2 with vertical flipping
	// vertical flipping: window coordinate system defines y from
	// top to bottom, while the trackball from bottom to top
	return vec2(npos.x * 2.0f - 1.0f, 1.0f - npos.y * 2.0f);
}

// Orthogonal Projection Function.
mat4 ortho( float fovy, float aspect, float dnear, float dfar )
{
	float t = 1 / tan(fovy / 2),	b = -t;
	float l = -t * aspect,			r = -l;
	float n = dnear,				f = dfar;
	mat4 matrix;
	//matrix->_11 = 2 / (r - l); matrix->_12 = 0; matrix->_13 = 0; matrix->_14 = (l + r) / (l - r);
	//matrix->_21 = 0; matrix->_22 = 2 / (t - b); matrix->_23 = 0; matrix->_24 = (b + t) / (b - t);
	//matrix->_31 = 0; matrix->_32 = 0; matrix->_33 = 2 / (n - f); matrix->_34 = (n + f) / (n - f);
	//matrix->_41 = 0; matrix->_42 = 0; matrix->_43 = 0; matrix->_44 = 1;
	matrix._11 = 2 / (r - l); matrix._12 = 0; matrix._13 = 0; matrix._14 = (l + r) / (l - r);
	matrix._21 = 0; matrix._22 = 2 / (t - b); matrix._23 = 0; matrix._24 = (b + t) / (b - t);
	matrix._31 = 0; matrix._32 = 0; matrix._33 = 2 / (n - f); matrix._34 = (n + f) / (n - f);
	matrix._41 = 0; matrix._42 = 0; matrix._43 = 0; matrix._44 = 1;
	return matrix;
}

//*************************************
void update()
{

	t = float(glfwGetTime());

	// update projection matrix
	cam.aspect = window_size.x/float(window_size.y);
	//cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect, cam.dnear, cam.dfar );
	cam.projection_matrix = ortho( cam.fovy, cam.aspect, cam.dnear, cam.dfar );

	// build the model matrix for oscillating scale
	float t = float(glfwGetTime());
	float scale	= 1;
	mat4 model_matrix = mat4::scale( scale, scale, scale );

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
	
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// notify GL that we use our own program
	glUseProgram(program);

	// bind vertex array object
	glBindVertexArray(block_vertex_array);

	// render circles: trigger shader program to process vertex data
	for (auto& s : blocks)
	{
		s.update(t);
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, s.model_matrix);
		glDrawElements(GL_TRIANGLES, 101 * 100 * 6, GL_UNSIGNED_INT, nullptr);
	}

	// swap front and back buffers, and display to screen
	glfwSwapBuffers(window);
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press Home to reset camera\n" );
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)		ctrl_pressed = 1;
		else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)		shift_pressed = 1;
		else if (key == GLFW_KEY_HOME)				cam = camera();
		else if (key == GLFW_KEY_W) {
			wire_mode = !wire_mode;
			glPolygonMode(GL_FRONT_AND_BACK, wire_mode ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", wire_mode ? "wireframe" : "solid");
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)		ctrl_pressed = 0;
		else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)		shift_pressed = 0;
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_MIDDLE || (button==GLFW_MOUSE_BUTTON_LEFT && ctrl_pressed))
	{
		//panning
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		vec2 npos = cursor_to_ndc( pos, window_size );
		if (action == GLFW_PRESS) {
			p_tracking = true;			// enable trackball tracking
			eye0 = cam.eye;
			at0 = cam.at;
			up0 = cam.up;
			m0 = npos;
		}
		else if (action == GLFW_RELEASE) {
			p_tracking = false;			// enable trackball tracking
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT || (button == GLFW_MOUSE_BUTTON_LEFT && shift_pressed))
	{
		//zooming
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS) {
			z_tracking = true;
			eye0 = cam.eye;
			at0 = cam.at;
			up0 = cam.up;
			m0 = npos;
		}
		else if (action == GLFW_RELEASE) {
			z_tracking = false;	
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		//rotating
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS) {
			m_tracking = true;
			eye0 = cam.eye;
			at0 = cam.at;
			up0 = cam.up;
			m0 = npos;
			printf("%f, %f, %f\n", cam.eye.x, cam.eye.y, cam.eye.z);
		}
		else if (action == GLFW_RELEASE) {
			m_tracking = false;
		}
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if (z_tracking) {
		//zooming
		vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);
		float p = (npos.y - m0.y);
		if ((p < 0.0001f && p >0)  || (p<0 && p>-0.0001f) ) return;		// ignore subtle movement
		if (p > 1.0f || p < -1.0f) return;
		mat4 tmp = mat4::look_at(eye0, at0, up0);
		cam.eye = eye0 + vec3(tmp._31, tmp._32, tmp._33) * p * 10;
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
	}
	else if(m_tracking) {
		// trackball
		vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);
		vec2 temp = (npos - m0) * -20;
		float distance = length(eye0 - at0);
		float yflag = 0;
		if (temp.y >= 0) yflag = 1; else yflag = -1;

		float theta = atan(temp.x / temp.y);
		float ztheta = atan( yflag*length(temp) / distance);

		mat4 tmp = mat4::look_at(eye0, at0, up0);
		vec3 u_frame = vec3(tmp._11, tmp._12, tmp._13);
		vec3 v_frame = vec3(tmp._21, tmp._22, tmp._23);
		vec3 n_frame = vec3(tmp._31, tmp._32, tmp._33);
		cam.eye = at0 + (
			u_frame * sin(ztheta) * sin(theta)
			+ v_frame * sin(ztheta) * cos(theta)
			+ n_frame * cos(ztheta)
			) * distance;
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

	}
	else if (p_tracking) {
		//panning
		vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);
		float px = (npos.x - m0.x)*20;					
		float py = (npos.y - m0.y)*20;					
		if (length(vec2(px,py)) < 0.0001f) return;		// ignore subtle movement
		mat4 tmp = mat4::look_at(eye0, at0, up0);
		cam.eye = eye0
			+ vec3(tmp._11, tmp._12, tmp._13) * px
			+ vec3(tmp._21, tmp._22, tmp._23) * py;

		cam.at = at0
			+ vec3(tmp._11, tmp._12, tmp._13) * px
			+ vec3(tmp._21, tmp._22, tmp._23) * py;

		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

	}

}


void create_block_vertex_array()
{
	std::vector<uint> indices;
	std::vector<vertex> vertices;

	//create vertices
	vertices.push_back({ vec3(-0.5,-0.5,-0.5), vec3(145.0f/255, 162.0f/255, 1.0f), vec2(0,0) });
	vertices.push_back({ vec3(0.5,-0.5,-0.5), vec3(145.0f/255, 162.0f/255, 1.0f), vec2(0,0) });
	vertices.push_back({ vec3(0.5,0.5,-0.5), vec3(145.0f/255, 162.0f/255, 1.0f), vec2(0,0) });
	vertices.push_back({ vec3(-0.5,0.5,-0.5), vec3(145.0f/255, 162.0f/255, 1.0f), vec2(0,0) });

	vertices.push_back({ vec3(-0.5,-0.5,0.5), vec3(145.0f/255, 162.0f/255, 1.0f), vec2(0,0) });
	vertices.push_back({ vec3(0.5,-0.5,0.5), vec3(145.0f/255, 162.0f/255, 1.0f), vec2(0,0) });
	vertices.push_back({ vec3(0.5,0.5,0.5), vec3(145.0f/255, 162.0f/255, 1.0f), vec2(1,1) });
	vertices.push_back({ vec3(-0.5,0.5,0.5), vec3(145.0f/255, 162.0f/255, 1.0f), vec2(0,0) });

	//create indices
	//bottom
	indices.push_back(0); indices.push_back(2); indices.push_back(1);
	indices.push_back(0); indices.push_back(3); indices.push_back(2);
	//top
	indices.push_back(4); indices.push_back(5); indices.push_back(6);
	indices.push_back(4); indices.push_back(6); indices.push_back(7);
	//front
	indices.push_back(5); indices.push_back(1); indices.push_back(2);
	indices.push_back(5); indices.push_back(2); indices.push_back(6);
	//left
	indices.push_back(4); indices.push_back(0); indices.push_back(5);
	indices.push_back(5); indices.push_back(0); indices.push_back(1);
	//right
	indices.push_back(6); indices.push_back(2); indices.push_back(3);
	indices.push_back(6); indices.push_back(3); indices.push_back(7);
	//back
	indices.push_back(7); indices.push_back(3); indices.push_back(0);
	indices.push_back(7); indices.push_back(0); indices.push_back(4);

	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (block_vertex_array) glDeleteVertexArrays(1, &block_vertex_array);
	block_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
}


bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests

	// load the mesh
	create_block_vertex_array();

	return true;
}



void user_finalize()
{
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
