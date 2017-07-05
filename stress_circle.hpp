#ifndef   STRESS_CIRCLE_HPP
#define   STRESS_CIRCLE_HPP

#include <climits>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <GL/glut.h>

struct MyGLObj {
  unsigned const int name; // unique name for selection
  const float color[3];
  const float* sVal; // sigma axis value
  const float* tVal; // tau axis value
	inline bool operator==(const MyGLObj& foe) { 
  return name == foe.name && color[0] == foe.color[0] && color[1] == foe.color[1] && color[2] == foe.color[2]; 
  }
	inline bool operator!=(const MyGLObj& foe) { return !(*this == foe); }
};

struct StressCircle {
  StressCircle(unsigned int w, unsigned int h);
  float sigma_x, sigma_y, tau, sigma_1, sigma_2, tau_max, angle;
  const unsigned int CIRCLE_DIVISION_NUM = 100;
  const unsigned int HANDLE_DIVISION_NUM = 4;
  const float HANDLE_RADIUS = 0.1;
  const float MAIN_STRESS_RADIUS = 0.05;
  const float SIGMA_X_INITIAL = 2.0;
  const float SIGMA_Y_INITIAL = 0.0;
  const float TAU_INITIAL = 0.0;
  const MyGLObj xHandle = {1, {1.0, 0.0, 0.0}}; // Red
  const MyGLObj yHandle = {2, {0.0, 0.0, 1.0}}; // Blue
  const MyGLObj xAngle = {3, {0.0, 1.0, 1.0}}; // Cyan
  const MyGLObj yAngle = {4, {0.0, 1.0, 1.0}}; // Cyan
  const MyGLObj mainStress1 = {0, {1.0, 0.0, 0.0}}; // Red (dragging disabled)
  const MyGLObj mainStress2 = {0, {0.0, 0.0, 1.0}}; // Blue (dragging disabled)
  unsigned const int BUF = 512;
  unsigned int windowWidth = 800;
  unsigned int windowHeight = 600;
  unsigned int object_name = 0;
  double object_depth = -1;
  /**
  * Return x coordination of the center of stress circle.
  */
  float center();
  /**
  * Return radius of stress circle.
  */
  float radius();
  /**
  * Update stress values.
  */
  void updateStress(float sigma_x, float sigma_y, float tau);
  /**
  * Update dependent variables.
  */
  void updateVal();
  /**
  * Update all values.
  */
  void resetVal();
  /**
  * Render stress circle.
  */
  void drawCircle();
  /**
  * Render handles.
  * @param mode current GL matrix mode
  */
  void drawHandles(GLenum mode);
  /**
  * Render stress value string.
  */
  void drawString();
  /**
  * Process mouse picking.
  * @param x mouse x coordination
  * @param y mouse y coordination
  */
  void pickHandle(int x, int y);
  /**
  * Update dragged handle's position.
  * @param x mouse x coordination
  * @param y mouse y coordination
  */
  void dragHandle(int x, int y);
};

StressCircle::StressCircle(unsigned int w, unsigned int h) : windowWidth(w), windowHeight(h) {
  sigma_x = (SIGMA_X_INITIAL);
  sigma_y = (SIGMA_Y_INITIAL);
  tau = (TAU_INITIAL);
  updateVal();
};

float StressCircle::center() { return (sigma_x + sigma_y) / 2; }

float StressCircle::radius() { return std::sqrt(std::pow(sigma_x - sigma_y, 2) / 4 + std::pow(tau, 2)); }

void StressCircle::updateStress(float sigma_x_new, float sigma_y_new, float tau_new) {
  sigma_x = sigma_x_new;
  sigma_y = sigma_y_new;
  tau = tau_new;
  updateVal();
}

void StressCircle::updateVal() {
  angle = std::atan2(2 * tau, sigma_x - sigma_y) / 2.0 / M_PI * 180;
  sigma_1 = center() + radius();
  sigma_2 = center() - radius();
  tau_max = radius();
}

void StressCircle::resetVal() {
    updateStress(SIGMA_X_INITIAL, SIGMA_Y_INITIAL, TAU_INITIAL);
}

void StressCircle::drawCircle() {
  glColor3f(1.0, 1.0, 1.0); // White
  glLineWidth(2);
  glBegin(GL_LINES);
  for (int i = 0; i < CIRCLE_DIVISION_NUM; ++i) {
    double theta = 2.0 * M_PI * i / CIRCLE_DIVISION_NUM;
    glVertex3d(radius() * std::cos(theta) + center(), radius() * std::sin(theta), -0.01); // slightly backward
    theta = 2.0 * M_PI * (i + 1) / CIRCLE_DIVISION_NUM;
    glVertex3d(radius() * std::cos(theta) + center(), radius() * std::sin(theta), -0.01); // slightly backward
  }
  glEnd();
  std::vector<MyGLObj> mainStressArray{mainStress1, mainStress2};
  for(auto mainStress : mainStressArray) {
    glColor3fv(mainStress.color);
    glBegin(GL_POLYGON);
    for (int i = 0; i < CIRCLE_DIVISION_NUM; ++i) {
      double theta = 2.0 * M_PI * i / CIRCLE_DIVISION_NUM;
      glVertex2d(MAIN_STRESS_RADIUS * std::cos(theta) + ((mainStress == mainStress1) ? sigma_1 : sigma_2),
                 MAIN_STRESS_RADIUS * std::sin(theta));
    }
    glEnd();
  }
}

void StressCircle::drawHandles(GLenum mode) {
  std::vector<MyGLObj> handleArray{xHandle, yHandle};
  for(auto handle : handleArray) {
    if(mode == GL_SELECT) { glLoadName(handle.name); }
    glColor3fv(handle.color);
    glBegin(GL_POLYGON); 
    for(int j = 0; j < HANDLE_DIVISION_NUM; ++j) {
      double theta = 2.0 * M_PI * j / HANDLE_DIVISION_NUM + M_PI / 4;
      glVertex2f(((handle == xHandle) ? sigma_x : sigma_y) + HANDLE_RADIUS * std::cos(theta), 
                 ((handle == xHandle) ? tau : -tau) + HANDLE_RADIUS * std::sin(theta));
    }
    glEnd();
  }
  std::vector<MyGLObj> angleArray{xAngle, yAngle};
  glLineWidth(5);
  for(auto angle : angleArray) {
    if(mode == GL_SELECT) { glLoadName(angle.name); }
    glColor3fv(angle.color);
    glBegin(GL_LINES); 
    (angle == xAngle) ? glVertex2f(sigma_x, tau) : glVertex2f(sigma_y,-tau);
    glVertex2f(center(), 0.0);
    glEnd();
  }
  glLineWidth(1);
  glFlush();
}

void StressCircle::drawString() {
  glColor3f(0.0, 1.0, 0.0);
  std::vector<std::pair<std::string, float> > stressValueStrings = {
  std::make_pair("sigma_x", sigma_x),
  std::make_pair("sigma_y", sigma_y),
  std::make_pair("tau", tau),
  std::make_pair("sigma_1", sigma_1),
  std::make_pair("sigma_2", sigma_2),
  std::make_pair("tau_max", tau_max),
  std::make_pair("angle", angle)
  };
  std::stringstream ss;
  ss <<  std::fixed << std::setprecision(4);
  int i = 0;
  for(const auto item : stressValueStrings) {
    glWindowPos2i(10, windowHeight - (++i) * 24); // add new row
    ss << item.first << ": " << std::to_string(item.second); // "{label}: {value}" 
    _glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const char*> (ss.str().c_str()));
    ss.str("");  // clear string stream
  }
}

void StressCircle::pickHandle(int x, int y) {
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  unsigned int selectBuf[BUF];
  glSelectBuffer(BUF, selectBuf);
  glRenderMode(GL_SELECT);
  glInitNames();
  glPushName(0);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluPickMatrix(x, windowHeight - y, 5.0, 5.0, viewport);
  gluPerspective(60.0, (double)viewport[2] / viewport[3] , 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
  drawHandles(GL_SELECT); // render with selection mode
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  int hits = glRenderMode(GL_RENDER); // hit number
  object_name = -1;
  object_depth = INT_MAX;
  GLuint* ptr = (GLuint*)selectBuf;
  for(int i = 0; i < hits; ++i) {
    unsigned int objs = *(ptr++);
    double near = ((double) *(ptr++) / INT_MAX / 2);
    ptr++; // far
    for(int j = 0; j < objs; ++j, ++ptr) {
      if(object_depth > near) {
        object_depth = near;
        object_name = *ptr;
      }
    }
  }
}

void StressCircle::dragHandle(int x, int y) {
  double world_x, world_y, world_z, mvMatrix[16], pjMatrix[16];
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, pjMatrix);
  gluUnProject(x, windowHeight - y, object_depth, mvMatrix, pjMatrix, viewport, &world_x, &world_y, &world_z);
  if(object_name == xHandle.name) { updateStress(world_x, sigma_y, world_y); }
  else if(object_name == yHandle.name) { updateStress(sigma_x, world_x,-world_y); }
  else {
    double theta = std::atan2(world_y, world_x - center()) + ((object_name == yAngle.name) ? M_PI : 0);
    updateStress(center() + radius() * std::cos(theta), center() - radius() * std::cos(theta), radius() * std::sin(theta));
  }
}

#endif // STRESS_CIRCLE_HPP
