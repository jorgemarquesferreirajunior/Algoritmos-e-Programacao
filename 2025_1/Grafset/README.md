# ☕ Projeto: Máquina de Café Automatizada com CLP

Este projeto simula o controle de uma máquina de café utilizando um Controlador Lógico Programável (CLP) Logo Siemens. O sistema permite a seleção de três receitas diferentes por meio de botões e realiza o controle de tempo dos atuadores envolvidos no processo.

---

## 🔌 Mapeamento de Entradas e Saídas do CLP

### 📥 Entradas Digitais

| IOs CLP | Elemento | Descrição         |
|--------|----------|-------------------|
| I1     | B1       | Botão Início      |
| I2     | B2       | Botão Receita 1   |
| I3     | B3       | Botão Receita 2   |
| I4     | B4       | Botão Receita 3   |
| I5     | Reserva  | -                 |

### 📤 Saídas Digitais

| IOs CLP | Elemento | Descrição                    |
|--------|----------|------------------------------|
| Q1     | L1       | Lâmpada Indicadora Status On/Off |
| Q2     | L2       | Moedor                       |
| Q3     | L3       | Bomba 1                      |
| Q4     | L4       | Bomba 2                      |

---

## ☕ Funcionamento da Máquina de Café

### ▶️ Modo de Espera
- Ao pressionar **B1**, a **Lâmpada de Status (L1)** acende, indicando que a máquina entrou em **modo Aguardando**.

### 🧃 Receita 1 - Café Simples
- Pressione **B2**:
  - O **Moedor (L2)** liga por **3 segundos**.
  - Em seguida, a **Bomba 1 (L3)** adiciona café por **3 segundos**.
  - Por fim, a **Lâmpada de Status (L1)** se apaga.

### 🧃 Receita 2 - Café Forte
- Pressione **B3**:
  - O **Moedor (L2)** liga por **5 segundos**.
  - Em seguida, a **Bomba 1 (L3)** adiciona café por **3 segundos**.
  - Por fim, a **Lâmpada de Status (L1)** se apaga.

### 🧃 Receita 3 - Café com Açúcar
- Pressione **B4**:
  - O **Moedor (L2)** liga por **1 segundo**.
  - Em seguida, a **Bomba 1 (L3)** adiciona café por **4 segundos**.
  - Após isso, a **Bomba 2 (L4)** adiciona açúcar por **2 segundos**.
  - Por fim, a **Lâmpada de Status (L1)** se apaga.

---

## 🛠️ Tecnologias Utilizadas

- CLP (Controlador Lógico Programável)
- Linguagem Grafset 
- Simulação em software de automação CADSIMU

---