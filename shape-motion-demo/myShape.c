#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "shape.h"
#include "buzzer.h"

// like abRectCheck, but excludes a triangle
int abSlicedRectCheck(const AbRect *rect, const Vec2 *centerPos, const Vec2 *pixel)
{
  Vec2 relPos;
  vec2Sub(&relPos, pixel, centerPos); /* vector from center to pixel */

  /* reject pixels in slice */
  if ((relPos.axes[0]/2 >= 0 && relPos.axes[0]/2 >= 0 < relPos.axes[1])) 
    return 0;
  else
    return abRectCheck(rect, centerPos, pixel);
}

AbRect rect101 = {abRectGetBounds, abSlicedRectCheck, 10,20};

Region fence2 = {{10,30}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}};

Layer myShape = {
  (AbShape *)&rect101,
  {screenWidth/2, screenHeight/2}, /* position */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  0,
};

Layer myShape2 = {
  (AbShape *)&rect101,
  {40, 90}, /* position */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &myShape,
};

Layer myShape3 = {
  (AbShape *)&rect101,
  {90, 90}, /* position */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_YELLOW,
  &myShape2,
};

void welcomeScreen()
{
  configureClocks();
  lcd_init();
  shapeInit();
  Vec2 rectPos = screenCenter;//, circlePos = {30,screenHeight - 30};
    
  clearScreen(COLOR_BLUE);
  clearScreen(COLOR_GREEN);
  clearScreen(COLOR_RED);
  clearScreen(COLOR_YELLOW);
  

  layerDraw(&myShape3);
  
  drawString5x7(25,20, "WELCOME!", COLOR_BLACK, COLOR_WHITE);
  drawString5x7(35,30, "LET'S PLAY!", COLOR_BLACK, COLOR_WHITE);
  
}
void winningScreen()
{
  configureClocks();
  lcd_init();
  shapeInit();
  Vec2 rectPos = screenCenter;//, circlePos = {30,screenHeight - 30};
    
  clearScreen(COLOR_YELLOW);
  clearScreen(COLOR_GREEN);
  clearScreen(COLOR_RED);
  clearScreen(COLOR_BLUE);
  

  layerDraw(&myShape3);
  
  drawString5x7(25,20, "CONGRATULATION", COLOR_BLACK, COLOR_WHITE);
  drawString5x7(35,30, "YOU WON!", COLOR_BLACK, COLOR_WHITE);
  
}

void lostScreen()
{
  configureClocks();
  lcd_init();
  shapeInit();
  Vec2 rectPos = screenCenter;//, circlePos = {30,screenHeight - 30};
  
  clearScreen(COLOR_BLUE);
  clearScreen(COLOR_RED);
  clearScreen(COLOR_BLUE);
  clearScreen(COLOR_RED);
  
  layerDraw(&myShape3);
  drawString5x7(35,40, "YOU LOST!", COLOR_BLACK, COLOR_WHITE);

}
