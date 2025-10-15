#ifndef MOTOR_CC_HPP
#define MOTOR_CC_HPP

#include <Arduino.h>
#include <driver/ledc.h>

class MotorCC {
private:
    uint8_t pinPWM;
    uint8_t pinEnable;
    uint8_t pinDirection1; // Opcional: para controle de direção H-bridge
    uint8_t pinDirection2; // Opcional: para controle de direção H-bridge
    
    int currentSpeed;
    int targetSpeed;
    bool enabled;
    bool isRamping;
    unsigned long rampStartTime;
    unsigned long rampDuration;
    int rampStartSpeed;
    
    // Configuração PWM
    ledc_channel_t pwmChannel;
    uint32_t pwmFrequency;
    uint8_t pwmResolution;
    uint16_t pwmMaxDuty;
    
    // Direção do motor
    bool clockwise;

public:
    // Construtor simples (apenas PWM e Enable)
    MotorCC(uint8_t pwmPin, uint8_t enablePin, 
            ledc_channel_t channel = LEDC_CHANNEL_0, 
            uint32_t freq = 5000, uint8_t resolution = 8) {
        
        pinPWM = pwmPin;
        pinEnable = enablePin;
        pinDirection1 = 255; // Indica não usado
        pinDirection2 = 255; // Indica não usado
        
        pwmChannel = channel;
        pwmFrequency = freq;
        pwmResolution = resolution;
        pwmMaxDuty = (1 << resolution) - 1;
        
        currentSpeed = 0;
        targetSpeed = 0;
        enabled = false;
        isRamping = false;
        clockwise = true;
        
        init();
    }

    // Construtor com controle de direção (H-bridge)
    MotorCC(uint8_t pwmPin, uint8_t enablePin, uint8_t dir1Pin, uint8_t dir2Pin,
            ledc_channel_t channel = LEDC_CHANNEL_0, 
            uint32_t freq = 5000, uint8_t resolution = 8) {
        
        pinPWM = pwmPin;
        pinEnable = enablePin;
        pinDirection1 = dir1Pin;
        pinDirection2 = dir2Pin;
        
        pwmChannel = channel;
        pwmFrequency = freq;
        pwmResolution = resolution;
        pwmMaxDuty = (1 << resolution) - 1;
        
        currentSpeed = 0;
        targetSpeed = 0;
        enabled = false;
        isRamping = false;
        clockwise = true;
        
        init();
    }

private:
    void init() {
        // Configura os pinos
        pinMode(pinEnable, OUTPUT);
        digitalWrite(pinEnable, LOW);
        
        pinMode(pinPWM, OUTPUT);
        digitalWrite(pinPWM, LOW);
        
        // Configura pinos de direção se existirem
        if (pinDirection1 != 255) {
            pinMode(pinDirection1, OUTPUT);
            digitalWrite(pinDirection1, LOW);
        }
        if (pinDirection2 != 255) {
            pinMode(pinDirection2, OUTPUT);
            digitalWrite(pinDirection2, LOW);
        }
        
        // Configura PWM do ESP32
        ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
        ledcAttachPin(pinPWM, pwmChannel);
        ledcWrite(pwmChannel, 0);
    }

    void setDirection(bool clockwise) {
        this->clockwise = clockwise;
        
        if (pinDirection1 != 255 && pinDirection2 != 255) {
            // Controle H-bridge
            if (clockwise) {
                digitalWrite(pinDirection1, HIGH);
                digitalWrite(pinDirection2, LOW);
            } else {
                digitalWrite(pinDirection1, LOW);
                digitalWrite(pinDirection2, HIGH);
            }
        }
        // Se não tem pinos de direção, assume que a direção é controlada
        // pelo sinal PWM (positivo/negativo em alguns drivers)
    }

public:
    // Habilita/desabilita o motor
    void setEnable(bool state) {
        enabled = state;
        digitalWrite(pinEnable, state ? HIGH : LOW);
        
        if (!state) {
            // Se desabilitado, para o motor
            ledcWrite(pwmChannel, 0);
            currentSpeed = 0;
            targetSpeed = 0;
            isRamping = false;
        }
    }

    // Define velocidade direta (0-255 para 8-bit, 0-1023 para 10-bit, etc.)
    void setSpeed(int speed) {
        if (!enabled) return;
        
        // Limita a velocidade dentro dos limites
        speed = constrain(speed, 0, pwmMaxDuty);
        
        // Determina direção baseada no sinal (se suportado)
        bool newDirection = (speed >= 0);
        if (newDirection != clockwise) {
            setDirection(newDirection);
        }
        
        targetSpeed = abs(speed);
        isRamping = false; // Cancela ramping se existir
        currentSpeed = targetSpeed;
        applySpeed();
    }

    // Define velocidade com ramping suave
    void setSpeedRamp(int speed, unsigned long rampTimeMs) {
        if (!enabled) return;
        
        speed = constrain(speed, 0, pwmMaxDuty);
        
        bool newDirection = (speed >= 0);
        if (newDirection != clockwise) {
            setDirection(newDirection);
        }
        
        targetSpeed = abs(speed);
        rampStartSpeed = currentSpeed;
        rampStartTime = millis();
        rampDuration = rampTimeMs;
        isRamping = true;
    }

    // Para o motor imediatamente
    void stop() {
        setSpeed(0);
    }

    // Para o motor com ramping
    void stopRamp(unsigned long rampTimeMs) {
        setSpeedRamp(0, rampTimeMs);
    }

    // Atualiza o motor (deve ser chamado frequentemente para ramping)
    void update() {
        if (!enabled || !isRamping) return;
        
        unsigned long currentTime = millis();
        unsigned long elapsed = currentTime - rampStartTime;
        
        if (elapsed >= rampDuration) {
            // Ramping completo
            currentSpeed = targetSpeed;
            isRamping = false;
        } else {
            // Interpola linearmente
            float progress = (float)elapsed / (float)rampDuration;
            currentSpeed = rampStartSpeed + (targetSpeed - rampStartSpeed) * progress;
        }
        
        applySpeed();
    }

    // Retorna a velocidade atual
    int getCurrentSpeed() const {
        return clockwise ? currentSpeed : -currentSpeed;
    }

    // Retorna a velocidade alvo
    int getTargetSpeed() const {
        return clockwise ? targetSpeed : -targetSpeed;
    }

    // Verifica se está habilitado
    bool isEnabled() const {
        return enabled;
    }

    // Verifica se está em ramping
    bool isRamping() const {
        return isRamping;
    }

    // Retorna a direção atual
    bool isClockwise() const {
        return clockwise;
    }

private:
    void applySpeed() {
        ledcWrite(pwmChannel, currentSpeed);
    }
};

#endif
