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
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
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

#define BUF 512
#define CIRCLE_DIVISION_NUM 50
using namespace std;
//static const int INF = 1e9;
//static const double EPS = 1e-10;

// pair
template<typename T1, typename T2> ostream& operator<<(ostream& s, const pair<T1, T2>& p) {
    return s << "(" << p.first << ", " << p.second << ")";
}
// vector
template<typename T> ostream& operator<<(ostream& s, const vector<T>& v) {
    int len = v.size();
    for (int i = 0; i < len; ++i) {
        s << v[i]; if (i < len - 1) s << "\t";
    }
    return s;
}
// 2 dimentional vector
template<typename T> ostream& operator<<(ostream& s, const vector< vector<T> >& vv) {
    int len = vv.size();
    for (int i = 0; i < len; ++i) {
        s << vv[i] << endl;
    }
    return s;
}

//幾何問題用・複素数
typedef complex<double> xy_t;
double dot_product(xy_t a, xy_t b){ return (conj(a) * b).real(); }
double cross_product(xy_t a, xy_t b){ return (conj(a) * b).imag(); }
xy_t projection(xy_t p, xy_t b) {return b * dot_product(p,b) / norm(b);}

// グローバル変数
static double objSize = 0.1;
static vector<complex<double> > objVec;
static double aspect = 640 / 480;

// マウス入力情報
GLint mouse_button = -1;
GLint mouse_x = 0, mouse_y = 0;

GLuint  object_number=0;
double  object_depth=-1;

static GLuint  move_flag=0;
//ライティングの情報
static const GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
static const GLfloat light_ambient[] = {0.4, 0.4, 0.4, 1.0};
static const GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};

static const GLfloat mat_default_color[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat mat_default_specular[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat mat_default_shininess[] = {100.0};
static const GLfloat mat_default_emission[] = {0.0, 0.0, 0.0, 0.0};


void drawCircle(GLenum mode){
  for (auto itr = objVec.begin(); itr != objVec.end(); ++itr){
    auto objLoc = *itr;
    if(mode == GL_SELECT){ glPushName(itr - objVec.begin() + 1); }
    glBegin(GL_POLYGON);
    for (int i = 0; i < CIRCLE_DIVISION_NUM; ++i) {
      double theta = 2.0 * M_PI * i / CIRCLE_DIVISION_NUM;
      double x = (objSize * cos(theta) + objLoc.real());
      double y = objSize * sin(theta) + objLoc.imag();
      glVertex3f(x, y, 0.0);
    }
    glEnd();
    glPopName();
  }
}

int SelectHits(GLint hits, GLuint *buf){
  GLuint hit_name;
  float depth_min = 10.0f;
  float depth_1 = 1.0f;
  float depth_2 = 1.0f;
  GLuint depth_name;
  GLuint *ptr;
  if(hits <= 0) { return -1; }
  ptr = (GLuint*)buf;
  debug((int)hits);
  for(int i=0; i<hits; i++){
    // 識別番号の階層の深さ
    depth_name = *ptr;
    ptr++;
    depth_1 = (float) *ptr/0x7fffffff;
    ptr++;
    depth_2 = (float) *ptr/0x7fffffff;
    ptr++;
    // 最小デプスの確認
    if(depth_min>depth_1){
      depth_min = depth_1;
      hit_name = *ptr;
    }
    ptr++;
  }
  return hit_name;
}

int pick(int x, int y){
  GLuint selectBuf[BUF];
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
  hits = glRenderMode(GL_RENDER); // hit records
  debug((int)hits);
  return SelectHits(hits, selectBuf);
}

/**
 * Render new image.
 */
void display(void) {
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear frame buffer
  glLoadIdentity();
  gluLookAt(0, 0, 12, 0, 0, 0, 0.0, 1.0, 0.0);
  glColor3f(0, 0, 1.0);
  drawCircle(GL_RENDER);
  glFlush();
  glutSwapBuffers();
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
  if(state == GLUT_UP) { mouse_button = -1; }
  if(mouse_button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { pick(x, y); }
  glutPostRedisplay();
}

/**
 * Mouse movement callback function.
 * @param x mouse x coordination
 * @param y mouse y coordination
 */
void motion(int x, int y) {
  if(y == mouse_y) { return; }
  else if (mouse_button == GLUT_LEFT_BUTTON && x != mouse_x) {
    complex<double> mouseMove((x - mouse_x) / 50.0, (y - mouse_y) / -50.0);
    //objVec += mouseMove;
  }
  else if(mouse_button == GLUT_RIGHT_BUTTON) {
    objSize = max(objSize + (GLfloat) (y - mouse_y) / 100.0, 0.0);
  }
  mouse_x = x;
  mouse_y = y;
  glutPostRedisplay();
}

/**
 * Idle callback function.
 * @param x mouse x coordination
 * @param y mouse y coordination
 */
void idle() {
  glutPostRedisplay();
}

/**
 * Main function.
 */
int main(int argc, char** argv) {
  objVec.pb((0, 0));
  objVec.pb((0.2, 0.2));
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
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  glutMainLoop();
  return EXIT_SUCCESS;
}
