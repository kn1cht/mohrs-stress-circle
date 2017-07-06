https://knsv.github.io/mermaid/live_editor/

# display()
``` mermaid
sequenceDiagram
display()->>display(): バッファ・変換行列・カメラ初期化
display()->>display(): 軸描画: drawAxis()
display()->>StressCircle: updateVal()
StressCircle->>StressCircle: 現在の応力値から表示する数値を計算
display()->>StressCircle: drawCircle()
StressCircle->>StressCircle: 応力円描画/主応力描画
display()->>StressCircle: drawHandles(GL_RENDER)
StressCircle->>StressCircle: 応力値変更用要素描画\n(x軸ハンドル/y軸ハンドル/角度ハンドル)
display()->>StressCircle: drawString()
StressCircle->>StressCircle: 各種数値を文字列化して描画
display()->>display(): glFlush() glutSwapBuffers()
```

# mouse(int button, int state, int x, int y)
``` mermaid
sequenceDiagram
opt ボタン押下時
  mouse()->>StressCircle: pickHandle(x, y)
  StressCircle->>StressCircle: マウスピッキング処理
Note right of StressCircle: object_nameにクリ<br>ックしたオブジェク<br>トの番号を保持する
end
mouse()->>mouse(): ドラッグフラグ反転
```

# motion(int x, int y)
``` mermaid
sequenceDiagram
opt ドラッグフラグがtrueの時のみ
motion()->>StressCircle: dragHandle(x, y)
Note right of StressCircle: x,y: マウス座標
StressCircle->>StressCircle: ウィンドウ座標→ワールド座標変換
opt x軸ハンドル
  StressCircle->>StressCircle: σ_x,τ更新
end
opt y軸ハンドル
  StressCircle->>StressCircle: σ_y,τ更新
end
opt 角度ハンドル
  StressCircle->>StressCircle: σ_x,σ_y,τ算出・更新
end
motion()->>GLUI: sync_live()
GLUI->>GLUI: Live variables更新
end
```

# reshape (int w, int h)
``` mermaid
sequenceDiagram
reshape()->>StressCircle: 新しいウィンドウサイズを渡す
reshape()->>GLUI_Master: auto_set_viewport() : GLUT表示部分のviewport取得
reshape()->>reshape(): 投影変換の再設定
```

# idle()
``` mermaid
sequenceDiagram
idle()->>GLUI: sync_live(): Live Valuesを各ポインタから読み込む
GLUI-->StressCircle: 数値取得
idle()->>idle(): glutPostRedisplay(): 表示更新
```

# gruiReset(int num)
``` mermaid
sequenceDiagram
gruiReset()->>StressCircle: resetVal()
StressCircle->>StressCircle: 数値を初期値に戻す
gruiReset()->>GLUI: sync_live(): Live Valuesを各ポインタから読み込む
GLUI-->StressCircle: 数値取得
```