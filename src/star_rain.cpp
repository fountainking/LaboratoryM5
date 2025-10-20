#include "star_rain.h"
#include "config.h"
#include "ui.h"
#include <M5Cardputer.h>
#include <M5Unified.h>

// Global state
std::vector<FallingStar> fallingStars;
StarRainMode currentStarMode;
bool starRainActive = false;
float gravityX = 0.0f;  // -1.0 to 1.0 (left to right tilt)
float gravityY = 1.0f;  // 0.5 to 2.0 (slow to fast)

// Color palette for stars - Original colors plus shades of blue
const uint16_t starColors[] = {
  // Original colors
  0xAD55,  // Light blue
  0x001F,  // Blue
  0xF800,  // Red
  0xFC00,  // Orange
  0xFFE0,  // Yellow
  0xFFE0,  // Yellow (extra for more yellow stars)
  0x8410,  // Grey
  // Additional blue shades
  0x0013,  // Deep blue
  0x0017,  // Navy blue
  0x001B,  // Royal blue
  0x035F,  // Medium blue
  0x04BF,  // Sky blue
  0x05FF,  // Bright blue
  0x07FF,  // Cyan
  0x867D,  // Pale blue
  0x9E7F,  // Baby blue
  0xB6DF,  // Ice blue
  0xCEFF,  // Powder blue
  0xE73F,  // Periwinkle
  0x051F   // Electric blue
};
const int numColors = 20;

void initStarRain(StarRainMode mode) {
  currentStarMode = mode;
  starRainActive = true;
  gravityX = 0.0f;
  gravityY = 1.0f;

  // Initialize IMU - M5.begin() should have done this, but try again
  M5.Imu.begin();

  // Clear existing stars
  fallingStars.clear();

  // Pre-allocate star slots
  fallingStars.reserve(MAX_STARS);

  // Initialize all stars as inactive
  for (int i = 0; i < MAX_STARS; i++) {
    FallingStar star;
    star.active = false;
    fallingStars.push_back(star);
  }

  // Clear screen - white for landing page, black for terminal, transparent for screensaver
  uint16_t bgColor = (mode == STARRAIN_TERMINAL || mode == STARRAIN_SCREENSAVER) ? TFT_BLACK : TFT_WHITE;

  // Only clear screen for landing page and terminal, not for screensaver (transparent overlay)
  if (mode == STARRAIN_LANDING || mode == STARRAIN_TERMINAL) {
    M5Cardputer.Display.fillScreen(bgColor);
  }

  // Draw status bar and nav dots for landing page (drawn once here, not every frame)
  if (mode == STARRAIN_LANDING) {
    drawStatusBar(false);  // Non-inverted for white background
    drawIndicatorDots(0, 4, false);  // Position 0 of 4 main items
  }

  // Spawn many initial stars at random positions for heavy pouring rain effect
  int minY = (mode == STARRAIN_LANDING) ? 25 : 0;  // Start below status bar for landing
  for (int i = 0; i < 100; i++) {
    spawnStar();
    // Place them at random Y positions so they don't all start at top
    if (fallingStars[i].active) {
      fallingStars[i].y = random(minY, 135);
    }
  }
}

void updateStarRain() {
  if (!starRainActive) return;

  // Update accelerometer-based gravity
  updateStarGravity();

  // Update each star
  for (auto& star : fallingStars) {
    if (!star.active) continue;

    // Save previous position before updating
    star.prevX = star.x;
    star.prevY = star.y;

    // Apply vertical gravity
    star.y += star.speed * gravityY;

    // Apply horizontal gravity (tilt)
    if (gravityX != 0) {
      star.x += (int)(gravityX * 2);  // Multiply for more noticeable effect

      // Keep within bounds
      if (star.x < 0) star.x = 0;
      if (star.x >= STAR_COLUMNS) star.x = STAR_COLUMNS - 1;
    }

    // Deactivate if off screen
    if (star.y > 135 || star.y < -10) {
      star.active = false;
    }
  }

  // Spawn multiple stars per frame to maintain heavy density
  // With 1500 max stars and fast fall speeds, we need to spawn many per frame
  for (int i = 0; i < 5; i++) {
    if (random(100) < STAR_SPAWN_CHANCE) {
      spawnStar();
    }
  }
}

void drawStarRain() {
  if (!starRainActive) return;

  // Determine background color based on mode and menu state
  uint16_t bgColor;
  if (currentStarMode == STARRAIN_SCREENSAVER) {
    // For screensaver, match the menu underneath
    bool inverted = (currentState == APPS_MENU || currentState == SCREEN_VIEW);
    bgColor = inverted ? TFT_BLACK : TFT_WHITE;
  } else if (currentStarMode == STARRAIN_TERMINAL) {
    bgColor = TFT_BLACK;
  } else {
    bgColor = TFT_WHITE;  // STARRAIN_LANDING
  }

  // For landing page, only clear below status bar to avoid flicker
  if (currentStarMode == STARRAIN_LANDING) {
    M5Cardputer.Display.fillRect(0, 25, 240, 110, bgColor);  // Clear main area (below status bar, above bottom text)
    // Status bar and nav dots are drawn once in initStarRain, don't redraw them here
  } else if (currentStarMode == STARRAIN_TERMINAL) {
    // Clear screen for terminal mode
    M5Cardputer.Display.fillScreen(bgColor);
  }
  // For screensaver, don't clear the screen - transparent overlay!

  // Draw each raindrop as single asterisk - no trails
  // For landing page, draw below status bar (y >= 25)
  int minY = (currentStarMode == STARRAIN_LANDING) ? 25 : 0;

  M5Cardputer.Display.setTextSize(1);
  for (const auto& star : fallingStars) {
    if (!star.active) continue;

    // Skip stars that are in the status bar area for landing page
    if (star.y < minY) continue;

    // Erase old position - use black for screensaver to create pooling effect
    if (star.prevY >= minY) {
      int prevPixelX = star.prevX * 6;
      int prevPixelY = star.prevY;
      // Clear old position with background color (black for screensaver/terminal, white for landing)
      M5Cardputer.Display.fillRect(prevPixelX, prevPixelY, 6, 8, bgColor);
    }

    // Calculate pixel position
    int pixelX = star.x * 6;  // 6 pixels per character width
    int pixelY = star.y;

    // Draw just the asterisk - no trails
    M5Cardputer.Display.setTextColor(star.color);
    M5Cardputer.Display.drawChar('*', pixelX, pixelY);
  }

  // For screensaver mode, we don't redraw the menu to avoid flashing
  // The stars will occasionally cover text, but they move so text shows through

  // Draw hint text based on mode
  M5Cardputer.Display.setTextSize(1);

  // Choose text color based on background
  uint16_t textColor = (currentStarMode == STARRAIN_TERMINAL) ? TFT_DARKGREY : TFT_LIGHTGREY;
  M5Cardputer.Display.setTextColor(textColor);

  if (currentStarMode == STARRAIN_LANDING) {
    M5Cardputer.Display.drawString("Press any key...", 75, 110);
  } else if (currentStarMode == STARRAIN_TERMINAL) {
    M5Cardputer.Display.drawString("Press ` to exit", 75, 125);
  }
}

void stopStarRain() {
  starRainActive = false;
  fallingStars.clear();
}

void updateStarGravity() {
  // Try to read accelerometer data
  M5.update();
  auto imu_update = M5.Imu.update();

  if (imu_update) {
    auto data = M5.Imu.getImuData();

    // Only use IMU if we're getting non-zero readings
    if (data.accel.x != 0.0f || data.accel.y != 0.0f) {
      gravityX = data.accel.x * -3.0f;
      if (gravityX > 3.0f) gravityX = 3.0f;
      if (gravityX < -3.0f) gravityX = -3.0f;

      gravityY = 1.0f + (data.accel.y * -1.2f);
      if (gravityY < 0.2f) gravityY = 0.2f;
      if (gravityY > 3.0f) gravityY = 3.0f;

      if (data.accel.z < -0.5f) {
        gravityY = -abs(gravityY);
      }
      return;
    }
  }

  // No IMU available - stars fall straight down with no horizontal movement
  gravityX = 0.0f;
  gravityY = 1.0f;
}

void spawnStar() {
  // Find an inactive star slot
  for (auto& star : fallingStars) {
    if (!star.active) {
      star.active = true;
      star.x = random(0, STAR_COLUMNS);
      star.y = -8;  // Start just above screen
      star.prevX = star.x;  // Initialize previous position
      star.prevY = star.y;
      star.speed = getRandomStarSpeed();
      star.color = getRandomStarColor();
      star.symbol = '*';
      star.trail = 0;  // No trails
      star.trailOpacity = 0;
      return;
    }
  }
}

uint16_t getRandomStarColor() {
  return starColors[random(0, numColors)];
}

int getRandomStarSpeed() {
  // Faster speeds with higher minimum
  int speedVariation = random(0, 100);
  if (speedVariation < 30) {
    return STAR_SPEED_BASE + random(4, 7);   // Fast: 7-10
  } else if (speedVariation < 70) {
    return STAR_SPEED_BASE + random(7, 11);  // Faster: 10-14
  } else {
    return STAR_SPEED_BASE + random(11, 16); // Very fast: 14-19
  }
}
