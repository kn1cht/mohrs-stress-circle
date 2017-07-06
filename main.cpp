#include <climits>
#include <cmath>
#include <iostream>
#include <iterator>
#include <GL/glut.h>
#include <GLUI/glui.h>
#include "stress_circle.hpp"
#define debug(x) std::cerr<< #x <<": "<<x<<std::endl
#define debug2(x,y) std::cerr<< #x <<": "<< x <<", "<< #y <<": "<< y <<std::endl

const unsigned int WINDOW_WIDTH  = 800;
const unsigned int WINDOW_HEIGHT = 600;

int main_window;
bool is_drag = 0;
GLUI* glui;
GLUI_Spinner *sigma_x_box, *sigma_y_box, *tau_box;
GLUI_EditText *angle_box, *sigma_1_box, *sigma_2_box, *tau_max_box;
struct StressCircle stressCircle(WINDOW_WIDTH, WINDOW_HEIGHT);

void drawAxis();
void display();
void motion(int x, int y);
void reshape (int w, int h);
void mouse(int button, int state, int x, int y);
void idle();
void gluiInit();
void gruiReset(int num);

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(50, 50);
  main_window = glutCreateWindow("Mohr's stress circle");
  gluiInit(); // GLUI initialization
  glClearColor (0.3, 0.3, 0.3, 0.0);
  glClearDepth(1.0);
  display(); // initial display
  glutMainLoop();
  return EXIT_SUCCESS;
}

void drawAxis() {
  double axis[2][2][3] = { // slightly backward
    {{-5.0, 0.0,-0.02}, { 5.0, 0.0,-0.02}},
    {{ 0.0,-5.0,-0.02}, { 0.0, 5.0,-0.02}}
  };
  double color[2][3] = {
    {5.0, 0.0, 0.0},
    {0.0, 5.0, 0.0}
  };
  for(auto itr = std::begin(axis); itr != std::end(axis); ++itr) {
    glBegin(GL_LINES);
    glColor3dv(color[itr - std::begin(axis)]);
    glVertex3dv(axis[itr - std::begin(axis)][0]);
    glVertex3dv(axis[itr - std::begin(axis)][1]);
    glEnd();
  }
}

void display() {
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear frame buffer
  glEnable(GL_DEPTH_TEST);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 5.0, 0, 0, 0, 0.0, 1.0, 0.0);
  drawAxis();
  stressCircle.updateVal();
  stressCircle.drawCircle();
  stressCircle.drawHandles(GL_RENDER);
  stressCircle.drawString();
  glFlush();
  glutSwapBuffers();
}

void reshape (int w, int h) {
  stressCircle.windowWidth  = w;
  stressCircle.windowHeight = h;
  GLUI_Master.auto_set_viewport();
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (double)viewport[2] / viewport[3], 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
  switch(button) {
    case GLUT_LEFT_BUTTON :
      if(state == GLUT_DOWN) { stressCircle.pickHandle(x, y); }
      is_drag = !is_drag; 
      break;
    default: break;
  }
}

void motion(int x, int y) {
  if(!is_drag) { return; }
  stressCircle.dragHandle(x, y);
  glui->sync_live(); 
}

void idle() {
  glutSetWindow(main_window);
  glui->sync_live(); 
  glutPostRedisplay();
}

void gluiInit() {
  GLUI_Master.set_glutDisplayFunc(display);
  GLUI_Master.set_glutMouseFunc(mouse);
  glutMotionFunc(motion); // GLUI don't support MotionFunc
  GLUI_Master.set_glutReshapeFunc(reshape);
  GLUI_Master.set_glutIdleFunc(idle);
  glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_BOTTOM);
  glui->set_main_gfx_window(main_window);
  sigma_x_box = glui->add_spinner("sigma_x", GLUI_SPINNER_FLOAT, &stressCircle.sigma_x);
  sigma_y_box = glui->add_spinner("sigma_y", GLUI_SPINNER_FLOAT, &stressCircle.sigma_y);
  tau_box = glui->add_spinner("tau", GLUI_SPINNER_FLOAT, &stressCircle.tau);
  glui->add_column();
  angle_box = glui->add_edittext("angle", GLUI_SPINNER_FLOAT, &stressCircle.angle);
  sigma_1_box = glui->add_edittext("sigma_1", GLUI_EDITTEXT_FLOAT, &stressCircle.sigma_1);
  glui->add_column();
  sigma_2_box = glui->add_edittext("sigma_2", GLUI_EDITTEXT_FLOAT, &stressCircle.sigma_2);
  tau_max_box = glui->add_edittext("tau_max", GLUI_EDITTEXT_FLOAT, &stressCircle.tau_max);
  glui->add_column();
  glui->add_button("Reset", 0, gruiReset);
  glui->add_separator();
  glui->add_button("Exit", 0, exit);
}

void gruiReset(int num) {
  stressCircle.resetVal();
  glui->sync_live(); 
}