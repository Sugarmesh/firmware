#include "Thumby.h"

#define THUMBY_COLS 7
#define THUMBY_ROWS 6
#define THUMBY_NUM_KEYS 42

using Key = TCA8418KeyboardBase::TCA8418Key;

enum ModifierKey : uint8_t {    //setup for modifier layers
    MOD_SHIFT = 0xF0,
    MOD_ALT   = 0xF1,
};

//      c0  c1  c2  c3  c4  c5      c6
//r0    q   w   e   r   t   y       UP
//r1    u   i   o   p   a   s       LEFT
//r2    d   f   g   h   j   k       DOWN
//r3    L   BSP alt z   x   c       RIGHT
//r4    v   b   n   m   @   SELECT  SELECT
//r5    shf 0   spc ESC shf

static unsigned char ThumbyKeyMap[THUMBY_NUM_KEYS][3 /*normal, shift, alt*/] = {
    /*r0*/
    {'q','Q','#'},      //q
    {'w','W','1'},      //w
    {'e','E','2'},      //e
    {'r','R','3'},      //r
    {'t','T','('},      //t
    {'y','Y',')'},      //y
    {UP, 0,0},          //hat up
    /*r1*/
    {'u','U','_'},      //u
    {'i','I','-'},      //i
    {'o','O','+'},      //o
    {'p','P','$'},      //p
    {'a','A','*'},      //a
    {'s','S','4'},      //s
    {LEFT, 0,0},        //hat left
    /*r2*/
    {'d','D','5'},      //d
    {'f','F','6'},      //f
    {'g','G','/'},      //g
    {'h','H',':'},      //h
    {'j','J',';'},      //j
    {'k','K','\\'},     //k
    {DOWN, 0,0},        //hat down
    /*r3*/
    {'l','L','"'},      //l
    {BSP, 0,0},         //del
    {MOD_ALT, 0,0},     //alt
    {'z','Z','7'},      //z
    {'x','X','8'},      //x
    {'c','C','9'},      //c
    {RIGHT,0,0},        //hat right
    /*r4*/
    {'v','V','?'},      //v
    {'b','B','!'},      //b
    {'n','N',','},      //n
    {'m','M','.'},      //m
    {'@', 0,'>'},       //@
    {SELECT, 0,0},      //enter
    {SELECT, 0,0},      //hat enter
    /*r5*/
    {MOD_SHIFT,0,0},    //shift
    {'0',0,'<'},        //0
    {' ',0,0},          //space
    {ESC,0,0},          //esc
    {MOD_SHIFT,0,0},    //shift
    {0,0,0},            //unused
    {0,0,0},            //unused
};

Thumby::Thumby()
    : TCA8418KeyboardBase(THUMBY_ROWS, THUMBY_COLS),
    last_key(UINT8_MAX),
    shift_pending(false),
    alt_pending(false)
{
}

void Thumby::reset()
{
    TCA8418KeyboardBase::reset();
}

void Thumby::pressed(uint8_t key)
{
    if (state == Init || state == Busy) {
        return;
    }
    int row = (key - 1) / THUMBY_COLS;
    int col = (key - 1) % THUMBY_COLS;

    if (row >= THUMBY_ROWS || col >= THUMBY_COLS) {
        return; // invalid key
    }
    uint8_t k = row * THUMBY_COLS + col;
    uint8_t base = ThumbyKeyMap[k][0];
    if (base == MOD_SHIFT) {    //shift layer
        shift_pending = true;   
        alt_pending = false;             
        return;
    }
    if (base == MOD_ALT) {  //alt layer
        alt_pending = true;       
        shift_pending = false;           
        return;
    }
    // store current key
    last_key = k;
    state = Held;
    
}

void Thumby::released()
{
    if (state != Held) {
        return;
    }

    uint8_t k = last_key;
    uint8_t output;

    if (alt_pending) {
        output = ThumbyKeyMap[k][2];
        alt_pending = false;   
    }
    else if (shift_pending) {
        output = ThumbyKeyMap[k][1];
        shift_pending = false; 
    }
    else {
        output = ThumbyKeyMap[k][0];
    }

    if (output != 0) {
    queueEvent(output);
    }

    state = Idle;
}