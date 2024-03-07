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
#include <GL/freeglut.h>
#include <cstdint>
#include "Point3D.h"
#include "Point3D.inl.h"
#include "VCGLib_Helper/LODMaker.h"
#include "ObjIO.h"
#include <chrono>

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4305 4244)
#endif

std::vector<uint32_t> _indices;
std::vector<Point3D> _vertices;
std::vector<Point3D> _normals;

std::vector<uint32_t> _indices2;
std::vector<Point3D> _vertices2;
std::vector<Point3D> _normals2;

bool displayNormals_ = false;
int displayMode = 0;

float lpos[] = {10, 10, 10, 0};
float lAmbient[] = {0.2, 0.2, 0.2, 0};
float lDiffuse[] = {1, 1, 1, 0};

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

void setMatColor(float r,float g,float b,float a){
    GLfloat m[] = {r,g,b,a};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m);
    glColor3f(r,g,b);
}

void computeNormals(std::vector<uint32_t> &indices, std::vector<v3f> &vertices, std::vector<v3f> &normals)
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

void translateVertices(std::vector<Point3D> & vertices, Point3D vec)
{
    for (auto &v : vertices){
        v += vec;
    }
}

void fillTabs(){
    ObjIO::readObj("../../objTUY/TUY_1071.obj",_indices,_vertices);
}

void displayNormal(Point3D & pos, Point3D &normal, float scale)
{
    glLineWidth(1.0f);  // Set the line width for clarity
    glDisable(GL_LIGHTING);

    glColor3f(1,1,0);
    glBegin(GL_LINES);
    glVertex3f(pos[0], pos[1], pos[2]);
    glVertex3f(pos[0]+ normal[0]*scale, pos[1]+normal[1]*scale, pos[2]+normal[2]*scale);
    glEnd();

    glEnable(GL_LIGHTING);
    glLineWidth(1.0f);  // Reset the line width
}

void displayMesh(std::vector<uint32_t> &indices, std::vector<v3f> &vertices, std::vector<v3f> &normals)
{
    if(normals.size() == 0)
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
    } else
    {
        for(int i = 0; i < indices.size()/3; ++i) {
            v3f n = normals[i];
            v3f v1 = vertices[indices[i*3]];
            v3f v2 = vertices[indices[i*3+1]];
            v3f v3 = vertices[indices[i*3+2]];

            glBegin(GL_TRIANGLES);
            glNormal3f(n[0], n[1], n[2]);
            glVertex3f(v1[0], v1[1], v1[2]);
            glVertex3f(v2[0], v2[1], v2[2]);
            glVertex3f(v3[0], v3[1], v3[2]);
            glEnd();

            if(displayNormals_){
                v3f pos = (v1+v2+v3)/3;
                displayNormal(pos,n, 0.05);
            }
        }
    }
}

void drawCoordinateAxis() {
    glLineWidth(2.0f);  // Set the line width for clarity

    if(displayMode == 0){
        glDisable(GL_LIGHTING);
    }

    // X-axis (red)
    //setMatColor(1,0,0,0);
    glColor3f(1,0,0);
    glBegin(GL_LINES);
    glVertex3f(-cam_pan[0], -cam_pan[1], -cam_pan[2]);
    glVertex3f(-cam_pan[0]+1, -cam_pan[1], -cam_pan[2]);
    glEnd();

    // Y-axis (green)
    glColor3f(0,1,0);
    glBegin(GL_LINES);
    glVertex3f(-cam_pan[0], -cam_pan[1], -cam_pan[2]);
    glVertex3f(-cam_pan[0], -cam_pan[1]+1, -cam_pan[2]);
    glEnd();

    // Z-axis (blue)
    glColor3f(0,0,1);
    glBegin(GL_LINES);
    glVertex3f(-cam_pan[0], -cam_pan[1], -cam_pan[2]);
    glVertex3f(-cam_pan[0], -cam_pan[1], -cam_pan[2]+1);
    glEnd();

    if(displayMode == 0){
        glEnable(GL_LIGHTING);
    }
    glLineWidth(1.0f);  // Reset the line width
}

int main(int argc, char **argv)
{
    fillTabs();

    computeNormals(_indices, _vertices, _normals);

    auto start = std::chrono::high_resolution_clock::now();

    CMeshO m1 = VCG_CMesh0_Helper::constructCMesh(_indices, _vertices, _normals);

    VCG_CMesh0_Helper::repairAndPrepareForDecimation(m1);

    LODMaker::decimateMesh(100, m1);

    VCG_CMesh0_Helper::retrieveCMeshData(m1, _indices2, _vertices2, _normals2);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "init vertices : " << _indices.size() << "\n";
    std::cout << "decimate mesh vertices : " << _indices2.size() << "\n";
    std::cout << "elapsed time : " << duration.count() << "\n";

    translateVertices(_vertices, Point3D(-0.15,0,0));
    translateVertices(_vertices2, Point3D(0.15,0,0));

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
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lDiffuse);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
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


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -cam_dist);
	glRotatef(cam_phi, 1, 0, 0);
	glRotatef(cam_theta, 0, 1, 0);
	glTranslatef(cam_pan[0], cam_pan[1], cam_pan[2]);


    setMatColor(1,1,1,0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-5, -1.3, 5);
	glVertex3f(5, -1.3, 5);
	glVertex3f(5, -1.3, -5);
	glVertex3f(-5, -1.3, -5);
	glEnd();

    setMatColor(1,1,1,0);
    displayMesh(_indices, _vertices, _normals);
    setMatColor(1,0.8,0.2,0);
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

#define ZNEAR	0.05f
void reshape(int x, int y)
{
	float vsz, aspect = (float)x / (float)y;
	win_width = x;
	win_height = y;

	glViewport(0, 0, x, y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	vsz = 0.4663f * ZNEAR;
	glFrustum(-aspect * vsz, aspect * vsz, -vsz, vsz, 0.05, 5000.0);
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
    case 'm':
        displayMode = (++displayMode)%2;
        if(displayMode == 0){
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glPolygonMode(GL_FRONT, GL_FILL);
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);

            glLightfv(GL_LIGHT0, GL_POSITION, lpos);
            glLightfv(GL_LIGHT0, GL_AMBIENT, lAmbient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, lDiffuse);
        } else if(displayMode == 1){
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        }
        display();
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
