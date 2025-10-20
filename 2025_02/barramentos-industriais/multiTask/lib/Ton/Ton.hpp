#ifndef TON_HPP
#define TON_HPP

#include <Arduino.h>

class TON {
private:
    unsigned long startTime;
    unsigned long preset;
    bool enabled;
    bool timerDone;
    bool prevIn;

public:
    // Construtor
    TON(unsigned long presetTime = 1000) {
        preset = presetTime;
        startTime = 0;
        enabled = false;
        timerDone = false;
        prevIn = false;
    }

    // Método principal para atualizar o timer
    bool update(bool input) {
        // Detecção de borda de subida
        if (input && !prevIn) {
            startTime = millis();
            enabled = true;
            timerDone = false;
        }
        
        // Se input está ativo e timer habilitado
        if (input && enabled) {
            unsigned long currentTime = millis();
            if (currentTime - startTime >= preset) {
                timerDone = true;
                enabled = false; // Timer completa uma vez até reset
            }
        }
        
        // Reset quando input vai para LOW
        if (!input) {
            enabled = false;
            timerDone = false;
        }
        
        prevIn = input;
        return timerDone;
    }

    // Retorna se o timer completou
    bool done() const {
        return timerDone;
    }

    // Retorna o tempo decorrido desde o início
    unsigned long elapsedTime() const {
        if (!enabled) return 0;
        return millis() - startTime;
    }

    // Retorna o tempo restante
    unsigned long remainingTime() const {
        if (!enabled || timerDone) return 0;
        unsigned long elapsed = elapsedTime();
        return (elapsed >= preset) ? 0 : (preset - elapsed);
    }

    // Reset manual do timer
    void reset() {
        enabled = false;
        timerDone = false;
        startTime = 0;
    }

    // Configurar novo tempo de preset
    void setPreset(unsigned long newPreset) {
        preset = newPreset;
    }

    // Verificar se o timer está ativo
    bool isEnabled() const {
        return enabled;
    }
};

#endif // TON_HPP
