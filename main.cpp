#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "assimp_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "block.h"
#include "trigger.h"
#include "character.h"

//*************************************
// global constants
static const char*	window_name = "team dreamlike";
static const char*	vert_shader_path = "../dreamlike.vert";
static const char*	frag_shader_path = "../dreamlike.frag";

GLuint	block_vertex_array = 0;	// ID holder for block vertex array object
GLuint	sphere_vertex_array_x = 0;	// ID holder for sphere vertex array object
GLuint	sphere_vertex_array_y = 0;	// ID holder for sphere vertex array object
GLuint	sphere_vertex_array_z = 0;	// ID holder for sphere vertex array object

GLuint	trigger_vertex_array_x = 0;	// ID holder for trigger vertex array object
GLuint	trigger_vertex_array_y = 0;	// ID holder for trigger vertex array object
GLuint	trigger_vertex_array_z = 0;	// ID holder for trigger vertex array object

#define	TOTAL_STAGE	(5)
std::vector<block_t> blocks[TOTAL_STAGE];				// 5개 스테이지, 기본 블럭
std::vector<block_t> rotate_blocks[TOTAL_STAGE][3];		// 5개 스테이지, 최대 3개의 rotating group
std::vector<trigger_t> triggers[TOTAL_STAGE];			// 5개 스테이지, triggers group

//multiple character use available
std::vector<character_t>	characters[5];

std::vector<std::vector<block_t>*> obstacles[5];

float	current_frame_time;
float	last_frame_time;
float	t = 0.0f;		
float	default_camera_zoom = 1.0f;
float	camera_zoom0 = default_camera_zoom;
float	camera_zoom = default_camera_zoom;
float	stage_change_t = 0.0f;
int		stage = 0;
int		next_stage = 0;

bool up_pressed = 0;
bool down_pressed = 0;
bool left_pressed = 0;
bool right_pressed = 0;
bool lever_activate = 0;
bool stage_change = 0;
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
	vec3	eye = vec3(200,200,200);
	vec3	at = vec3( 0, 0, 0 );
	vec3	up = vec3( 0, 1, 0 );
	mat4	view_matrix = mat4::look_at( eye, at, up );

	float	current_theta	= PI / 4.0f;
	float	target_theta	= PI / 4.0f;
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
mat4 ortho( float fovy, float aspect, float dnear, float dfar, float how_far )
{
	float t = how_far / tan(fovy / 2),	b = -t;
	float l = -t * aspect,			r = -l;
	float n = dnear,				f = dfar;
	mat4 matrix;
	//matrix->_11 = 2 / (r - l); matrix->_12 = 0; matrix->_13 = 0; matrix->_14 = (l + r) / (l - r);
	//matrix->_21 = 0; matrix->_22 = 2 / (t - b); matrix->_23 = 0; matrix->_24 = (b + t) / (b - t);
	//matrix->_31 = 0; matrix->_32 = 0; matrix->_33 = 2 / (n - f); matrix->_34 = (n + f) / (n - f);
	//matrix->_41 = 0; matrix->_42 = 0; matrix->_43 = 0; matrix->_44 = 1;
	matrix._11 = 2 / (r - l);	matrix._12 = 0;				matrix._13 = 0;				matrix._14 = (l + r) / (l - r);
	matrix._21 = 0;				matrix._22 = 2 / (t - b);	matrix._23 = 0;				matrix._24 = (b + t) / (b - t);
	matrix._31 = 0;				matrix._32 = 0;				matrix._33 = 2 / (n - f);	matrix._34 = (n + f) / (n - f);
	matrix._41 = 0;				matrix._42 = 0;				matrix._43 = 0;				matrix._44 = 1;
	return matrix;
}

//*************************************
void update()
{
	// update global simulation parameter
	if (current_frame_time) {
		current_frame_time = float(glfwGetTime());
		t = current_frame_time - last_frame_time;
		if (t > 0.005f) t = 0.005f;
		last_frame_time = current_frame_time;
	}
	else {
		current_frame_time = float(glfwGetTime());
		last_frame_time = current_frame_time;
	}
	if (stage_change == 1) {
		if (stage_change_t > 0.0f) {
			stage_change_t = stage_change_t - t;
			camera_zoom += stage_change_t * stage_change_t/10;
		}
		else {
			stage = next_stage;
			stage_change_t = stage_change_t - t;
			camera_zoom -= stage_change_t * stage_change_t/10;
			if (camera_zoom <= default_camera_zoom) {
				stage_change = 0;
				camera_zoom = default_camera_zoom;
			}
		}
	}
	//camera rotate
	if (cam.current_theta < cam.target_theta) {
		cam.current_theta = min(cam.current_theta + t, cam.target_theta);
		cam.eye = vec3(200 * sqrt(2.0f) * cos(cam.current_theta), 200, 200 * sqrt(2.0f) * sin(cam.current_theta));
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
	}
	else {
		lever_activate = 0;
	}
	// update projection matrix
	cam.aspect = window_size.x/float(window_size.y);
	//cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect, cam.dnear, cam.dfar );
	cam.projection_matrix = ortho( cam.fovy, cam.aspect, cam.dnear, cam.dfar, camera_zoom);

	// build the model matrix for oscillating scale
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
	glBindVertexArray(block_vertex_array);
	for (auto& s : blocks[stage])
	{
		s.update(t);
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, s.model_matrix);
		glDrawElements(GL_TRIANGLES, 6 * 4 * 3, GL_UNSIGNED_INT, nullptr);
	}

	for (auto& s : rotate_blocks[stage][0])
	{
		s.update(t);
		if (s.rotate_flag == true) lever_activate = 1;
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, s.model_matrix);
		glDrawElements(GL_TRIANGLES, 6 * 4 * 3, GL_UNSIGNED_INT, nullptr);
	}

	for (auto& s : triggers[stage])
	{
		s.update(t);
		if (s.floor == 0) glBindVertexArray(trigger_vertex_array_z);
		if (s.floor == 1) glBindVertexArray(trigger_vertex_array_x);
		if (s.floor == 2) glBindVertexArray(trigger_vertex_array_y);
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, s.model_matrix);
		glDrawElements(GL_TRIANGLES, 8 * 3, GL_UNSIGNED_INT, nullptr);
	}

	for (auto& s : characters[stage])
	{
		if (s.floor == 0) glBindVertexArray(sphere_vertex_array_z);
		if (s.floor == 1) glBindVertexArray(sphere_vertex_array_x);
		if (s.floor == 2) glBindVertexArray(sphere_vertex_array_y);
		if (!lever_activate)
			s.update(t, obstacles[stage]);

		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, s.model_matrix);
		glDrawElements(GL_TRIANGLES, 301 * 300 * 6, GL_UNSIGNED_INT, nullptr);
		
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
		if (key == GLFW_KEY_UP)
			for (auto& s : characters[stage]) s.front_button(1);
		if (key == GLFW_KEY_DOWN)
			for (auto& s : characters[stage]) s.back_button(1);
		if (key == GLFW_KEY_LEFT)
			for (auto& s : characters[stage]) s.left_button(1);
		if (key == GLFW_KEY_RIGHT)
			for (auto& s : characters[stage]) s.right_button(1);
		if (key == GLFW_KEY_SPACE)
			for (auto& s : characters[stage]) s.spacebar_button();
		if (key == GLFW_KEY_R) {
			if (lever_activate == 0) {
				lever_activate = 1;
				for (auto& s : rotate_blocks[stage][0])
					s.block_rotation(PI / 2);
			}
		}

		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME)				cam = camera();
		else if (key == GLFW_KEY_W) {
			wire_mode = !wire_mode;
			glPolygonMode(GL_FRONT_AND_BACK, wire_mode ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", wire_mode ? "wireframe" : "solid");
		}
	}
	else if (action == GLFW_RELEASE) {

		if (key == GLFW_KEY_UP)
			for (auto& s : characters[stage]) s.front_button(0);
		if (key == GLFW_KEY_DOWN)
			for (auto& s : characters[stage]) s.back_button(0);
		if (key == GLFW_KEY_LEFT)
			for (auto& s : characters[stage]) s.left_button(0);
		if (key == GLFW_KEY_RIGHT)
			for (auto& s : characters[stage]) s.right_button(0);
		if (key == GLFW_KEY_T)
		{
			next_stage = (stage + 1) % 3;//TOTAL_STAGE;
			stage_change = 1;
			stage_change_t = 1.5f;
			printf("stage : %d\n", next_stage);
		}
		if (key == GLFW_KEY_M)
		{
			if (lever_activate == 0) {
				lever_activate = 1;
				cam.target_theta = cam.current_theta + PI / 2.0f;
			}
		}
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_MIDDLE)
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
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		//zooming
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS) {
			z_tracking = true;
			camera_zoom0 = camera_zoom;
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
		camera_zoom = camera_zoom0 + p;
	}
	else if(m_tracking) {
		// trackball
		vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);
		vec2 temp = (npos - m0) *-200;
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
	vertices.push_back({ vec3(0.5,0.5,0.5), vec3(145.0f/255, 162.0f/255, 1.0f), vec2(0,0) });
	vertices.push_back({ vec3(-0.5,0.5,0.5), vec3(145.0f/255, 162.0f/255, 1.0f), vec2(0,0) });

	vertices.push_back({ vec3(0,0,-0.5), vec3(1.0f, 1.0f, 1.0f), vec2(1,1) });
	vertices.push_back({ vec3(0,0,0.5), vec3(1.0f, 1.0f, 1.0f), vec2(1,1) });
	vertices.push_back({ vec3(0.5,0,0), vec3(1.0f, 1.0f, 1.0f), vec2(1,1) });
	vertices.push_back({ vec3(0,-0.5,0), vec3(1.0f, 1.0f, 1.0f), vec2(1,1) });
	vertices.push_back({ vec3(-0.5,0,0), vec3(1.0f, 1.0f, 1.0f), vec2(1,1) });
	vertices.push_back({ vec3(0,0.5,0), vec3(1.0f, 1.0f, 1.0f), vec2(1,1) });

	//create indices
	//bottom
	indices.push_back(0); indices.push_back(8); indices.push_back(1);
	indices.push_back(1); indices.push_back(8); indices.push_back(2);
	indices.push_back(2); indices.push_back(8); indices.push_back(3);
	indices.push_back(3); indices.push_back(8); indices.push_back(0);
	//top
	indices.push_back(5); indices.push_back(9); indices.push_back(4);
	indices.push_back(6); indices.push_back(9); indices.push_back(5);
	indices.push_back(7); indices.push_back(9); indices.push_back(6);
	indices.push_back(4); indices.push_back(9); indices.push_back(7);
	//front
	indices.push_back(5); indices.push_back(10); indices.push_back(6);
	indices.push_back(6); indices.push_back(10); indices.push_back(2);
	indices.push_back(2); indices.push_back(10); indices.push_back(1);
	indices.push_back(1); indices.push_back(10); indices.push_back(5);
	//left
	indices.push_back(4); indices.push_back(11); indices.push_back(5);
	indices.push_back(5); indices.push_back(11); indices.push_back(1);
	indices.push_back(1); indices.push_back(11); indices.push_back(0);
	indices.push_back(0); indices.push_back(11); indices.push_back(4);
	//back
	indices.push_back(7); indices.push_back(12); indices.push_back(4);
	indices.push_back(4); indices.push_back(12); indices.push_back(0);
	indices.push_back(0); indices.push_back(12); indices.push_back(3);
	indices.push_back(3); indices.push_back(12); indices.push_back(7);
	//right
	indices.push_back(6); indices.push_back(13); indices.push_back(7);
	indices.push_back(7); indices.push_back(13); indices.push_back(3);
	indices.push_back(3); indices.push_back(13); indices.push_back(2);
	indices.push_back(2); indices.push_back(13); indices.push_back(6);

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
void create_sphere_vertex_array()
{
	//if floor is x, y, z (commonly it's z)
	std::vector<uint> indices;
	std::vector<vertex> vertices_x, vertices_y, vertices_z;
	vec3 color;

	//create bottom
	for (int i = 0; i < 300; i++) {
		vertices_x.push_back({ vec3(-1,0,0), vec3(1,0,0), vec2(float(i) / 300, 0) });
		vertices_y.push_back({ vec3(0,-1,0), vec3(0,1,0), vec2(float(i) / 300, 0) });
		vertices_z.push_back({ vec3(0,0,-1), vec3(0,0,1), vec2(float(i) / 300, 0) });
	}
	
	for (int i = 0; i < 300; i++) {
		float t = PI * i / float(300), c = cos(t), s = sin(t);
		for (int j = 0; j < 300; j++) {
			float p = PI * 2.0f * j / float(300), pc = cos(p), ps = sin(p);

			if ((pc * s) > 0.98f) color = vec3(0);
			else if ((pc * s) > 0.95f) color = vec3(1.0f, 1.0f, 1.0f);
			else if ((pc * s) > 0.9f) color = vec3(0);
			else if ((pc * s) < -0.98f) color = vec3(0);
			else color = vec3(1.0f, (1.0f - ((pc * s) + 1.0f) / 2.0f), 1.0f);

			vertices_x.push_back({
				vec3(-c, pc * s, ps * s),
				color,
				vec2(float(j) / 300, float(i) / 300)
				});
			vertices_y.push_back({
				vec3(ps * s , -c, pc * s),
				color,
				vec2(float(j) / 300,float(i) / 300)
			});
			vertices_z.push_back({
				vec3(pc * s,ps * s,-c),
				color,
				vec2(float(j) / 300,float(i) / 300)
				});
		}
	}
	//create top
	for (int i = 0; i < 300; i++) {
		float t = PI * i / float(300), c = cos(t), s = sin(t);
		vertices_x.push_back({ vec3(1,0,0), vec3(cos(float(i) * PI / 600),sin(float(i) * PI / 600),1), vec2(float(i) / 300, 1) });
		vertices_y.push_back({ vec3(0,1,0), vec3(cos(float(i) * PI / 600),sin(float(i) * PI / 600),1), vec2(float(i) / 300, 1) });
		vertices_z.push_back({ vec3(0,0,1), vec3(cos(float(i) * PI / 600),sin(float(i) * PI / 600),1), vec2(float(i) / 300, 1) });
	}

	//create indices
	for (int i = 0; i < 300 + 1; i++) {
		for (int j = 0; j < 300; j++) {
			indices.push_back(j + i * 300);
			indices.push_back((j + 1) % 300 + (i + 1) * 300);
			indices.push_back(j + (i + 1) * 300);

			indices.push_back(j + i * 300);
			indices.push_back((j + 1) % 300 + i * 300);
			indices.push_back((j + 1) % 300 + (i + 1) * 300);
		}
	}

	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;
	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices_x.size(), &vertices_x[0], GL_STATIC_DRAW);
	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (sphere_vertex_array_x) glDeleteVertexArrays(1, &sphere_vertex_array_x);
	sphere_vertex_array_x = cg_create_vertex_array(vertex_buffer, index_buffer);

	vertex_buffer = 0;	// ID holder for vertex buffer
	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices_y.size(), &vertices_y[0], GL_STATIC_DRAW);
	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (sphere_vertex_array_y) glDeleteVertexArrays(1, &sphere_vertex_array_y);
	sphere_vertex_array_y = cg_create_vertex_array(vertex_buffer, index_buffer);

	vertex_buffer = 0;	// ID holder for vertex buffer
	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices_z.size(), &vertices_z[0], GL_STATIC_DRAW);
	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (sphere_vertex_array_z) glDeleteVertexArrays(1, &sphere_vertex_array_z);
	sphere_vertex_array_z = cg_create_vertex_array(vertex_buffer, index_buffer);
}
void create_trigger_vertex_array()
{
	std::vector<uint> indices;
	std::vector<vertex> vertices;

	//create vertices
	vertices.push_back({ vec3(-0.5f,-0.5f,0), vec3(1.0f, 0.0f, 0.0f), vec2(0,0) });
	vertices.push_back({ vec3(0.5f,-0.5f,0), vec3(1.0f, 0.0f, 0.0f), vec2(0,0) });
	vertices.push_back({ vec3(0.5f,0.5f,0), vec3(1.0f, 0.0f, 0.0f), vec2(0,0) });
	vertices.push_back({ vec3(-0.5f,0.5f,0), vec3(1.0f, 0.0f, 0.0f), vec2(0,0) });
	vertices.push_back({ vec3(-0.3f,-0.3f,0), vec3(1.0f, 0.0f, 0.0f), vec2(0,0) });
	vertices.push_back({ vec3(0.3f,-0.3f,0), vec3(1.0f, 0.0f, 0.0f), vec2(0,0) });
	vertices.push_back({ vec3(0.3f,0.3f,0), vec3(1.0f, 0.0f, 0.0f), vec2(0,0) });
	vertices.push_back({ vec3(-0.3f,0.3f,0), vec3(1.0f, 0.0f, 0.0f), vec2(0,0) });

	//create indices
	indices.push_back(0); indices.push_back(1); indices.push_back(4);
	indices.push_back(4); indices.push_back(1); indices.push_back(5);
	indices.push_back(1); indices.push_back(2); indices.push_back(5);
	indices.push_back(5); indices.push_back(2); indices.push_back(6);
	indices.push_back(2); indices.push_back(3); indices.push_back(6);
	indices.push_back(6); indices.push_back(3); indices.push_back(7);
	indices.push_back(3); indices.push_back(0); indices.push_back(7);
	indices.push_back(7); indices.push_back(0); indices.push_back(4);

	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;
	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (trigger_vertex_array_z) glDeleteVertexArrays(1, &trigger_vertex_array_z);
	trigger_vertex_array_z = cg_create_vertex_array(vertex_buffer, index_buffer);

	for (auto& s : vertices) s.pos = vec3(s.pos.z, s.pos.x, s.pos.y);
	vertex_buffer = 0;
	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (trigger_vertex_array_x) glDeleteVertexArrays(1, &trigger_vertex_array_x);
	trigger_vertex_array_x = cg_create_vertex_array(vertex_buffer, index_buffer);

	for (auto& s : vertices) s.pos = vec3(s.pos.z, s.pos.x, s.pos.y);
	vertex_buffer = 0;
	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (trigger_vertex_array_y) glDeleteVertexArrays(1, &trigger_vertex_array_y);
	trigger_vertex_array_y = cg_create_vertex_array(vertex_buffer, index_buffer);
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
	create_sphere_vertex_array();
	create_trigger_vertex_array();

	// Stage 0.
	stage = 0;
	blocks[stage]				= std::move(create_blocks0());
	rotate_blocks[stage][0]		= std::move(create_rotate_blocks0());
	triggers[stage]				= std::move(create_triggers0());
	characters[stage]			= std::move(create_characters0());
	obstacles[stage].push_back(&blocks[stage]);
	obstacles[stage].push_back(&rotate_blocks[stage][0]);

	// Stage 1.
	stage = 1;
	blocks[stage]				= std::move(create_blocks1());
	rotate_blocks[stage][0]		= std::move(create_rotate_blocks1());
	triggers[stage]				= std::move(create_triggers1());
	characters[stage]			= std::move(create_characters1());
	obstacles[stage].push_back(&blocks[stage]);
	obstacles[stage].push_back(&rotate_blocks[stage][0]);

	// Stage 2.
	stage = 2;
	blocks[stage] = std::move(create_blocks2());
	rotate_blocks[stage][0] = std::move(create_rotate_blocks2());
	triggers[stage] = std::move(create_triggers2());
	characters[stage] = std::move(create_characters2());
	obstacles[stage].push_back(&blocks[stage]);
	obstacles[stage].push_back(&rotate_blocks[stage][0]);

	stage = 0;

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
