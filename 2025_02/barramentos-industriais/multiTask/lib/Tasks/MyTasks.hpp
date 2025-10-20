#ifndef MYTASKS_HPP
#define MYTASKS_HPP


#include <Arduino.h>
#include "Globals.hpp"

// prototipos das tasks
void taskConveyor(void *pvParameters);
void taskModbus(void *pvParameters);


// varaiveis compartilhadas
extern int contador;

#endif //MYTASKS_HPP
