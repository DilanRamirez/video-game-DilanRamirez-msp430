/** \file shapemotion.c
 *  \brief This is a simple shape motion demo.
 *  This demo creates two layers containing shapes.
 *  One layer contains a rectangle and the other a circle.
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */  
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>

#define GREEN_LED BIT6
#define SW0 BIT0
#define SW1 BIT1
#define SW2 BIT2
#define SW3 BIT3

#define SWITCHES (SW0|SW1|SW2|SW3)

int score;
int life = 5;

AbRect rect10 = {abRectGetBounds, abRectCheck, {17,2}}; /**< 10x10 rectangle */
AbRect rect11 = {abRectGetBounds, abRectCheck, {12,2}};
AbRect rect12 = {abRectGetBounds, abRectCheck, {20,2}};
AbRect rect13 = {abRectGetBounds, abRectCheck, {16,2}};
AbRect rect14 = {abRectGetBounds, abRectCheck, {17,2}};
AbRect rect15 = {abRectGetBounds, abRectCheck, {6,6}};

AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2-1, screenHeight/2-1}
};

Layer layer10 = {
  (AbShape *)&rect10,
  {screenWidth/2, 4}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLUE,
  0
};
 
Layer layer11 = {		/**< Layer with a red square */
  (AbShape *)&rect11,
  {screenWidth/2, 36}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &layer10,
};

Layer layer12 = {		/**< Layer with a green bar */
  (AbShape *)&rect12,
  {screenWidth/2, 68}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer11,
};

Layer layer13 = {		/**< Layer with a black bar */
  (AbShape *)&rect13,
  {screenWidth/2, 100}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  &layer12,
};

Layer layer14 = {		/**< Layer with a black bar */
  (AbShape *)&rect14,
  {screenWidth/2, 132}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_YELLOW,
  &layer13,
};

Layer layer15 = {		/**< Layer with a black bar */
  (AbShape *)&rect15,
  {screenWidth/2, 150}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &layer14,
};

Layer fieldLayer = {		/* playing field as a layer */
  (AbShape *) &fieldOutline,
  {screenWidth/2, screenHeight/2},/**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  &layer15,
};


/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */
MovLayer ml10 = { &layer10, {4,0}, 0 }; /**< not all layers move */
MovLayer ml11 = { &layer11, {3,0}, &ml10 };
MovLayer ml12 = { &layer12, {6,0}, &ml11};
MovLayer ml13 = { &layer13, {5,0}, &ml12 };
MovLayer ml14 = { &layer14, {2,0}, &ml13 };
MovLayer ml15 = { &layer15, {0,0}, &ml14 }; 



void movLayerDraw(MovLayer *movLayers, Layer *layers){
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */


  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  



//Region fence = {{10,30}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}}; /**< Create a fence region */

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */

void mlAdvance(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  Region rectBoundary;
  MovLayer *head = ml;

 vec2Add(&newPos, &ml15.layer->posNext, &ml15.velocity);
 abShapeGetBounds(ml15.layer->abShape, &newPos, &rectBoundary);
 for(int i=0; i<4;i++){
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < rectBoundary.topLeft.axes[axis]) ||
	  (shapeBoundary.botRight.axes[axis] > rectBoundary.botRight.axes[axis]) ) {
	drawString5x7(3,5, "Game Over!", COLOR_BLACK, COLOR_WHITE);
	P1OUT &= GREEN_LED;
      }
    ml->next;
    }
 }

  ml = head;

 for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||
	 (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]) ) {
	    int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
            newPos.axes[axis] += (2*velocity);
      }/**< if outside of fence */
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}


u_int bgColor = COLOR_WHITE;     /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */
Region fieldFence;		/**< fence around playing field  */


/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */
void main()
{
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  shapeInit();
  p2sw_init(15);

  shapeInit();

  layerInit(&fieldLayer);
  layerDraw(&fieldLayer);


  layerGetBounds(&fieldLayer, &fieldFence);


  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */

  drawString5x7(3,5, "Life: 3!", COLOR_BLACK, COLOR_WHITE);
  for(;;) { 
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10);	      /**< CPU OFF */
    }
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    redrawScreen = 0;
    movLayerDraw(&ml15, &fieldLayer);
  }
}

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  while(count == 15){
    u_int switches = p2sw_read();
    mlAdvance(&ml15, &fieldFence);
    count = 0;
    if(~switches & SW3){
      ml15.velocity.axes[0] = 3;
      ml15.velocity.axes[1] = 0;
    }
    else if(~switches & SW0){
      ml15.velocity.axes[0] = -3;
      ml15.velocity.axes[1] = 0;
    }
    else if(~switches & SW1){
      ml15.velocity.axes[0] = 0;
      ml15.velocity.axes[1] = 3;
    }
    else if(~switches & SW2){
      ml15.velocity.axes[0] = 0;
      ml15.velocity.axes[1] = -3;
    }
    if(p2sw_read())
      redrawScreen = 1;
  } 
  // P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}
