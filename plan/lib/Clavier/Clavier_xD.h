#ifndef __CLAVIER_xD_H__
#define __CLAVIER_xD_H__

#include "Clavier.h"
#include <broches.h>
#include <Arduino.h>


typedef volatile uint8_t PortReg;
typedef uint8_t PortMask;

class ToucheDigit{
public:
  ToucheDigit(unsigned char broche, unsigned char touche);
  //ToucheDigit();
  ~ToucheDigit();
  void begin() const;
  bool active() const;
  unsigned char toucheDigit() const;
protected:
  unsigned char broche;
  unsigned char touche;
  volatile PortReg *portIn;
  PortMask pinmask;
  friend class Clavier_xD;
};

class Clavier_xD:public Clavier{
public:
  Clavier_xD(const ToucheDigit* touches, unsigned char nbTouche, FonctionUpdate _fctUpdate);
  Clavier_xD(const ToucheDigit* touches, unsigned char nbTouche);
  virtual void begin();
  virtual uint8_t getToucheAct(bool valDerTouche = true);
  virtual bool aucuneTouche(bool attend = true);
protected:
  const ToucheDigit* touches;
  unsigned char nbTouche;
};


#endif
