//
//  shotBall.h
//  mybox2dtest
//
//  Created by apple on 13-7-24.
//
//

#ifndef __mybox2dtest__shotBall__
#define __mybox2dtest__shotBall__

#include <iostream>

using namespace std;
#include "cocos2d.h"
#include "Box2D.h"
using namespace cocos2d;
#include "myObj.h"
class CshotBall:public CmyObj
{
public:
    bool isAfterHit;//�Ƿ��Ѿ������巢������ײ
    CshotBall(b2World*world):CmyObj(world){
        isAfterHit=false;
    }
    static CshotBall * create(b2World*world)
    {
        CshotBall * pRet = new CshotBall(world);
        if (pRet && pRet->init())
        {
            pRet->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(pRet);
        }
        return pRet;
    }

    void removeMeFromSceneAndDestroyBodyCallBack(/*CCTime*/float dt){
        removeMeFromSceneAndDestroyBody(true);
    }
    
};
#endif /* defined(__mybox2dtest__shotBall__) */
