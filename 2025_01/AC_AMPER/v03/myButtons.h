class Btn {
  public:
    int pino;
    bool btn_liberado, btn_clicado;
    bool borda_descida;  // true = borda de descida, false = subida

    Btn(int p, bool usar_borda_descida = true) {
      pino = p;
      borda_descida = usar_borda_descida;
      btn_liberado = btn_clicado = 0;
    }

    bool pressed() {
      bool leitura = digitalRead(pino);

      // Inverte lógica conforme borda desejada
      if (borda_descida) {
        leitura = !leitura; // nível baixo representa clique
      }

      if (leitura) {
        btn_liberado = false;
        btn_clicado = true;
      } else {
        btn_liberado = true;
      }

      if (btn_clicado && btn_liberado) {
        btn_clicado = btn_liberado = false;
        return true;
      } else {
        return false;
      }
    }
};
