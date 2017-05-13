#ifndef __IO_SH1106_xD_H__
#define __IO_SH1106_xD_H__

#include <Clavier_xD.h>
#include <Adafruit_SH1106.h>

class IO_SH1106_xD: virtual public Adafruit_SH1106, public Clavier_xD{
public:
  IO_SH1106_xD(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, const ToucheDigit* touches, unsigned char nbTouche, FonctionUpdate _fctUpdate);
  IO_SH1106_xD(int8_t DC, int8_t RST, int8_t CS, const ToucheDigit* touches, unsigned char nbTouche, FonctionUpdate _fctUpdate);
  IO_SH1106_xD(int8_t RST, const ToucheDigit* touches, unsigned char nbTouche, FonctionUpdate _fctUpdate);

  IO_SH1106_xD(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, const ToucheDigit* touches, unsigned char nbTouche);
  IO_SH1106_xD(int8_t DC, int8_t RST, int8_t CS, const ToucheDigit* touches, unsigned char nbTouche);
  IO_SH1106_xD(int8_t RST, const ToucheDigit* touches, unsigned char nbTouche);

  void begin(uint8_t switchvcc = SH1106_SWITCHCAPVCC, uint8_t i2caddr = SH1106_I2C_ADDRESS, bool reset=true);

  // change la fonction d'update du clavier
  void setUpdateFct(FonctionUpdate _fctUpdate);

  void onOffEcran(bool etat); // réécriture de la fonction d'allumage/extinction de l'écran
  bool etatEcran(void);
protected:
  bool ecranActif;

};


#endif
