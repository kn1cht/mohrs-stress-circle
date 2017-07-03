#ifndef   STRESS_CIRCLE_HPP
#define   STRESS_CIRCLE_HPP

struct StressCircle {
  StressCircle();
  float sigma_x, sigma_y, tau;
  static const int CircleDivisionNum = 100;
  float center();
  float radius();
  void update(float sigma_x, float sigma_y, float tau);
  void draw();
};

StressCircle::StressCircle() : sigma_x(2.0),sigma_y(-2.0),tau(0.0) {};

float StressCircle::center() { return (sigma_x + sigma_y) / 2; }

float StressCircle::radius() { return std::sqrt(std::pow(sigma_x - sigma_y, 2) / 4 + std::pow(tau, 2)); }

void StressCircle::update(float sigma_x_new, float sigma_y_new, float tau_new){
  sigma_x = sigma_x_new;
  sigma_y = sigma_y_new;
  tau = tau_new;
}

void StressCircle::draw() {
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);
  for (int i = 0; i <= CircleDivisionNum; ++i) {
    double theta = 2.0 * M_PI * i / CircleDivisionNum;
    glVertex2d(radius() * cos(theta) + center(), radius() * sin(theta));
    theta = 2.0 * M_PI * (i + 1) / CircleDivisionNum;
    glVertex2d(radius() * cos(theta) + center(), radius() * sin(theta));
  }
  glEnd();
}

#endif // STRESS_CIRCLE_HPP