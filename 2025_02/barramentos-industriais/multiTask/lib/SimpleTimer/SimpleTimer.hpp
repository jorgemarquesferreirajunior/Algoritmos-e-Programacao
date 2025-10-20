#ifndef SIMPLETIMER_HPP
#define SIMPLETIMER_HPP

#include <Arduino.h>

class SimpleTimer {
private:
    unsigned long startTime;
    unsigned long duration;
    bool running;
    bool oneShot;
    const char* name; // Para debug

public:
    // Construtor
    SimpleTimer(unsigned long timeoutMs = 1000, bool oneShotMode = true, const char* timerName = "Timer") {
        duration = timeoutMs;
        oneShot = oneShotMode;
        name = timerName;
        reset();
    }

    // Inicia/Reinicia o timer
    void start() {
        startTime = millis();
        running = true;
    }

    // Reinicia completamente
    void reset() {
        startTime = millis();
        running = false;
    }

    // Para o timer
    void stop() {
        running = false;
    }

    // Verifica se o timer completou
    bool done() {
        if (!running) return false;
        
        if (millis() - startTime >= duration) {
            if (oneShot) {
                running = false; // Para após completar (modo one-shot)
            } else {
                startTime = millis(); // Reinicia automaticamente (modo periódico)
            }
            return true;
        }
        return false;
    }

    // Verifica se está rodando
    bool isRunning() {
        return running;
    }

    // Tempo decorrido
    unsigned long elapsed() {
        if (!running) return 0;
        return millis() - startTime;
    }

    // Tempo restante
    unsigned long remaining() {
        if (!running) return 0;
        unsigned long elapsedTime = elapsed();
        return (elapsedTime >= duration) ? 0 : (duration - elapsedTime);
    }

    // Configura novo timeout
    void setTimeout(unsigned long newDuration) {
        duration = newDuration;
    }

    // Método combinado: start + check
    bool update(bool condition) {
        if (condition && !running) {
            start(); // Inicia quando condição se torna verdadeira
        }
        
        if (!condition) {
            running = false; // Para quando condição se torna falsa
        }
        
        return done();
    }
};

#endif // SIMPLETIMER_HPP
