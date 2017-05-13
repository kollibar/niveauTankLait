#include "Clavier_xD.h"

/*ToucheDigit::ToucheDigit(){
  this->broche=0;
}*/

ToucheDigit::ToucheDigit(unsigned char broche, unsigned char touche){
  this->broche=broche;
  this->touche=touche;
  this->portIn=portInputRegister(digitalPinToPort(broche));
  this->pinmask=digitalPinToBitMask(broche);
}

void ToucheDigit::begin() const{
  pinMode(this->broche, INPUT_PULLUP);
}

bool ToucheDigit::active() const{
  //return (! digitalRead(this->broche));
  return ((*(this->portIn) & ( this->pinmask)) == 0 );
}

unsigned char ToucheDigit::toucheDigit() const{
  if((*this->portIn & ( this->pinmask)) == 0 ) return this->touche;
  else return 0;
}

ToucheDigit::~ToucheDigit(){
  pinMode(this->broche,INPUT);
}

Clavier_xD::Clavier_xD(const ToucheDigit* touches, unsigned char nbTouche, FonctionUpdate _fctUpdate): Clavier(_fctUpdate){
  this->touches=touches;
  this->nbTouche=nbTouche;
}

Clavier_xD::Clavier_xD(const ToucheDigit* touches, unsigned char nbTouche): Clavier(){
  this->touches=touches;
  this->nbTouche=nbTouche;
}


void Clavier_xD::begin(){
  for(unsigned char i=0;i<this->nbTouche;++i){
    (this->touches+i)->begin();
  }
}

uint8_t Clavier_xD::getToucheAct(bool valDerTouche) {
  // renvoi la valeur de la touche actuellement pressé
  uint8_t touche;

  onOffEcran(true);

  touche = 0;

  for(unsigned char i=0;i<this->nbTouche;++i){
    touche = touche | ((this->touches+i)->toucheDigit());
  }

  if (valDerTouche && touche!=0) {
    dateDerniereTouche = millis();
    derniereTouche = touche;
  }

  return touche;
}

bool Clavier_xD::aucuneTouche(bool attend) {
  uint8_t touche;
  if (this->getToucheAct(true) == 0 ) return true;

  if (attend) {
    do {
      touche = getToucheAct(false);
      /*if (touche == derniereTouche && (millis() - dateDerniereTouche) > delaiTouche) {
        delaiTouche = DELAI_REDUIT;
        return true;
      }
      if (touche != derniereTouche) delaiTouche = DELAI_STANDARD;*/
    } while (touche != 0);
    return true;
  }

  return false;
}
