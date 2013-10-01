//
//  HelloWorldScene.h
//  mybox2dtest
//
//  Created by apple on 13-7-4.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#ifndef __HELLO_WORLD_H__
#define __HELLO_WORLD_H__

// When you import this file, you import all the cocos2d classes
#include "cocos2d.h"
#include "Box2D.h"
#include "myObj.h"
#include "mouse.h"
#include "cocos-ext.h"
#include "myContactListener.h"
#include "inputSignal.h"
#include "player.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::CCLayer {
    
public:
    bool initDone;
    b2World* world;
    //    cocos2d::CCTexture2D* m_pSpriteTexture; // weak ref
    //----
    Cplayer*pPlayer;
    CmyContactListener myContactListener;
    CinputSignal inputSignal;
    float m_waterSurfaceHeight;

    ~HelloWorld();
    HelloWorld();
    
    // returns a Scene that contains the HelloWorld as the only child
    static cocos2d::CCScene* scene();
    


    virtual void draw();//重载CCLayer的draw函数
    void update(float dt);
    //----信息模板
    CmyObjInfo objInfo_shotBall_right;
    CmyObjInfo objInfo_shotBall_left;
    //----control
 //   virtual void ccTouchesBegan(cocos2d::CCSet* touches , cocos2d::CCEvent* event);
 //   virtual void ccTouchesMoved(cocos2d::CCSet* touches , cocos2d::CCEvent* event);
 //   virtual void ccTouchesEnded(cocos2d::CCSet* touches , cocos2d::CCEvent* event);
    void rightKeyDown(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
    void rightKeyUpInside(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
    void rightKeyDragExit(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
    void rightKeyDragEnter(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
    //
    void leftKeyDown(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
    void leftKeyUpInside(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
    void leftKeyDragExit(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
    void leftKeyDragEnter(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
    //
    void jumpKeyDown(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
    //
    void shotKeyDown(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
  };

#endif // __HELLO_WORLD_H__
