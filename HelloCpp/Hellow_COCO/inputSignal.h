//
//  inputSignal.h
//  mybox2dtest
//
//  Created by apple on 13-7-19.
//
//

#ifndef __mybox2dtest__inputSignal__
#define __mybox2dtest__inputSignal__

#include <iostream>
class CinputSignal{
public:
    bool rightKeyDown;
    bool leftKeyDown;
    CinputSignal(){
        rightKeyDown=false;
        leftKeyDown=false;
    }
};
#endif /* defined(__mybox2dtest__inputSignal__) */
