#ifndef BTN_HPP
#define BTN_HPP

class Btn
{
  public:
    int pino;
    bool btn_liberado, btn_clicado;
    bool ativo_alto; // true para ativo alto, false para ativo baixo
    
    Btn(int p, bool alto = true) // Por padrão é ativo alto
    {
      pino = p;
      ativo_alto = alto;
      btn_liberado = btn_clicado = 0;
      // pinMode(pino, ativo_alto ? INPUT_PULLDOWN : INPUT_PULLUP);
    }
    
    bool press()
    {
      bool estado = digitalRead(pino);
      
      // Inverte a lógica se for ativo baixo
      if (ativo_alto) {
        if(estado) {
          btn_liberado = 0;
          btn_clicado = 1;
        } else {
          btn_liberado = 1;
        }
      } else {
        if(!estado) { // Ativo baixo: pressionado quando LOW
          btn_liberado = 0;
          btn_clicado = 1;
        } else {
          btn_liberado = 1;
        }
      }
      
      if(btn_clicado && btn_liberado)
      {
        btn_liberado = btn_clicado = 0;
        return true;
      } else {
        return false;
      }
    }
};

#endif //BTN_HPP
