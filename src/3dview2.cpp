/* 3D view manipulation demo
 * Written by John Tsiombikas <nuclear@member.fsf.org>
 *
 * Demonstrates how to use freeglut callbacks to manipulate a 3D view, similarly
 * to how a modelling program or a model viewer would operate.
 *
 * Rotate: drag with the left mouse button.
 * Scale: drag up/down with the right mouse button.
 * Pan: drag with the middle mouse button.
 *
 * Press space to animate the scene and update the display continuously, press
 *   again to return to updating only when the view needs to change.
 * Press escape or q to exit.
 */
#include <stdio.h>
#include <math.h>
#include <vector>
#include <unordered_map>
#include <GL/freeglut.h>
#include "quadricSimplification/quadric_simp.h"
#include "quadricSimplification/VcgMesh.h"
#include "Point3D.h"
#include "Point3D.inl.h"

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/edge_collapse.h>


#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4305 4244)
#endif

std::vector<int> _indices;
std::vector<Point3D> _vertices;
std::vector<Point3D> _normals;

std::vector<int> _indices2;
std::vector<Point3D> _vertices2;
std::vector<Point3D> _normals2;

class MyVertex;   // Forward declaration
class MyEdge;     // Forward declaration
class MyFace;     // Forward declaration

struct MyUsedTypes: public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType, vcg::Use<MyEdge>::AsEdgeType, vcg::Use<MyFace>::AsFaceType>{};
class MyVertex : public vcg::Vertex<MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::Qualityf, vcg::vertex::VFAdj, vcg::vertex::VEAdj>{};
class MyEdge : public vcg::Edge<MyUsedTypes>{};
class MyFace : public vcg::Face<MyUsedTypes, vcg::face::VertexRef, vcg::face::FFAdj, vcg::face::VFAdj, vcg::face::EdgeRef>{};
class MyMesh : public vcg::tri::TriMesh<std::vector<MyVertex>, std::vector<MyFace>>{};


typedef std::pair<unsigned int, unsigned int> Edge;

// For edgeMap
struct EdgeHash {
    template<class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

struct EdgeEqual {
    template<class T1, class T2>
    bool operator()(const std::pair<T1, T2> &lhs, const std::pair<T1, T2> &rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

static const char *helpprompt[] = {"Press F1 for help", 0};
static const char *helptext[] = {
	"Rotate: left mouse drag",
	" Scale: right mouse drag up/down",
	"   Pan: middle mouse drag",
	"",
	"Toggle fullscreen: f",
	"Toggle animation: space",
	"Quit: escape",
	0
};

void idle(void);
void display(void);
void print_help(void);
void reshape(int x, int y);
void keypress(unsigned char key, int x, int y);
void skeypress(int key, int x, int y);
void mouse(int bn, int st, int x, int y);
void motion(int x, int y);

int win_width, win_height;
float cam_theta, cam_phi = 25, cam_dist = 8;
float cam_pan[3];
int mouse_x, mouse_y;
int bnstate[8];
int anim, help;
long anim_start;
long nframes;

#ifndef GL_FRAMEBUFFER_SRGB
#define GL_FRAMEBUFFER_SRGB	0x8db9
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809d
#endif

using v3f = Point3D;

void computeNormals(std::vector<int> &indices, std::vector<v3f> &vertices, std::vector<v3f> &normals)
{
    normals.clear();
    for(int i = 0; i < indices.size()/3; ++i){

        v3f v1 = vertices[indices[i*3]];
        v3f v2 = vertices[indices[i*3+1]];
        v3f v3 = vertices[indices[i*3+2]];
        v3f n = (v2-v1)^(v3-v1);
        n.Normalize();
        normals.push_back(n);
    }
}

void fillTabs(){
    // TUBE
    _vertices.emplace_back( 0.0 , 1.0 , -73.61962890625 );
    _vertices.emplace_back( 0.0 , 1.0 , 73.61962890625 );
    _vertices.emplace_back( 0.19509032368659973 , 0.9807852506637573 , -73.61962890625 );
    _vertices.emplace_back( 0.19509032368659973 , 0.9807852506637573 , 73.61962890625 );
    _vertices.emplace_back( 0.3826834559440613 , 0.9238795042037964 , -73.61962890625 );
    _vertices.emplace_back( 0.3826834559440613 , 0.9238795042037964 , 73.61962890625 );
    _vertices.emplace_back( 0.5555702447891235 , 0.8314695954322815 , -73.61962890625 );
    _vertices.emplace_back( 0.5555702447891235 , 0.8314695954322815 , 73.61962890625 );
    _vertices.emplace_back( 0.7071067690849304 , 0.7071067690849304 , -73.61962890625 );
    _vertices.emplace_back( 0.7071067690849304 , 0.7071067690849304 , 73.61962890625 );
    _vertices.emplace_back( 0.8314695954322815 , 0.5555702447891235 , -73.61962890625 );
    _vertices.emplace_back( 0.8314695954322815 , 0.5555702447891235 , 73.61962890625 );
    _vertices.emplace_back( 0.9238795042037964 , 0.3826834559440613 , -73.61962890625 );
    _vertices.emplace_back( 0.9238795042037964 , 0.3826834559440613 , 73.61962890625 );
    _vertices.emplace_back( 0.9807852506637573 , 0.19509032368659973 , -73.61962890625 );
    _vertices.emplace_back( 0.9807852506637573 , 0.19509032368659973 , 73.61962890625 );
    _vertices.emplace_back( 1.0 , 0.0 , -73.61962890625 );
    _vertices.emplace_back( 1.0 , 0.0 , 73.61962890625 );
    _vertices.emplace_back( 0.9807852506637573 , -0.19509032368659973 , -73.61962890625 );
    _vertices.emplace_back( 0.9807852506637573 , -0.19509032368659973 , 73.61962890625 );
    _vertices.emplace_back( 0.9238795042037964 , -0.3826834559440613 , -73.61962890625 );
    _vertices.emplace_back( 0.9238795042037964 , -0.3826834559440613 , 73.61962890625 );
    _vertices.emplace_back( 0.8314695954322815 , -0.5555702447891235 , -73.61962890625 );
    _vertices.emplace_back( 0.8314695954322815 , -0.5555702447891235 , 73.61962890625 );
    _vertices.emplace_back( 0.7071067690849304 , -0.7071067690849304 , -73.61962890625 );
    _vertices.emplace_back( 0.7071067690849304 , -0.7071067690849304 , 73.61962890625 );
    _vertices.emplace_back( 0.5555702447891235 , -0.8314695954322815 , -73.61962890625 );
    _vertices.emplace_back( 0.5555702447891235 , -0.8314695954322815 , 73.61962890625 );
    _vertices.emplace_back( 0.3826834559440613 , -0.9238795042037964 , -73.61962890625 );
    _vertices.emplace_back( 0.3826834559440613 , -0.9238795042037964 , 73.61962890625 );
    _vertices.emplace_back( 0.19509032368659973 , -0.9807852506637573 , -73.61962890625 );
    _vertices.emplace_back( 0.19509032368659973 , -0.9807852506637573 , 73.61962890625 );
    _vertices.emplace_back( 0.0 , -1.0 , -73.61962890625 );
    _vertices.emplace_back( 0.0 , -1.0 , 73.61962890625 );
    _vertices.emplace_back( -0.19509032368659973 , -0.9807852506637573 , -73.61962890625 );
    _vertices.emplace_back( -0.19509032368659973 , -0.9807852506637573 , 73.61962890625 );
    _vertices.emplace_back( -0.3826834559440613 , -0.9238795042037964 , -73.61962890625 );
    _vertices.emplace_back( -0.3826834559440613 , -0.9238795042037964 , 73.61962890625 );
    _vertices.emplace_back( -0.5555702447891235 , -0.8314695954322815 , -73.61962890625 );
    _vertices.emplace_back( -0.5555702447891235 , -0.8314695954322815 , 73.61962890625 );
    _vertices.emplace_back( -0.7071067690849304 , -0.7071067690849304 , -73.61962890625 );
    _vertices.emplace_back( -0.7071067690849304 , -0.7071067690849304 , 73.61962890625 );
    _vertices.emplace_back( -0.8314695954322815 , -0.5555702447891235 , -73.61962890625 );
    _vertices.emplace_back( -0.8314695954322815 , -0.5555702447891235 , 73.61962890625 );
    _vertices.emplace_back( -0.9238795042037964 , -0.3826834559440613 , -73.61962890625 );
    _vertices.emplace_back( -0.9238795042037964 , -0.3826834559440613 , 73.61962890625 );
    _vertices.emplace_back( -0.9807852506637573 , -0.19509032368659973 , -73.61962890625 );
    _vertices.emplace_back( -0.9807852506637573 , -0.19509032368659973 , 73.61962890625 );
    _vertices.emplace_back( -1.0 , 0.0 , -73.61962890625 );
    _vertices.emplace_back( -1.0 , 0.0 , 73.61962890625 );
    _vertices.emplace_back( -0.9807852506637573 , 0.19509032368659973 , -73.61962890625 );
    _vertices.emplace_back( -0.9807852506637573 , 0.19509032368659973 , 73.61962890625 );
    _vertices.emplace_back( -0.9238795042037964 , 0.3826834559440613 , -73.61962890625 );
    _vertices.emplace_back( -0.9238795042037964 , 0.3826834559440613 , 73.61962890625 );
    _vertices.emplace_back( -0.8314695954322815 , 0.5555702447891235 , -73.61962890625 );
    _vertices.emplace_back( -0.8314695954322815 , 0.5555702447891235 , 73.61962890625 );
    _vertices.emplace_back( -0.7071067690849304 , 0.7071067690849304 , -73.61962890625 );
    _vertices.emplace_back( -0.7071067690849304 , 0.7071067690849304 , 73.61962890625 );
    _vertices.emplace_back( -0.5555702447891235 , 0.8314695954322815 , -73.61962890625 );
    _vertices.emplace_back( -0.5555702447891235 , 0.8314695954322815 , 73.61962890625 );
    _vertices.emplace_back( -0.3826834559440613 , 0.9238795042037964 , -73.61962890625 );
    _vertices.emplace_back( -0.3826834559440613 , 0.9238795042037964 , 73.61962890625 );
    _vertices.emplace_back( -0.19509032368659973 , 0.9807852506637573 , -73.61962890625 );
    _vertices.emplace_back( -0.19509032368659973 , 0.9807852506637573 , 73.61962890625 );
//------------------------ INDICE ------------------------
    _indices = {1,2,0,3,4,2,5,6,4,7,8,6,9,10,8,11,12,10,13,14,12,15,16,14,17,18,16,19,20,18,21,22,20,23,24,22,25,26,24,27,28,26,29,30,28,31,32,30,33,34,32,35,36,34,37,38,36,39,40,38,41,42,40,43,44,42,45,46,44,47,48,46,49,50,48,51,52,50,53,54,52,55,56,54,57,58,56,59,60,58,37,21,5,61,62,60,63,0,62,30,46,62,1,3,2,3,5,4,5,7,6,7,9,8,9,11,10,11,13,12,13,15,14,15,17,16,17,19,18,19,21,20,21,23,22,23,25,24,25,27,26,27,29,28,29,31,30,31,33,32,33,35,34,35,37,36,37,39,38,39,41,40,41,43,42,43,45,44,45,47,46,47,49,48,49,51,50,51,53,52,53,55,54,55,57,56,57,59,58,59,61,60,5,3,1,1,63,5,63,61,5,61,59,57,57,55,53,53,51,49,49,47,53,47,45,53,45,43,41,41,39,37,37,35,29,35,33,29,33,31,29,29,27,25,25,23,21,21,19,13,19,17,13,17,15,13,13,11,9,9,7,5,61,57,5,57,53,5,45,41,53,41,37,53,29,25,37,25,21,37,13,9,21,9,5,21,5,53,37,61,63,62,63,1,0,62,0,2,2,4,6,6,8,10,10,12,14,14,16,18,18,20,22,22,24,26,26,28,30,30,32,34,34,36,38,38,40,42,42,44,46,46,48,50,50,52,54,54,56,58,58,60,62,62,2,14,2,6,14,6,10,14,14,18,30,18,22,30,22,26,30,30,34,46,34,38,46,38,42,46,46,50,62,50,54,62,54,58,62,62,14,30};

// PIRA
//    _vertices.emplace_back( 0.0 , 1.0 , 0.0 );
//    _vertices.emplace_back( 1.0 , -1.0 , -1.0 );
//    _vertices.emplace_back( 1.0 , -1.0 , 1.0 );
//    _vertices.emplace_back( -1.0 , -1.0 , -1.0 );
//    _vertices.emplace_back( -1.0 , -1.0 , 1.0 );
////------------------------ INDICE ------------------------
//    _indices = {2,4,3,2,0,4,4,0,3,3,1,2,1,0,2,3,0,1};
}






void displayMesh(std::vector<int> &indices, std::vector<v3f> &vertices, std::vector<v3f> &normals)
{
    for(int i = 0; i < indices.size()/3; ++i) {
        //v3f n = normals[indices[i*3]];
        v3f v1 = vertices[indices[i*3]];
        v3f v2 = vertices[indices[i*3+1]];
        v3f v3 = vertices[indices[i*3+2]];

        glBegin(GL_TRIANGLES);
        //glNormal3f(n[0], n[1], n[2]);
        glVertex3f(v1[0], v1[1], v1[2]);
        glVertex3f(v2[0], v2[1], v2[2]);
        glVertex3f(v3[0], v3[1], v3[2]);
        glEnd();
    }
}

void drawCoordinateAxis() {
    glLineWidth(2.0f);  // Set the line width for clarity

    // X-axis (red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(-cam_pan[0], -cam_pan[1], -cam_pan[2]);
    glVertex3f(-cam_pan[0]+1, -cam_pan[1], -cam_pan[2]);
    glEnd();

    // Y-axis (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(-cam_pan[0], -cam_pan[1], -cam_pan[2]);
    glVertex3f(-cam_pan[0], -cam_pan[1]+1, -cam_pan[2]);
    glEnd();

    // Z-axis (blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(-cam_pan[0], -cam_pan[1], -cam_pan[2]);
    glVertex3f(-cam_pan[0], -cam_pan[1], -cam_pan[2]+1);
    glEnd();

    glLineWidth(1.0f);  // Reset the line width
    glColor3f(1.0f, 1.0f, 1.0f);
}

int main(int argc, char **argv)
{

    fillTabs();
    //computeNormals(_indices, _vertices, _normals);

    //CMeshO m1 = constructCMesh(_indices, _vertices, _normals);

    AMesh m1;
    m1.load(_indices, _vertices);
    m1.quadricInit();
    m1.quadricSimplify(4);
    m1.retrieveData(_indices2, _vertices2);

    //vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromVF(m1);
    //vcg::tri::TriEdgeCollapseQuadricParameter params;

//    params.BoundaryQuadricWeight = 0.5;
//    params.FastPreserveBoundary = true;
//    params.AreaCheck = true;
//    params.HardQualityCheck = true;
//    params.HardQualityThr = 0.1;
//    params.HardNormalCheck = true;
//    params.NormalCheck = true;
//    params.NormalThrRad = M_PI/2.0;
//    params.CosineThr             = 0 ; // ~ cos(pi/2)
//    params.OptimalPlacement =true;
//    params.SVDPlacement = false;
//    params.PreserveTopology =true;
//    params.PreserveBoundary = true;
//    params.QuadricEpsilon = 1e-15;
//    params.QualityCheck =true;
//    params.QualityThr =.3;     // Collapsed that generate faces with quality LOWER than this value are penalized. So higher the value -> better the quality of the accepted triangles
//    params.QualityQuadric = false; // planar simplification
//    params.QualityQuadricWeight = 0.001f; // During the initialization manage all the edges as border edges adding a set of additional quadrics that are useful mostly for keeping face aspect ratio good.
//    params.QualityWeight=false;
//    params.QualityWeightFactor=0.0;
//    params.ScaleFactor=1.0;
//    params.ScaleIndependent=true;
//    params.UseArea =true;
//    params.UseVertexWeight=false;

    //QuadricSimplification(m1, 4, false, params, vcg::DummyCallBackPos);



    //m.updateBoxAndNormals();
//    vcg::tri::UpdateNormal<CMeshO>::NormalizePerFace(m1);
//    vcg::tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(m1);
//    vcg::tri::UpdateNormal<CMeshO>::NormalizePerVertex(m1);

    //retrieveCMeshData(m1, _indices2, _vertices2, _normals2);
    //computeNormals(_indices2, _vertices2, _normals2);








	glutInit(&argc, argv);
	glutInitWindowSize(1600, 900);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("freeglut 3D view demo");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keypress);
	glutSpecialFunc(skeypress);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //glCullFace(GL_BACK);

	glutMainLoop();
	return 0;
}

void idle(void)
{
	glutPostRedisplay();
}

void display(void)
{
	long tm;
	float lpos[] = {-1, 2, 10, 0};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -cam_dist);
	glRotatef(cam_phi, 1, 0, 0);
	glRotatef(cam_theta, 0, 1, 0);
	glTranslatef(cam_pan[0], cam_pan[1], cam_pan[2]);

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);


    glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-5, -1.3, 5);
	glVertex3f(5, -1.3, 5);
	glVertex3f(5, -1.3, -5);
	glVertex3f(-5, -1.3, -5);
	glEnd();


    //displayMesh(_indices, _vertices, _normals);
    glColor3f(1.0f, 0.8f, 0.8f);
    displayMesh(_indices2, _vertices2, _normals2);

    drawCoordinateAxis();

	print_help();

	glutSwapBuffers();
	nframes++;
}

void print_help(void)
{
	int i;
	const char *s, **text;

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, win_width, 0, win_height, -1, 1);

	text = help ? helptext : helpprompt;

	for(i=0; text[i]; i++) {
		glColor3f(0, 0.1, 0);
		glRasterPos2f(7, win_height - (i + 1) * 20 - 2);
		s = text[i];
		while(*s) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *s++);
		}
		glColor3f(0, 0.9, 0);
		glRasterPos2f(5, win_height - (i + 1) * 20);
		s = text[i];
		while(*s) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *s++);
		}
	}

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();
}

#define ZNEAR	0.5f
void reshape(int x, int y)
{
	float vsz, aspect = (float)x / (float)y;
	win_width = x;
	win_height = y;

	glViewport(0, 0, x, y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	vsz = 0.4663f * ZNEAR;
	glFrustum(-aspect * vsz, aspect * vsz, -vsz, vsz, 0.5, 500.0);
}

void keypress(unsigned char key, int x, int y)
{
	static int fullscr;
	static int prev_xsz, prev_ysz;

	switch(key) {
	case 27:
	case 'q':
		exit(0);
		break;

	case ' ':
		anim ^= 1;
		glutIdleFunc(anim ? idle : 0);
		glutPostRedisplay();

		if(anim) {
			anim_start = glutGet(GLUT_ELAPSED_TIME);
			nframes = 0;
		} else {
			long tm = glutGet(GLUT_ELAPSED_TIME) - anim_start;
			long fps = (nframes * 100000) / tm;
			printf("framerate: %ld.%ld fps\n", fps / 100, fps % 100);
		}
		break;

	case '\n':
	case '\r':
		if(!(glutGetModifiers() & GLUT_ACTIVE_ALT)) {
			break;
		}
	case 'f':
		fullscr ^= 1;
		if(fullscr) {
			prev_xsz = glutGet(GLUT_WINDOW_WIDTH);
			prev_ysz = glutGet(GLUT_WINDOW_HEIGHT);
			glutFullScreen();
		} else {
			glutReshapeWindow(prev_xsz, prev_ysz);
		}
		break;
	}
}

void skeypress(int key, int x, int y)
{
	switch(key) {
	case GLUT_KEY_F1:
		help ^= 1;
		glutPostRedisplay();

	default:
		break;
	}
}

void mouse(int bn, int st, int x, int y)
{
	int bidx = bn - GLUT_LEFT_BUTTON;
	bnstate[bidx] = st == GLUT_DOWN;
	mouse_x = x;
	mouse_y = y;
}

void motion(int x, int y)
{
	int dx = x - mouse_x;
	int dy = y - mouse_y;
	mouse_x = x;
	mouse_y = y;

	if(!(dx | dy)) return;

	if(bnstate[0]) {
		cam_theta += dx * 0.5;
		cam_phi += dy * 0.5;
		if(cam_phi < -90) cam_phi = -90;
		if(cam_phi > 90) cam_phi = 90;
		glutPostRedisplay();
	}
	if(bnstate[1]) {
		float up[3], right[3];
		float theta = cam_theta * M_PI / 180.0f;
		float phi = cam_phi * M_PI / 180.0f;

		up[0] = -sin(theta) * sin(phi);
		up[1] = -cos(phi);
		up[2] = cos(theta) * sin(phi);
		right[0] = cos(theta);
		right[1] = 0;
		right[2] = sin(theta);

		cam_pan[0] += (right[0] * dx + up[0] * dy) * 0.01;
		cam_pan[1] += up[1] * dy * 0.01;
		cam_pan[2] += (right[2] * dx + up[2] * dy) * 0.01;
		glutPostRedisplay();
	}
	if(bnstate[2]) {
		cam_dist += dy * 0.1;
		if(cam_dist < 0) cam_dist = 0;
		glutPostRedisplay();
	}
}
