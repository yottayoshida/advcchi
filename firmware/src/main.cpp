#include <M5Cardputer.h>
#include "clawd_sprites.h"

// ── walk config ──

static constexpr int WALK_RANGE  = 25;
static constexpr int STEP_PX     = 2;
static constexpr int STEP_MS     = 150;
static constexpr int BOB_PX      = 2;

// ── sprite buffer ──

static constexpr int SPRITE_W = clawd::TOTAL_W + WALK_RANGE * 2 + 2;
static constexpr int SPRITE_H = clawd::TOTAL_H + BOB_PX + 2;
static constexpr int SPRITE_X = (240 - SPRITE_W) / 2;
static constexpr int SPRITE_Y = (135 - SPRITE_H) / 2 - 6;

static M5Canvas canvas(&M5Cardputer.Display);

// ── state ──

static clawd::Expression currentExpr = clawd::EXPR_IDLE;

static int  walkX        = 0;
static int  walkDir      = 1;
static int  walkStep     = 0;
static bool walkActive   = true;
static unsigned long nextStepMs    = 0;
static unsigned long nextBlinkMs   = 3000;
static unsigned long reactionEndMs = 0;
static bool          inReaction    = false;

// ── drawing ──

static void drawClawdToCanvas(clawd::Expression expr,
                              int offsetX, int offsetY) {
  canvas.fillSprite(TFT_BLACK);

  const int ox = WALK_RANGE + 1 + offsetX;
  const int oy = BOB_PX + 1 + offsetY;

  const auto *shape = clawd::shapeFor(expr);

  for (int r = 0; r < clawd::GRID_ROWS; r++) {
    for (int c = 0; c < clawd::GRID_COLS; c++) {
      if (shape[r][c]) {
        canvas.fillRect(ox + c * clawd::QW, oy + r * clawd::QH,
                        clawd::QW, clawd::QH, clawd::COLOR_MAIN);
      }
    }
  }

  canvas.pushSprite(SPRITE_X, SPRITE_Y);
}

static void showStatus(clawd::Expression expr) {
  const char* labels[] = {
      "idle", "blink", "happy", "surprised", "sleepy", "excited"};
  int ty = 135 - 14;
  M5Cardputer.Display.fillRect(0, ty, 240, 14, TFT_BLACK);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(0x7BEF);
  M5Cardputer.Display.setCursor(4, ty + 3);
  M5Cardputer.Display.printf("%s  heap:%u", labels[expr],
                             ESP.getFreeHeap());
}

// ── animation ──

static void animTick(unsigned long now) {
  if (walkActive && now >= nextStepMs) {
    walkX += walkDir * STEP_PX;
    if (walkX >= WALK_RANGE || walkX <= -WALK_RANGE) {
      walkDir = -walkDir;
    }
    walkStep++;
    nextStepMs = now + STEP_MS;
  }

  int bobY = (walkActive && (walkStep & 1)) ? -BOB_PX : 0;

  if (!inReaction && currentExpr == clawd::EXPR_IDLE) {
    if (now > nextBlinkMs) {
      currentExpr = clawd::EXPR_BLINK;
      reactionEndMs = now + 150;
      inReaction = true;
      nextBlinkMs = now + 3000 + (esp_random() % 5000);
    }
  }

  if (inReaction && now > reactionEndMs) {
    currentExpr = clawd::EXPR_IDLE;
    inReaction = false;
    walkActive = true;
  }

  drawClawdToCanvas(currentExpr, walkX, bobY);
}

// ── key handling ──

static void triggerExpression(clawd::Expression expr, unsigned long durationMs,
                              int toneHz) {
  currentExpr = expr;
  reactionEndMs = millis() + durationMs;
  inReaction = true;
  walkActive = false;

  if (toneHz > 0) {
    M5Cardputer.Speaker.tone(toneHz, 80);
  }
  showStatus(expr);
}

static int keyPressCount = 0;

static void handleKey() {
  keyPressCount++;
  int idx = keyPressCount % (clawd::EXPR_COUNT - 1);
  clawd::Expression exprs[] = {
      clawd::EXPR_HAPPY,     clawd::EXPR_SURPRISED,
      clawd::EXPR_SLEEPY,    clawd::EXPR_EXCITED,
      clawd::EXPR_IDLE,
  };
  int tones[] = {1200, 800, 500, 1500, 1000};
  triggerExpression(exprs[idx], 1200, tones[idx]);
}

// ── setup / loop ──

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  Serial.begin(115200);

  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.fillScreen(TFT_BLACK);

  canvas.setColorDepth(16);
  canvas.createSprite(SPRITE_W, SPRITE_H);

  showStatus(clawd::EXPR_IDLE);

  M5Cardputer.Speaker.setVolume(64);
  M5Cardputer.Speaker.tone(880, 100);
  delay(120);
  M5Cardputer.Speaker.tone(1100, 80);

  Serial.printf("[clawd] char %dx%d, sprite %dx%d (%dB), heap: %u\n",
                clawd::TOTAL_W, clawd::TOTAL_H,
                SPRITE_W, SPRITE_H, SPRITE_W * SPRITE_H * 2,
                ESP.getFreeHeap());
}

void loop() {
  M5Cardputer.update();

  if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
    handleKey();
    Serial.printf("[clawd] key → expr=%d\n", currentExpr);
  }

  animTick(millis());
  delay(16);
}
