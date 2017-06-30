//
// feedback_pick
// main.cpp
//
// The MIT License
//
// Copyright (c) 2009 sonson, sonson@Picture&Software
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH  300
#define HEIGHT  300
#define BUFSIZE 512

////////////////////////////////////////////////////////////////////
//フィードバックに用いる配列
//各オブジェクトの初期位置
GLdouble object_position[3][3] = {
  {0.0, 0.0, 0.0},
  {1.0, 3.0, 4.5},
  {-1.0, -3.0, 0.0}
};

////////////////////////////////////////////////////////////////////
//ドラッグに用いる
GLuint  object_number=0;
double  object_depth=-1;
static GLuint  move_flag=0;

////////////////////////////////////////////////////////////////////
//3点と三角形の描画
void DrawObjects(GLenum mode){
  glColor3f(1,1,1);
  // 点1
  if(mode==GL_SELECT) { glPushName(1); }
  glBegin(GL_POINTS);
  glVertex3dv(object_position[0]);
  glEnd();
  glPopName();
  // 点2
  if(mode==GL_SELECT) { glPushName(2); }
  glBegin(GL_POINTS);
  glVertex3dv(object_position[1]);
  glEnd();
  glPopName();
  // 点3
  if(mode==GL_SELECT) { glPushName(3); }
  glBegin(GL_POINTS);
  glVertex3dv(object_position[2]);
  glEnd();
  glPopName();
  //3点を結ぶ三角形
  glColor3f(1,0,0);
  if(mode==GL_RENDER){
    glBegin(GL_LINE_LOOP);
    glVertex3dv(object_position[0]);
    glVertex3dv(object_position[1]);
    glVertex3dv(object_position[2]);
    glEnd();
  }
}
///////////////////////////////////////////////////////////////////////////////
//セレクションバッファを整理する
int SelectHits(GLuint hits,GLuint *buf){
  unsigned int i;
  GLuint hit_name;
  float depth_min=10.0f;
  float depth_1=1.0f;
  float depth_2=1.0f;
  GLuint depth_name;
  GLuint *ptr;
  // ヒットしたデータなし
  if(hits<=0) { return -1; }
  // ポインタを作業用ptrへ渡す．
  ptr = (GLuint*)buf;

  for(i=0; i<hits; i++){
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
///////////////////////////////////////////////////////////////////////////////
//ピックアップ
int PickUp(int x, int y){
  GLuint selectBuf[BUFSIZE];
  GLint hits;
  GLint viewport[4];
  float current_aspect;
  glGetIntegerv(GL_VIEWPORT, viewport);
  glSelectBuffer(BUFSIZE, selectBuf);
  (void) glRenderMode(GL_SELECT);
  glInitNames();
  glMatrixMode(GL_PROJECTION);      //プロジェクションモード
  glPushMatrix();              //セレクションモードのときのみ下の行列計算
    glLoadIdentity();          //を反映させるためにPushする
    gluPickMatrix(x,viewport[3]-y,    //ピッキング行列の乗算
      5.0,5.0, viewport);
    current_aspect = (float)viewport[2]/(float)viewport[3];
    gluPerspective( 60.0, current_aspect, 1.0, 100.0 );
    glMatrixMode( GL_MODELVIEW );    //ここでモデルビューモードにする！！！！
    DrawObjects(GL_SELECT);        //セレクションモードで描画する
    glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode( GL_MODELVIEW );      //モデルビューモードへ戻す

  hits = glRenderMode(GL_RENDER);
  return SelectHits(hits,selectBuf);
}
////////////////////////////////////////////////////////////////////
//クリックしたところの描画したもののデプス値を取得する．
float returnDepth(int x, int y){
  float z;
  GLint viewport[4];      //ビューポート
  //デバイス座標系とウィンドウ座標系の変換
  glGetIntegerv(GL_VIEWPORT,viewport);  //現在のビューポートを代入
  glReadPixels(x,viewport[3]-y,1,1,
    GL_DEPTH_COMPONENT,
    GL_FLOAT,
    &z);
  return z;
}
////////////////////////////////////////////////////////////////////
//クリックした点にあるピクセルのデプス値を用いて，世界座標を割り出す．
void  CalculateWorldCo(int x, int y, float depth, double &wx, double &wy,double &wz){
  GLdouble mvMatrix[16],pjMatrix[16];
  GLint viewport[4];

  //ビューポートパラメータを取得する．
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, pjMatrix);
  //世界座標を取得する
  gluUnProject((double)x,(double)viewport[3]-y,depth,
    mvMatrix,
    pjMatrix,
    viewport,
    &wx,
    &wy,
    &wz);
}
////////////////////////////////////////////////////////////////////
//オブジェクトをドラッグする
void PickObject(int x, int y){
  if((object_number = PickUp(x,y))==-1) { return; }
  //Zバッファ値をゲット
  object_depth = returnDepth(x,y);
}
void DruggingObject(int x, int y){
  double xx[3];
  if(object_depth>0 && object_depth<1){
    CalculateWorldCo(x,y,object_depth,xx[0],xx[1],xx[2]);
    object_position[object_number-1][0] = xx[0];
    object_position[object_number-1][1] = xx[1];
    object_position[object_number-1][2] = xx[2];
  }
}
void FreeObject(int x, int y){
  object_number=0;
  object_depth=-1;
}
///////////////////////////////////////////////////////////////////////////////
//描画関数
void display(void){
  //ウィンドウを塗りつぶす
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  //視点の変換行列の初期化
  glLoadIdentity();
  //視点の決定
  gluLookAt(
    0,0,12,
    0,0,0,
    0.0, 1.0, 0.0);

  DrawObjects(GL_RENDER);
  glFlush();
  glutSwapBuffers();
}
///////////////////////////////////////////////////////////////////////////////
//OpenGL初期化コマンド
void init (void){
  glEnable(GL_DEPTH_TEST);
  //クライアント領域の塗りつぶし色の設定
  glClearColor( 0.0, 0.0, 0.0, 1.0 );
  //デプスバッファクリア値の設定
  glClearDepth( 1.0 );
  //点のピクセルサイズの設定
  glPointSize(5.0f);
}
///////////////////////////////////////////////////////////////////////////////
//マウス処理
void mouse(int button, int state, int x, int y){
  switch(button){
    case GLUT_LEFT_BUTTON:
      if(state == GLUT_DOWN){
        if(move_flag==0){
          PickObject(x,y);
          move_flag = 1;
        }
      }
      else if(state == GLUT_UP){
        FreeObject(x,y);
        move_flag=0;
      }
      break;
  }
}
///////////////////////////////////////////////////////////////////////////////
//マウス・ドラッグ処理
void drag(int x, int y){
  if(move_flag==1) { DruggingObject(x,y); }
}
///////////////////////////////////////////////////////////////////////////////
//リサイズ処理
void resize(int w, int h)
{
  GLdouble aspect;
  //それによってビューポートを設定する
  glViewport(0, 0, w, h);
  //アスペクト比の初期化
  aspect = (GLdouble)w/(GLdouble)h;
  //プロジェクションモードで射影
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 60.0, aspect, 1.0,100.0 );
  //ノーマルのモデルビューモードへ移行
  glMatrixMode( GL_MODELVIEW );
}
///////////////////////////////////////////////////////////////////////////////
//メイン関数
int main(int argc, char** argv){
  //GLUT初期化
  glutInit(&argc, argv);
  //GLUTピクセルフォーマット
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
  //ウィンドウの初期化と作成
  glutInitWindowSize (WIDTH,HEIGHT);
  glutInitWindowPosition (100, 100);
  glutCreateWindow ("Simple GLUT");
  //OpenGL初期化
  init();
  //各処理関数の登録
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouse);
  glutMotionFunc(drag);
  glutIdleFunc(display);
  //メインループ
  glutMainLoop();
  return 0;
}
