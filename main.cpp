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
#define FOR(i,k,n) for (int (i)=(k); (i)<(n); ++(i))
#define RFOR(i,k,n) for (int (i)=(n)-1; (i)>=(k); --(i))
#define REP(i,n) FOR(i,0,n)
#define RREP(i,n) RFOR(i,0,n)
#define pb push_back
#define mp make_pair
#define fst first
#define snd second
#define all(v) v.begin(), v.end()
#define debug(x) cerr<< #x <<": "<<x<<endl
#define debug2(x,y) cerr<< #x <<": "<< x <<", "<< #y <<": "<< y <<endl
#define debug3(x,y,z) cerr<< #x <<": "<< x <<", "\
                          << #y <<": "<< y <<", "\
                          << #z <<": "<< z << endl

#define BUF 512
#define OBJ_NUM 2
#define CIRCLE_DIVISION_NUM 50
using namespace std;


// グローバル変数
static double objSize = 1.0;
GLdouble object_position[OBJ_NUM][3] = {
  {0.0, 0.0, 0.0},
  {2.0, 2.0, 0.0},
};
static double aspect = 640 / 480;

// マウス入力情報
GLint mouse_button = -1;
GLint mouse_x = 0, mouse_y = 0;

GLuint object_number = 0;
double object_depth = -1;
static GLuint  move_flag = 0;

void drawCircle(GLenum mode){
  for (int i=0 ; i < OBJ_NUM; ++i) {
    if(mode == GL_SELECT){ glPushName(i+1); }
    glColor3f(0, 0, 1.0);
    glBegin(GL_POLYGON);
    for (int j = 0; j < CIRCLE_DIVISION_NUM; ++j) {
      double theta = 2.0 * M_PI * j / CIRCLE_DIVISION_NUM;
      double x = objSize * cos(theta) + object_position[i][0];
      double y = objSize * sin(theta) + object_position[i][1];
      glVertex3f(x, y, object_position[i][2]);
    }
    glEnd();
    glPopName();
  }
}

/**
 * Process mouse picking.
 * @param x mouse x coordination
 * @param y mouse y coordination
 */
void pick(int x, int y){
  GLuint selectBuf[BUF] = {0};
  GLint hits, viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glSelectBuffer(BUF, selectBuf);
  (void) glRenderMode(GL_SELECT);
  glInitNames();
  glPushName(0);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(x, viewport[3] - y, 5.0, 5.0, viewport);
    gluPerspective( 60.0, aspect, 1.0, 20.0 );
    glMatrixMode(GL_MODELVIEW);
    drawCircle(GL_SELECT); // render with selection mode
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

/**
 * Calculate world coordination from depth of given pixel
 * @param x mouse x coordination
 * @param y mouse y coordination
 * @param &wx world x coordination
 * @param &wy world y coordination
 * @param &wz world z coordination
 */
void  calcWorldCoordination(int x, int y, double depth, double &wx, double &wy,double &wz){
  GLdouble mvMatrix[16],pjMatrix[16];
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, pjMatrix);
  gluUnProject(x, viewport[3] - y, depth / 2,
               mvMatrix, pjMatrix, viewport,
               &wx, &wy, &wz);
}


/**
 * Render new image.
 */
void display(void) {
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear frame buffer
  glEnable(GL_DEPTH_TEST);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 5.0, 0, 0, 0, 0.0, 1.0, 0.0);
  drawCircle(GL_RENDER);
  glFlush();
  glutSwapBuffers();
}

/**
 * Mouse movement callback function.
 * @param x mouse x coordination
 * @param y mouse y coordination
 */
void dragObject(int x, int y){
  if(!move_flag) { return; }
  else if(mouse_button == GLUT_RIGHT_BUTTON) {
    objSize = max(objSize + (GLfloat) (y - mouse_y) / 100.0, 0.0);
  }
  double xx[3];
  if(object_depth > 0){
    calcWorldCoordination(x, y, object_depth, xx[0], xx[1], xx[2]);
    object_position[object_number-1][0] = xx[0];
    object_position[object_number-1][1] = xx[1];
    object_position[object_number-1][2] = 0;
  }
  display();
  mouse_x = x;
  mouse_y = y;
  glutPostRedisplay();
}

void FreeObject(int x, int y){
  object_number = 0;
  object_depth = -1;
  glutPostRedisplay();
}

/**
 * Window resize callback function.
 * @param w width of window
 * @param h height of window
 */
void reshape (int w, int h) {
  aspect = 1.0 * w / h;
  glViewport (0, 0, w, h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective(60.0, (GLfloat) aspect, 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
}

/**
 * Keyboard input callback function.
 * @param key key number
 * @param x mouse x coordination
 * @param y mouse y coordination
 */
void keyboard (unsigned char key, int x, int y) {
  if(key == 27) { exit(0);  } //ESCで終了
}

/**
 * Mouse click callback function.
 * @param button button number
 * @param state button state
 * @param x mouse x coordination
 * @param y mouse y coordination
 */
void mouse(int button, int state, int x, int y) {
  mouse_button = button;
  mouse_x = x;
  mouse_y = y;
  //if(mouse_button != GLUT_LEFT_BUTTON) { return; }
  if(state == GLUT_DOWN && move_flag == 0) {
    move_flag = !move_flag;
    pick(x, y);
  }
  else if(state == GLUT_UP) {
    FreeObject(x, y);
    move_flag = 0;
    mouse_button = -1;
  }
  glutPostRedisplay();
}

/**
 * Idle callback function.
 */
void idle() {
  glutPostRedisplay();
}

/**
 * Main function.
 */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
  glutInitWindowSize (800, 600);
  glutInitWindowPosition (50, 50);

  glutCreateWindow ("Main");
  glClearColor (0.0, 0.0, 0.0, 0.0); // set clear color
  glClearDepth(1.0);
  glPointSize(5.0f);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(dragObject);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);

  glutMainLoop();
  return EXIT_SUCCESS;
}
