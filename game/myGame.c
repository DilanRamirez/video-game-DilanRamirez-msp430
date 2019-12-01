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
#include "buzzer.h"

#define GREEN_LED BIT6

/*Buttons are assigned by the bit it uses from BIT0 to BIT4*/
#define SW0 BIT0
#define SW1 BIT1
#define SW2 BIT2
#define SW3 BIT3

#define SWITCHES (SW0|SW1|SW2|SW3) //Switches are defined for the buttons

//All the bars and squares used in the game are declared as an AbRect
AbRect rect10 = {abRectGetBounds, abRectCheck, {20,2}}; // 20x2 rectangle 
AbRect rect11 = {abRectGetBounds, abRectCheck, {20,2}}; // 20x2 rectangle
AbRect rect12 = {abRectGetBounds, abRectCheck, {20,2}}; // 20x2 rectangle
AbRect rect15 = {abRectGetBounds, abRectCheck, {6,6}}; //  6x6 square


AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2-10, screenHeight/2-10}
};


// A layer is created for every AbRect shape 
Layer layer10 = {
  (AbShape *)&rect10,
  {screenWidth/2, 30}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  0
};
 
Layer layer11 = {		/**< Layer with a red square */
  (AbShape *)&rect11,
  {screenWidth/2, 80}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer10,
};

Layer layer12 = {		/**< Layer with a green bar */
  (AbShape *)&rect12,
  {screenWidth/2, 120}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_YELLOW,
  &layer11,
};

Layer layer13 = {		/**< Layer with a black bar */
  (AbShape *)&rect15,
  {screenWidth/2, 140}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLUE,
  &layer12,
};

Layer fieldLayer = {		/* playing field as a layer */
  (AbShape *) &fieldOutline,
  {screenWidth/2, screenHeight/2},/**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  &layer13,
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

/* initial value of {0,0} will be overwritten. This part generates the velocity for every layer. I set a 0 velocity in x in order to make the bars only move horizontally */
MovLayer ml10 = { &layer10, {2,0}, 0 }; // Red bar
MovLayer ml11 = { &layer11, {3,0}, &ml10 };// Green bar
MovLayer ml12 = { &layer12, {2,0}, &ml11}; // Yellow bar
MovLayer ml13 = { &layer13, {0,0}, &ml12 }; // Blue square

void movLayerDraw(MovLayer *movLayers, Layer *layers)
{
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

int state = 0; // state is a varible used in my state machine which is used in the lives board.

/* Score is the function that controls the lives, and it decreases a life every time the blue square touches a bar from three lives to game over*/
void score(){
  char life3[2] = "3";
  char life2[2] = "2";
  char life1[2] = "1";
  char gameover[15] = "game over";
 
  drawString5x7(50,2, "life:", COLOR_BLACK, COLOR_WHITE);
  
  switch(state){
    case 0:
      drawString5x7(80,2, life3, COLOR_BLACK, COLOR_WHITE);
      break;
    case 1:
      drawString5x7(80,2, life2, COLOR_BLACK, COLOR_WHITE);
      break;
    case 2:
      drawString5x7(80,2, life1, COLOR_BLACK, COLOR_WHITE);
      break;
    case 3:
      lostScreen();
      break;
    default:
      drawString5x7(77,2, gameover, COLOR_BLACK, COLOR_WHITE);
      break;
  }
}

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
  Region rect; // A new region is created only for the blue square
  Region bar1; // A new region is created only for the bars blue square
  MovLayer *head = &ml12;

  /*The linked list is break in order to separate the blue square from the bars and detect collisions*/
  for(int i=0; i<2;i++){
      vec2Add(&newPos, &ml13.layer->posNext, &ml13.velocity);
      abShapeGetBounds(ml13.layer->abShape, &newPos, &rect);

      vec2Add(&newPos, &head->layer->posNext, &head->velocity);
      abShapeGetBounds(head->layer->abShape, &newPos, &bar1);
 
   for (axis = 0; axis < 2; axis ++) {
     /*If the blue square touches the top of the gaming field, the player wins*/
      if(rect.topLeft.axes[1] < fence->topLeft.axes[1]){
	     winningScreen(); // it calls a function that shows my original rendered shape
	     drawString5x7(90,152, "WIN", COLOR_BLACK, COLOR_WHITE);
	     break;
      }

      /*This part detects the collision between the blue square and the moving bars by checking the        top left and bottom right of the bars and the blue square  */
      if((rect.topLeft.axes[axis] == bar1.botRight.axes[axis] ||
	  rect.botRight.axes[axis] == bar1.topLeft.axes[axis] ) &&
	  rect.topLeft.axes[!axis]-2 > bar1.topLeft.axes[!axis] &&
	 rect.botRight.axes[!axis]+2 < bar1.botRight.axes[!axis]){
	    state++;
	    score();
	    sounds(state);
	    drawString5x7(40,152, "Keep Playing", COLOR_BLACK, COLOR_WHITE);
      }
   }
    head = head->next;
 }

  /*This part makes the bars and the blue square move*/
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||
	  (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]) ) {
	buzzer_set_period(0);
	int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (2*velocity);
      }/**< if outside of fence */
      else if((rect.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||
	      (rect.botRight.axes[axis] > fence->botRight.axes[axis])){
	   if(state<2)
	     buzzer_set_period(550); // every time the square touches a wall, it makes a sound
      }
      }/**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}


u_int bgColor = COLOR_WHITE;    /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */
Region fieldFence;	/**< fence around playing field  */

/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */
void main()
{
  configureClocks();
  welcomeScreen();
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
  P1OUT |= GREEN_LED;

  lcd_init();
  shapeInit();
  p2sw_init(15);

  shapeInit();
  buzzer_init();
  
  layerInit(&fieldLayer);
  layerDraw(&fieldLayer);

  layerGetBounds(&fieldLayer, &fieldFence);

  score();
  sounds(state);
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */


  for(;;) { 
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10);	      /**< CPU OFF */
    }
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    redrawScreen = 0;
    movLayerDraw(&ml13, &fieldLayer);
  }
}

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
    static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  
  /*This part uses the switches and sets the buttons in order to make the blue square move. They ar    e the controls of the blue square*/
  while(count == 15){
    u_int switches = p2sw_read();
    mlAdvance(&ml13, &fieldFence);
    count = 0;
    if(~switches & SW3){
      ml13.velocity.axes[0] = 3;
      ml13.velocity.axes[1] = 0;
    }
    else if(~switches & SW0){
      ml13.velocity.axes[0] = -3;
      ml13.velocity.axes[1] = 0;
    }
    else if(~switches & SW1){
      ml13.velocity.axes[0] = 0;
      ml13.velocity.axes[1] = 1;
    }
    else if(~switches & SW2){
      ml13.velocity.axes[0] = 0;
      ml13.velocity.axes[1] = -1;
    }
    if(p2sw_read())
      redrawScreen = 1;
  } 
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
/**< Green LED off when cpu off */
}
