#ifndef BTN_HPP
#define BTN_HPP

class Btn
{
  public:
    int pino;
    bool estado_anterior;
    bool borda_subida;  // true = subida, false = descida

    // Construtor
    Btn(int p, bool borda)
    {
      pino = p;
      borda_subida = borda;
      estado_anterior = digitalRead(pino);
    }

    bool press()
    {
      bool estado_atual = digitalRead(pino);
      bool evento = false;

      // Detecta borda de subida (LOW → HIGH)
      if (borda_subida && (estado_atual == HIGH && estado_anterior == LOW))
      {
        evento = true;
      }

      // Detecta borda de descida (HIGH → LOW)
      else if (!borda_subida && (estado_atual == LOW && estado_anterior == HIGH))
      {
        evento = true;
      }

      estado_anterior = estado_atual;
      return evento;
    }
};

#endif // BTN_HPP
