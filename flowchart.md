https://knsv.github.io/mermaid/live_editor/

``` mermaid
graph TB
  subgraph int main
    glutinit[GLUT初期化/ウィンドウ生成] --> cb[コールバック登録]
    cb --> cl[クリア色/デプス初期化]
    cl --> mr["glutMainLoop()"]
    mr --> mouse((mouse))
    mr --> motion((motion))
    mr --> reshape((reshape))
    mr --> idle((idle))
    mr --> reset((Resetボタン))
    mr --> exit((Exitボタン))
    mouse --> f[各コールバック関数呼び出し]
    motion --> f
    reshape --> f
    idle --> f
    reset --> f
    exit --> ret["exit(0)"]
    f --> mr
  end
  subgraph GLUI初期化: void gluiInit
    cb --> subw[サブウィンドウ生成]
    subw --> ui[各UI生成<br>Live variables登録]
    ui -.-> cb
  end
  subgraph 初期描画: void display
    cl --> disp[OpenGLオブジェクト描画]
    disp -.-> cl
  end
```