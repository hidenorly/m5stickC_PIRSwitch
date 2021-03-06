/* 
 Copyright (C) 2019 hidenorly

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include "base.h"
#include "PowerControlPoller.h"

PowerControlPoller::PowerControlPoller(PowerControl* pPowerControl, GpioDetector* pHumanDetector, int humanTimeout, int dutyMSec):LooperThreadTicker(NULL, NULL, dutyMSec),mpPowerControl(pPowerControl),mpHumanDetector(pHumanDetector),mHumanTimeout(humanTimeout)
{

}

PowerControlPoller::~PowerControlPoller()
{
  mpPowerControl = NULL;
  mpHumanDetector = NULL;
}

void PowerControlPoller::doCallback(void)
{
  if(mpHumanDetector && mpPowerControl){
    mpHumanDetector->update();

    static bool lastPowerStatus = mpPowerControl->getPowerStatus();
    static bool decidedPower = false;
    static unsigned long lastHumanDetected = 0;

    bool curHumanStatus = mpHumanDetector->getStatus();

    DEBUG_PRINT("Human ");
    DEBUG_PRINTLN(curHumanStatus ? "detected" : "not detected");

    unsigned long n = millis();

    if( curHumanStatus ) {
      // human detected
      lastHumanDetected = n;
      decidedPower = true; // true because human detected)
    } else {
      // currently human not detected!
      if( (n - lastHumanDetected) > mHumanTimeout ){
        DEBUG_PRINTLN("Human is absent for a while then turn off");
        decidedPower = false; // false because human not detected
      }
    }

    if(lastPowerStatus!=decidedPower){
      mpPowerControl->setPower(decidedPower);
      lastPowerStatus = mpPowerControl->getPowerStatus();
    }
  }
}
