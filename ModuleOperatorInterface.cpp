#include <Arduino.h>
#include <Button.h>
#include <ModuleOperatorInterface.h>
  
tModuleOperatorInterface::tModuleOperatorInterface(tModuleOperatorInterfaceClient **modeHandlers, unsigned long revertInterval) {
  this->modeHandlers = modeHandlers;
  this->revertInterval = revertInterval;
  this->currentMode = 0;
  this->currentAddress = -1;
  this->buttonPressedAt = 0UL;
}

int tModuleOperatorInterface::getCurrentMode() {
  return(this->currentMode);
}

unsigned long tModuleOperatorInterface::getButtonPressedAt() {
  return(this->buttonPressedAt);
}

void tModuleOperatorInterface::revertModeMaybe() {
  if (millis() > (this->buttonPressedAt + this->revertInterval)) {
    this->currentMode = 0;
    this->currentAddress = -1;
  }
}
    
tModuleOperatorInterface::EventOutcome tModuleOperatorInterface::handleButtonEvent(bool buttonState, unsigned char value) {
  EventOutcome retval = MODE_CHANGE;
  unsigned long now = millis();

  if (buttonState == Button::PRESSED) {
    this->buttonPressedAt = now;
  } else {
    if ((this->buttonPressedAt) && (now < (this->buttonPressedAt + 1000))) {
      if (this->currentAddress != -1) {
        retval = (this->modeHandlers[this->currentMode]->processValue((unsigned char) this->currentAddress, value))?VALUE_ACCEPTED:VALUE_REJECTED;
        this->currentAddress = -1;
      } else {
        this->currentMode++;
        if (this->modeHandlers[this->currentMode] == 0) this->currentMode = 0;
        retval = MODE_CHANGE;
      }
    } else {
      if (this->modeHandlers[this->currentMode]->validateAddress(value)) {
        this->currentAddress = value;
        retval = ADDRESS_ACCEPTED;
      } else {
        this->currentAddress = -1;
        retval = ADDRESS_REJECTED;
      }
    }
  }
  return(retval);
}
