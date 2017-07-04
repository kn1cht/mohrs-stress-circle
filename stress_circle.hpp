#ifndef   STRESS_CIRCLE_HPP
#define   STRESS_CIRCLE_HPP

#include <climits>
#include <cmath>

const float SIGMA_X_INITIAL = 2.0;
const float SIGMA_Y_INITIAL =-2.0;
const float TAU_INITIAL = 0.0;

struct StressCircle {
  StressCircle(unsigned int name1, unsigned int name2, unsigned int w, unsigned int h);
  float sigma_x, sigma_y, tau, sigma_1, sigma_2, tau_max, angle;
  unsigned int x_handle_name;
  unsigned int y_handle_name;
  unsigned int angle_handle_name = 3;
  unsigned int windowWidth = 800;
  unsigned int windowHeight = 600;
  unsigned int object_name = 0;
  double object_depth = -1;
  const int CIRCLE_DIVISION_NUM = 100;
  const int HANDLE_DIVISION_NUM = 4;
  const float HANDLE_RADIUS = 0.1;
  const float SMALL_HANDLE_RADIUS = 0.05;
  const int BUF = 512;
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

StressCircle::StressCircle(unsigned int name1, unsigned int name2, unsigned int w, unsigned int h) :
  sigma_x(SIGMA_X_INITIAL),sigma_y(SIGMA_Y_INITIAL),tau(TAU_INITIAL),
  x_handle_name(name1), y_handle_name(name2), windowWidth(w), windowHeight(h) {
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
  glColor3f(1.0, 1.0, 1.0);
  glLineWidth(2);
  glBegin(GL_LINES);
  for (int i = 0; i < CIRCLE_DIVISION_NUM; ++i) {
    double theta = 2.0 * M_PI * i / CIRCLE_DIVISION_NUM;
    glVertex3d(radius() * std::cos(theta) + center(), radius() * std::sin(theta), -0.01);
    theta = 2.0 * M_PI * (i + 1) / CIRCLE_DIVISION_NUM;
    glVertex3d(radius() * std::cos(theta) + center(), radius() * std::sin(theta), -0.01);
  }
  glEnd();
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_POLYGON);
  for (int i = 0; i < CIRCLE_DIVISION_NUM; ++i) {
    double theta = 2.0 * M_PI * i / CIRCLE_DIVISION_NUM;
    glVertex2d(SMALL_HANDLE_RADIUS * std::cos(theta) + sigma_1, SMALL_HANDLE_RADIUS * std::sin(theta));
  }
  glEnd();
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_POLYGON);
  for (int i = 0; i < CIRCLE_DIVISION_NUM; ++i) {
    double theta = 2.0 * M_PI * i / CIRCLE_DIVISION_NUM;
    glVertex2d(SMALL_HANDLE_RADIUS * std::cos(theta) + sigma_2, SMALL_HANDLE_RADIUS * std::sin(theta));
  }
  glEnd();
}

void StressCircle::drawHandles(GLenum mode) {
  if(mode == GL_SELECT) { glLoadName(x_handle_name); }
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_QUADS);
  for(int i = 0; i < HANDLE_DIVISION_NUM; ++i) {
    double theta = 2.0 * M_PI * i / HANDLE_DIVISION_NUM + M_PI / 4;
    glVertex2f(sigma_x + HANDLE_RADIUS * std::cos(theta), tau + HANDLE_RADIUS * std::sin(theta));
  }
  glEnd();
  if(mode == GL_SELECT) { glLoadName(y_handle_name); }
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_QUADS);
  for(int i = 0; i < HANDLE_DIVISION_NUM; ++i) {
    double theta = 2.0 * M_PI * i / HANDLE_DIVISION_NUM + M_PI / 4;
    glVertex2f(sigma_y + HANDLE_RADIUS * std::cos(theta),-tau + HANDLE_RADIUS * std::sin(theta));
  }
  glEnd();
  if(mode == GL_SELECT) { glLoadName(angle_handle_name); }
  glColor3f(0.0, 1.0, 1.0);
  glLineWidth(5);
  glBegin(GL_LINES);
  glVertex2d(sigma_x, tau);
  glVertex2d(sigma_y,-tau);
  glEnd();
  glLineWidth(1);
  glFlush();
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
  double world_x, world_y,  world_z, mvMatrix[16], pjMatrix[16];
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, pjMatrix);
  gluUnProject(x, windowHeight - y, object_depth, mvMatrix, pjMatrix, viewport, &world_x, &world_y, &world_z);
  if(object_name == 1) {
    updateStress(world_x, sigma_y, world_y);
  }
  else if(object_name == 2) {
    updateStress(sigma_x, world_x,-world_y);
  }
  else if(object_name == 3) {
    double theta = std::atan2(world_y, world_x - center());
    if((tau && world_y) || (!tau && !world_y)) { // sigma_x side
      updateStress(center() + radius() * std::cos(theta), 
                   center() - radius() * std::cos(theta),
                   radius() * std::sin(theta));
    }
    else { // sigma_y side
      updateStress(center() - radius() * std::cos(theta), 
                   center() + radius() * std::cos(theta),
                   radius() * std::sin(theta));
    }
  }
}

#endif // STRESS_CIRCLE_HPP
