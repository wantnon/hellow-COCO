//
//  myContactListener.h
//  mybox2dtest
//
//  Created by apple on 13-7-19.
//
//

#ifndef __mybox2dtest__myContactListener__
#define __mybox2dtest__myContactListener__

#include <iostream>
using namespace std;
#include "Box2D.h"
#include "myObj.h"
#include "player.h"
#include "myDef.h"
#include "shotBall.h"

class CmyContactListener:public b2ContactListener
{
public:
    void BeginContact(b2Contact* contact){
     
        b2Fixture*fixtureA=contact->GetFixtureA();
        b2Fixture*fixtureB=contact->GetFixtureB();
        b2Body*bodyA=fixtureA->GetBody();
        b2Body*bodyB=fixtureB->GetBody();
        CmyObj*pObjA=(CmyObj*)bodyA->GetUserData();
        CmyObj*pObjB=(CmyObj*)bodyB->GetUserData();
   /*       //����Ƿ���������player��sensor��ײ
       if(pObjA->getTag()==tag_player &&*(int*)(fixtureA->GetUserData())==partID_foot){
            pObjA->update_touchSolid(true);
        }
        if(pObjB->getTag()==tag_player&&*(int*)(fixtureB->GetUserData())==partID_foot){
            pObjB->update_touchSolid(true);
        }*/
        //��������ײ���ӵ����������Ӱ�죨���ؼ�ֵ����Ϊ����ֵ�������ʵ���С��
        if(pObjA->getTag()==tag_shotBall_right||pObjA->getTag()==tag_shotBall_left){
            CshotBall*pShotBall=(CshotBall*)pObjA;
            pShotBall->isAfterHit=true;
            pShotBall->pb2Body->SetGravityScale(0.7);
        }
        if(pObjB->getTag()==tag_shotBall_right||pObjB->getTag()==tag_shotBall_left){
            CshotBall*pShotBall=(CshotBall*)pObjB;
            pShotBall->isAfterHit=true;
            pShotBall->pb2Body->SetGravityScale(0.7);
        }
        
     /*   //����Ƿ���������player������ײ
        if(pObjA->type==myObjType_player||pObjB->type==myObjType_player){//pObjA��pObjB����player
            Cplayer*pPlayer=(Cplayer*)(pObjA->type==myObjType_player?pObjA:pObjB);
            //��player��������
            b2Vec2 backVec;//��������
            b2WorldManifold worldManifold;
            contact->GetWorldManifold(&worldManifold);
            b2Vec2 normal=worldManifold.normal;//��objAָ��objB
            if(pObjA->type==myObjType_player){//���objA��player
                backVec=-normal;
            }else{//���objB��player
                backVec=normal;
            }//�õ�backVec
            //��backVec������ײЧ��
            float m=pPlayer->pb2Body->GetMass();
            b2Vec2 dv=b2Vec2(0,10);
            b2Vec2 impluse=m*dv;
            pPlayer->pb2Body->ApplyLinearImpulse(impluse, pPlayer->pb2Body->GetWorldCenter());
        }*/
    }
    void EndContact(b2Contact* contact){
 /*       //����Ƿ���������player��sensor�����ײ
        b2Fixture*fixtureA=contact->GetFixtureA();
        b2Fixture*fixtureB=contact->GetFixtureB();
        b2Body*bodyA=fixtureA->GetBody();
        b2Body*bodyB=fixtureB->GetBody();
        CmyObj*pObjA=(CmyObj*)bodyA->GetUserData();
        CmyObj*pObjB=(CmyObj*)bodyB->GetUserData();
        if(pObjA->getTag()==tag_player&&*(int*)(fixtureA->GetUserData())==partID_foot){
            pObjA->update_touchSolid(false);
            cout<<"leave touch"<<endl;
        }
        if(pObjB->getTag()==tag_player&&*(int*)(fixtureB->GetUserData())==partID_foot){
            pObjB->update_touchSolid(false);
            cout<<"leave touch"<<endl;
        }
        */
    }

};
#endif /* defined(__mybox2dtest__myContactListener__) */
