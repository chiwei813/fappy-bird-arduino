#include <LiquidCrystal.h>

// 初始化 LCD (平行介面)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// 超音波感測器腳位
const int trigPin = 9;
const int echoPin = 8;

// 遊戲參數
int birdY = 1; // 小鳥的初始高度 (LCD 的行數，0 或 1)
const int numTrees = 3; // 樹的數量
int treeX[numTrees]; // 樹的位置
int gapY[numTrees]; // 樹的空隙位置
bool gameOver = false;
int gameSpeed = 500; // 初始遊戲速度 (毫秒)
unsigned long startTime; // 遊戲開始時間
unsigned long elapsedTime; // 遊戲存活時間

// 初始化遊戲
void setup() {
  // 初始化 LCD
  lcd.begin(16, 2);

  // 初始化超音波感測器腳位
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // 顯示遊戲開始訊息
  lcd.print("Flappy Bird!");
  delay(2000);
  lcd.clear();

  // 初始化樹的位置和空隙
  randomSeed(analogRead(0));
  for (int i = 0; i < numTrees; i++) {
    treeX[i] = 16 + i * 5; // 每棵樹間隔 5 列
    gapY[i] = random(0, 2); // 隨機生成空隙
  }

  // 記錄遊戲開始時間
  startTime = millis();
}

void loop() {
  if (gameOver) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game Over!");

    // 計算遊戲存活時間
    elapsedTime = (millis() - startTime) / 1000; // 轉換為秒
    lcd.setCursor(0, 1);
    lcd.print("SurviveTime: ");
    lcd.print(elapsedTime);
    lcd.print("s");

    delay(3000);
    restartGame();
    return;
  }

  // 更新小鳥的位置
  updateBird();

  // 更新樹的位置
  updateTrees();

  // 檢查碰撞
  checkCollision();

  // 繪製遊戲畫面
  drawGame();

  // 遊戲速度逐漸加快
  delay(gameSpeed);
  if (gameSpeed > 100) {
    gameSpeed -= 5; // 每次更新減少延遲，增加速度
  }
}

void updateBird() {
  // 測量距離
  long duration = measureDistance();
  int distance = duration * 0.034 / 2; // 計算距離 (cm)

  // 根據距離調整小鳥的高度
  if (distance < 10) {
    birdY = 1; // 上方
  } else if (distance >= 10 && distance < 20) {
    birdY = 0; // 下方
  }
}

void updateTrees() {
  for (int i = 0; i < numTrees; i++) {
    treeX[i]--; // 樹向左移動
    if (treeX[i] < 0) {
      treeX[i] = 15; // 重置樹的位置
      gapY[i] = random(0, 2); // 隨機生成新的空隙
    }
  }
}

void checkCollision() {
  for (int i = 0; i < numTrees; i++) {
    // 如果小鳥與樹的位置重疊，且不在空隙中，遊戲結束
    if (treeX[i] == 0 && birdY != gapY[i]) {
      gameOver = true;
    }
  }
}

void drawGame() {
  lcd.clear();

  // 繪製小鳥
  lcd.setCursor(0, birdY);
  lcd.print("O"); // 小鳥的符號

  // 繪製樹
  for (int i = 0; i < numTrees; i++) {
    lcd.setCursor(treeX[i], 0);
    if (gapY[i] != 0) lcd.print("|"); // 樹的上部分
    lcd.setCursor(treeX[i], 1);
    if (gapY[i] != 1) lcd.print("|"); // 樹的下部分
  }
}

long measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 讀取 Echo 腳位的高電位時間
  return pulseIn(echoPin, HIGH);
}

void restartGame() {
  birdY = 1;
  gameSpeed = 500; // 重置遊戲速度
  for (int i = 0; i < numTrees; i++) {
    treeX[i] = 16 + i * 5; // 每棵樹間隔 5 列
    gapY[i] = random(0, 2); // 隨機生成空隙
  }
  gameOver = false;

  // 重置遊戲開始時間
  startTime = millis();
}