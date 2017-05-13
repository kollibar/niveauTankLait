#ifndef __BROCHAGE_H__
#define __BROCHAGE_H__

#include <Arduino.h>
#include <Clavier_xD.h>

const ToucheDigit touches[]={ToucheDigit(A2,BT_BAS),
                      ToucheDigit(A3,BT_HAUT),
                      ToucheDigit(A1,BT_SELECT)};

const char broche_buzzer = 3;

const char broche_trigger = 8;
//const char broche_trigger = 13;
//const char broche_echo = 12;
const char broche_echo = 9;

const char broche_onewire = 7;

const char broche_sqw = 5;
const char broche_32k = 6;

const ToucheDigit interrupteur=ToucheDigit(A0,1);

const char broche_chauffe = 4;

#endif
