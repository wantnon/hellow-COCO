//
//  player.h
//  mybox2dtest
//
//  Created by apple on 13-7-21.
//
//

#ifndef __mybox2dtest__player__
#define __mybox2dtest__player__

#include <iostream>
#include "myObj.h"
using namespace std;
#include "cocos2d.h"
#include "Box2D.h"
using namespace cocos2d;
#define player_state_still_right 0
#define player_state_still_left 1
#define player_state_forward_right 2
#define player_state_forward_left 3
#define player_state_jump_still_right 4
#define player_state_jump_still_left 5
#define player_state_jump_forward_right 6
#define player_state_jump_forward_left 7
#define player_state_swim_still_right 8
#define player_state_swim_still_left 9
#define player_state_swim_forward_right 10
#define player_state_swim_forward_left 11


class Cplayer:public CmyObj
{
public:
    CCTexture2D*tex_still_right;
    CCTexture2D*tex_still_left;
    CCTexture2D*tex_jump_still_right;
    CCTexture2D*tex_jump_still_left;
    CCTexture2D*tex_jump_forward_right;
    CCTexture2D*tex_jump_forward_left;
    CCTexture2D*tex_shotBall_left;
    CCTexture2D*tex_shotBall_right;
    CCTexture2D*tex_swim_still_left;
    CCTexture2D*tex_swim_still_right;
    CCTexture2D*tex_forward_left_first;//forward_left��һ֡
    CCTexture2D*tex_forward_right_first;//forward_right��һ֡
    int statef,state;
    b2Fixture*fixture_body;
    b2Fixture*fixture_foot;
    b2Fixture*fixture_foot2;

    Cplayer(b2World*world):CmyObj(world){

        fixture_body=NULL;
        fixture_foot=NULL;
        fixture_foot2=NULL;
        
        state=statef=player_state_jump_still_right;

        tex_still_right=NULL;
        tex_still_left=NULL;
        tex_jump_still_right=NULL;
        tex_jump_still_left=NULL;
        tex_shotBall_left=NULL;
        tex_shotBall_right=NULL;
        tex_swim_still_left=NULL;
        tex_swim_still_right=NULL;
        tex_jump_forward_right=NULL;
        tex_jump_forward_left=NULL;
        tex_forward_left_first=NULL;
        tex_forward_right_first=NULL;
    }

    bool isChange_state(){
        return (state!=statef);
    }
    static Cplayer * create(b2World*world)
    {
        Cplayer * pRet = new Cplayer(world);
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

    void update_state(int newState){
        statef=state;
        state=newState;
    
    }
    void print_sate(){
        switch (state) {
            case player_state_still_right:
                cout<<"still_right";
                break;
            case player_state_still_left:
                cout<<"still_left";
                break;
            case player_state_forward_right:
                cout<<"run_forward_right";
                break;
            case player_state_forward_left:
                cout<<"run_forward_left";
                break;
            case player_state_jump_still_right:
                cout<<"jump_still_right";
                break;
            case player_state_jump_still_left:
                cout<<"jump_still_left";
                break;
            case player_state_jump_forward_right:
                cout<<"jump_foward_right";
                break;
            case player_state_jump_forward_left:
                cout<<"jump_forward_left";
                break;
            case player_state_swim_still_right:
                cout<<"swim_still_right";
                break;
            case player_state_swim_still_left:
                cout<<"swim_still_left";
                break;
            case player_state_swim_forward_right:
                cout<<"swim_forward_right";
                break;
            case player_state_swim_forward_left:
                cout<<"swim_forward_left";
                break;
        }
    }
    bool is_jump_still(){
        return (state==player_state_jump_still_right||state==player_state_jump_still_left);
    }
    bool is_jump_forward(){
        return (state==player_state_jump_forward_right||state==player_state_jump_forward_left);
    }
    bool is_forward(){
        return (state==player_state_forward_left||state==player_state_forward_right);
    }
    bool is_still(){
        return (state==player_state_still_left||state==player_state_still_right);
    }
    bool is_swim_forward(){
        return (state==player_state_swim_forward_right||state==player_state_swim_forward_left);
    }
    bool is_swim_still(){
        return (state==player_state_swim_still_right||state==player_state_swim_still_left);
    }
    bool is_jump(){
        return (is_jump_forward()||is_jump_still());
    }
    bool is_swim(){
        return (is_swim_forward()||is_swim_still());
    }
    bool is_xx_still(){
        return (is_still()||is_jump_still()||is_swim_still());
    }
    bool is_xx_forward(){
        return (is_forward()||is_jump_forward()||is_swim_forward());
    }
    bool isFacingRight()const {
        switch (state) {
            case player_state_still_right:
            case player_state_forward_right:
            case player_state_jump_still_right:
            case player_state_jump_forward_right:
            case player_state_swim_still_right:
            case player_state_swim_forward_right:
                return true;
                break;
            case player_state_still_left:
            case player_state_forward_left:
            case player_state_jump_still_left:
            case player_state_jump_forward_left:
            case player_state_swim_still_left:
            case player_state_swim_forward_left:
                return false;
                break;
                default:
                cout<<"error:δ֪��state!"<<endl;
                exit(0);
                break;
        }
    }
    void create_pb2Body(b2World* world,float rx,float ry,const b2Filter*fliter=NULL){
        // Define the dynamic body.
        //Set up a 1m squared box in the physics world
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.userData=this;
        
        b2Body *body = world->CreateBody(&bodyDef);
        
        // Define another box shape for our dynamic body.
        const float margx=0.3;
        const float margy_up=0.15;
        const float margy_bottom=0.3;
        b2PolygonShape shape_body;
        {
            b2Vec2 verts[8];//��Ҫ����8����Ϊ���������8
            verts[0].Set(-rx/3+margx, ry-margy_up);
            verts[1].Set(-rx+margx, ry*0.6-margy_up);
            verts[2].Set(-rx+margx, -ry/3.5+margy_bottom);
            verts[3].Set(-rx*0.3+margx, -ry+margy_bottom);
            verts[4].Set(rx*0.3-margx, -ry+margy_bottom);
            verts[5].Set(rx-margx, -ry/3.5+margy_bottom);
            verts[6].Set(rx-margx, ry*0.6-margy_up);
            verts[7].Set(rx/3-margx, ry-margy_up);
            shape_body.Set(verts, 8);
        }
        //
        b2PolygonShape shape_foot;
        {
            b2Vec2 verts[4];//��Ҫ����8����Ϊ���������8
            verts[0].Set(-rx+0.5+margx+0.08, -ry*0.4);
            verts[1].Set(-rx*0.3+margx, -ry+margy_bottom);
            verts[2].Set(rx*0.3-margx, -ry+margy_bottom);
            verts[3].Set(rx-0.5-margx-0.08, -ry*0.4);
            shape_foot.Set(verts, 4);
           
        }
        //
        b2PolygonShape shape_foot2;
        {
            shape_foot2.SetAsBox(rx*0.3-margx, ry/6, b2Vec2(0,-(ry-ry/6)+margy_bottom), 0);
        }


        //fixture_body
        {
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape_body;
            fixtureDef.density = 10.0f;
            fixtureDef.friction = 0;
            fixtureDef.isSensor=false;
            if(fliter!=NULL){
                fixtureDef.filter=*fliter;
            }
           fixture_body=body->CreateFixture(&fixtureDef);

        }
     
        //fixture_foot
        {
            b2FixtureDef fixtureDef;
            fixtureDef.shape=&shape_foot;
            fixtureDef.density=0.5f;
            fixtureDef.friction=1;
            fixtureDef.isSensor=false;
            if(fliter!=NULL){
                fixtureDef.filter=*fliter;
            }
            fixture_foot=body->CreateFixture(&fixtureDef);
  
        }
		//fixture_foot2
		//ע�⣬������fixture���������󣬲�����Ч
        {
            b2FixtureDef fixtureDef;
            fixtureDef.shape=&shape_foot2;
            fixtureDef.density=0.0001f;//��������ռ����
            fixtureDef.friction=0;
            fixtureDef.isSensor=true;
            if(fliter!=NULL){
                fixtureDef.filter=*fliter;
            }
            fixture_foot2=body->CreateFixture(&fixtureDef);
       
        }
        //��body����pb2Body
        pb2Body=body;
    }


};
#endif /* defined(__mybox2dtest__player__) */
