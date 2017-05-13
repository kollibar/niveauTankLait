#include "IO_SH1106_xD.h"

IO_SH1106_xD::IO_SH1106_xD(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, const ToucheDigit* touches, unsigned char nbTouche, FonctionUpdate _fctUpdate)
  : Adafruit_SH1106(SID, SCLK, DC, RST, CS), Clavier_xD(touches,nbTouche,_fctUpdate){
  return;
}
IO_SH1106_xD::IO_SH1106_xD(int8_t DC, int8_t RST, int8_t CS, const ToucheDigit* touches, unsigned char nbTouche, FonctionUpdate _fctUpdate)
  : Adafruit_SH1106( DC, RST, CS), Clavier_xD(touches,nbTouche,_fctUpdate){
  return;
}
IO_SH1106_xD::IO_SH1106_xD(int8_t RST, const ToucheDigit* touches, unsigned char nbTouche, FonctionUpdate _fctUpdate)
: Adafruit_SH1106(RST), Clavier_xD(touches,nbTouche,_fctUpdate){
  return;
}

IO_SH1106_xD::IO_SH1106_xD(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, const ToucheDigit* touches, unsigned char nbTouche)
  : Adafruit_SH1106(SID, SCLK, DC, RST, CS), Clavier_xD(touches,nbTouche){
  return;
}
IO_SH1106_xD::IO_SH1106_xD(int8_t DC, int8_t RST, int8_t CS, const ToucheDigit* touches, unsigned char nbTouche)
  : Adafruit_SH1106( DC, RST, CS), Clavier_xD(touches,nbTouche){
  return;
}
IO_SH1106_xD::IO_SH1106_xD(int8_t RST, const ToucheDigit* touches, unsigned char nbTouche)
: Adafruit_SH1106(RST), Clavier_xD(touches,nbTouche){
  return;
}

void IO_SH1106_xD::begin(uint8_t switchvcc, uint8_t i2caddr, bool reset){
  Adafruit_SH1106::begin(switchvcc, i2caddr, reset);
  Clavier_xD::begin();
  ecranActif=true;
}

bool IO_SH1106_xD::etatEcran(){
  return ecranActif;
}

void IO_SH1106_xD::onOffEcran(bool etat){
  if(etat==ecranActif) return;

  if( etat ){ // on allume l'écran

  } else { // on éteint l'écran

  }

  ecranActif=etat;
}

void IO_SH1106_xD::setUpdateFct(FonctionUpdate _fctUpdate){
  fctUpdate=_fctUpdate;
}
