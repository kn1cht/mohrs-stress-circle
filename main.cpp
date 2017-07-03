#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <unordered_map>
#include <queue>
#include <set>
#include <unordered_set>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#include <GL/glut.h>
#include <GLUI/glui.h>
#define pb push_back
#define mp make_pair
#define fst first
#define snd second
#define all(v) v.begin(), v.end()
#define debug(x) cerr<< #x <<": "<<x<<endl
#define debug2(x,y) cerr<< #x <<": "<< x <<", "<< #y <<": "<< y <<endl
#define debug3(x,y,z) cerr<< #x <<": "<< x <<", " << #y <<": "<< y <<", " << #z <<": "<< z << endl

using namespace std;

static const int Buf = 512;
static const int CircleDivisionNum = 50;
static const double ObjSize = 1.0;

double object_position[3] = {0.0, 0.0, 0.0};
double handle_position[3] = {0.0, 0.0, 0.0};
double handlePos[2][4] = {
  {0.1,-0.1,-0.1, 0.1},
  {0.1, 0.1,-0.1,-0.1}
};

int main_window;
unsigned int width = 800;
unsigned int height = 600;
int mouse_button = -1;

unsigned int object_number = 0;
double object_depth = -1;
bool move_flag = 0;

/**
 * Draw two circles.
 * @param mode OpenGL matrix mode.
 */
void drawCircle(GLenum mode);
/**
 * Process mouse picking.
 * @param x mouse x coordination
 * @param y mouse y coordination
 */
void pickMouse(int x, int y);
/**
 * Render new image.
 */
void display(void);
/**
 * Mouse movement callback function.
 * @param x mouse x coordination
 * @param y mouse y coordination
 */
void dragObject(int x, int y);
/**
 * Window resize callback function.
 * @param w width of window
 * @param h height of window
 */
void reshape (int w, int h);
/**
 * Keyboard input callback function.
 * @param key key number
 * @param x mouse x coordination
 * @param y mouse y coordination
 */
void keyboard (unsigned char key, int x, int y);
/**
 * Mouse click callback function.
 * @param button button number
 * @param state button state
 * @param x mouse x coordination
 * @param y mouse y coordination
 */
void mouse(int button, int state, int x, int y);
/**
 * Idle callback function.
 */
void idle();
/**
 * Exit button callback function.
 */
void gluiExit(int num);

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
  glutInitWindowSize (width, height);
  glutInitWindowPosition (50, 50);
  main_window = glutCreateWindow("Main");
  GLUI_Master.set_glutDisplayFunc(display);
  GLUI_Master.set_glutKeyboardFunc(keyboard);
  GLUI_Master.set_glutMouseFunc(mouse);
  glutMotionFunc(dragObject);
  GLUI_Master.set_glutReshapeFunc(reshape);
  GLUI_Master.set_glutIdleFunc(idle);
  GLUI *glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_BOTTOM);
  glui->set_main_gfx_window(main_window);
  glui->add_button("Exit", 0, gluiExit);
  glClearColor (0.3, 0.3, 0.3, 0.0); // set clear color
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearDepth(1.0);
  display();
  glutMainLoop();
  return EXIT_SUCCESS;
}

void drawCircle() {
  glColor3f(0, 0, 1.0);
  glBegin(GL_LINES);
  for (int j = 0; j <= CircleDivisionNum; ++j) {
    double theta = 2.0 * M_PI * j / CircleDivisionNum;
    double x = ObjSize * cos(theta) + object_position[0];
    double y = ObjSize * sin(theta) + object_position[1];
    glVertex3d(x, y, object_position[2]);
    theta = 2.0 * M_PI * (j+1) / CircleDivisionNum;
    x = ObjSize * cos(theta) + object_position[0];
    y = ObjSize * sin(theta) + object_position[1];
    glVertex3d(x, y, object_position[2]);
  }
  glEnd();
}

void drawHandle(GLenum mode) {
  if(mode == GL_SELECT) { glLoadName(1); }
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_QUADS);
  for(int i = 0; i < 4; ++i) {
    glVertex3d(handle_position[0] + handlePos[0][i], handle_position[1] + handlePos[1][i], 0.0);
  }
  glEnd();
  if(mode == GL_SELECT) { glLoadName(2); }
   glPointSize(3);
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_QUADS);
  for(int i = 0; i < 4; ++i) {
    glVertex3d(handle_position[0] + handlePos[0][i] + 1.0, handle_position[1] + handlePos[1][i] + 1.0, 0.0);
  }
  glEnd();
  glFlush();
}

void drawAxis(void) {
  GLdouble axis[][3] = {
    {-5.0, 0.0, 0.0 },
    { 5.0, 0.0, 0.0 },
    { 0.0,-5.0, 0.0 },
    { 0.0, 5.0, 0.0 }
  };
  glBegin(GL_LINES);
  for(int i = 0; i < 2; ++i) {
      glColor3dv(axis[2 * i + 1]);
      glVertex3dv(axis[2 * i]);
      glVertex3dv(axis[2 * i + 1]);
  }
  glEnd();
}

void pickMouse(int x, int y) {
  GLuint selectBuf[Buf] = {0};
  GLint hits, viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glSelectBuffer(Buf, selectBuf);
  glRenderMode(GL_SELECT);
  glInitNames();
  glPushName(0);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(x, height - y, 5.0, 5.0, viewport);
    gluPerspective(60.0, (double)viewport[2] / viewport[3] , 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    drawHandle(GL_SELECT); // render with selection mode
    glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  hits = glRenderMode(GL_RENDER); // hit number
  if(!hits) { return; }
  double near = 1.0f,
         far = 1.0f;
  GLuint* ptr;
  ptr = (GLuint*) selectBuf;
  object_depth = INT_MAX;
  for(int i=0; i < hits; ++i) {
    unsigned int objs = *(ptr++);
    near = (float) *(ptr++) / 0x7fffffff;
    far = (float) *(ptr++) / 0x7fffffff;
    object_depth = min(near, object_depth);
    for(int j=0; j < objs; ++j, ++ptr) {
      if(*ptr) { object_number = *ptr; }
    }
  }
}

void calcWorldCoordination(int x, int y, double depth, double &wx, double &wy,double &wz) {

}

void display(void) {
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear frame buffer
  glEnable(GL_DEPTH_TEST);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 5.0, 0, 0, 0, 0.0, 1.0, 0.0);
  drawCircle();
  drawHandle(GL_RENDER);
  drawAxis();
  glFlush();
  glutSwapBuffers();
}

void dragObject(int x, int y) {
  if(!move_flag || !object_depth) { return; }
  double xx[3], mvMatrix[16], pjMatrix[16];
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, pjMatrix);
  gluUnProject(x, height - y, object_depth/2, mvMatrix, pjMatrix, viewport, &xx[0], &xx[1], &xx[2]);
  handle_position[0] = xx[0];
  handle_position[1] = xx[1];
}

void reshape (int w, int h) {
  width = w;
  height = h;
  GLUI_Master.auto_set_viewport();
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (double)viewport[2] / viewport[3], 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
}

void keyboard (unsigned char key, int x, int y) {
  if(key == 27) { exit(0); } //ESC to Exit
}

void mouse(int button, int state, int x, int y) {
  mouse_button = button;
  if(state == GLUT_DOWN && move_flag == 0) {
    move_flag = !move_flag;
    pickMouse(x, y);
  }
  else if(state == GLUT_UP) {
    object_number = 0;
    object_depth = -1;
    move_flag = 0;
    mouse_button = -1;
  }
}

void idle() {
  glutSetWindow(main_window);
  glutPostRedisplay();
}

void gluiExit(int num) {
	exit(0);
}