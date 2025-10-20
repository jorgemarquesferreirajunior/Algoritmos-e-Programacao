#ifndef DCMOTOR_HPP
#define DCMOTOR_HPP
#include "esp32-hal-gpio.h"
#include <Arduino.h>

class DCMotor {  
private:
  int pinDir1, pinDir2, pinPWM;
  int currentSpeed = 0;
  int targetSpeed = 0;
  int maxSpeed = 255;
  unsigned long lastRampTime = 0;
  unsigned long rampInterval = 50; // ms entre cada passo da rampa
  int rampStep = 5; // incremento/decremento de velocidade por passo
  bool isRamping = false;
  
  enum MotorDirection {
    STOPPED,
    FORWARD,
    BACKWARD
  };
  
  MotorDirection currentDirection = STOPPED;

public:  
  // Construtor - configura os 3 pinos diretamente
  DCMotor(int dir1, int dir2, int pwmPin, int maxSpd = 255) {
    pinDir1 = dir1;
    pinDir2 = dir2;
    pinPWM = pwmPin;
    maxSpeed = maxSpd;

    pinMode(pinDir1,OUTPUT);
    pinMode(pinDir2,OUTPUT);
    pinMode(pinPWM,OUTPUT);

    Stop(); // Inicia com motor parado
  }
  
  // Define a velocidade máxima do motor
  void setMaxSpeed(int maxSpd) {
    maxSpeed = constrain(maxSpd, 0, 255);
  }
  
  // Configura parâmetros da rampa
  void setRampParameters(unsigned long interval, int step) {
    rampInterval = interval;
    rampStep = step;
  }
  
  // Move o motor para frente com velocidade específica (com rampa)
  void Forward(int speed = -1) {
    if (speed >= 0) {
      targetSpeed = constrain(speed, 0, maxSpeed);
    } else {
      targetSpeed = maxSpeed;
    }
    currentDirection = FORWARD;
    isRamping = true;
  }
  
  // Move o motor para trás com velocidade específica (com rampa)
  void Backward(int speed = -1) {
    if (speed >= 0) {
      targetSpeed = constrain(speed, 0, maxSpeed);
    } else {
      targetSpeed = maxSpeed;
    }
    currentDirection = BACKWARD;
    isRamping = true;
  }
  
  // Para o motor imediatamente
  void Stop() {
    targetSpeed = 0;
    currentSpeed = 0;
    currentDirection = STOPPED;
    isRamping = false;
    applySpeed(); // Aplica velocidade zero
  }
  
  // Para o motor suavemente com rampa de desaceleração
  void StopWithRamp() {
    targetSpeed = 0;
    currentDirection = STOPPED;
    isRamping = true;
  }
  
  // Define velocidade alvo para rampa suave
  void setSpeed(int speed) {
    targetSpeed = constrain(speed, 0, maxSpeed);
    isRamping = true;
  }
  
  // Define direção sem alterar velocidade
  void setDirection(bool forward) {
    if (forward) {
      currentDirection = FORWARD;
    } else {
      currentDirection = BACKWARD;
    }
    applySpeed(); // Re-aplica a velocidade atual com nova direção
  }
  
  // Atualiza a rampa - deve ser chamado frequentemente no loop()
  void update() {
    if (!isRamping || millis() - lastRampTime < rampInterval) {
      return;
    }
    
    lastRampTime = millis();
    
    // Executa rampa de aceleração/desaceleração
    if (currentSpeed < targetSpeed) {
      currentSpeed += rampStep;
      if (currentSpeed > targetSpeed) currentSpeed = targetSpeed;
    } else if (currentSpeed > targetSpeed) {
      currentSpeed -= rampStep;
      if (currentSpeed < targetSpeed) currentSpeed = targetSpeed;
    } else {
      isRamping = false;
      return;
    }
    
    // Aplica a velocidade atualizada ao motor
    applySpeed();
  }
  
  // Retorna se está em processo de rampa
  bool isRampingInProgress() {
    return isRamping;
  }
  
  // Retorna velocidade atual
  int getCurrentSpeed() {
    return map(analogRead(pinPWM), 0, 4095, 0, 255);
  }
  
  // Retorna velocidade alvo
  int getTargetSpeed() {
    return targetSpeed;
  }
  
  // Retorna direção atual
  MotorDirection getCurrentDirection() {
    return currentDirection;
  }

  // Retorna estado do motor
  bool isWorking() {
    return getCurrentDirection() != STOPPED ? true : false;
  }

  // retorna estado da direcao 1
  bool getDirection1() {
    return digitalRead(pinDir1);
  }
  // retorna estado da direcao 2
  bool getDirection2() {
    return digitalRead(pinDir2);
  }

private:
  // Aplica a velocidade atual aos pinos do motor
  void applySpeed() {
    switch (currentDirection) {
      case FORWARD:
        digitalWrite(pinDir1, HIGH);
        digitalWrite(pinDir2, LOW);
        analogWrite(pinPWM, currentSpeed);
        break;
      case BACKWARD:
        digitalWrite(pinDir1, LOW);
        digitalWrite(pinDir2, HIGH);
        analogWrite(pinPWM, currentSpeed);
        break;
      case STOPPED:
        digitalWrite(pinDir1, LOW);
        digitalWrite(pinDir2, LOW);
        analogWrite(pinPWM, 0);
        break;
    }
  }
};

#endif // DCMOTOR_HPP
