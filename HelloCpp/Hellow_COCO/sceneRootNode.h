//
//  sceneRootNode.h
//  mybox2dtest
//
//  Created by apple on 13-7-20.
//
//

#ifndef __mybox2dtest__sceneRootNode__
#define __mybox2dtest__sceneRootNode__

#include <iostream>
#include "cocos2d.h"
#include "Box2D.h"
#include "myObj.h"
#include "myDef.h"
class CsceneRootNode:public cocos2d::CCNode
{
public:
    bool isShowBodyWireFrame;
    virtual ~CsceneRootNode(){}
    CsceneRootNode(){
        isShowBodyWireFrame=true;
    }
    void set_isShowBodyWireFrame(bool isShow){
        isShowBodyWireFrame=isShow;
    }

    virtual void draw(void){
        if(isShowBodyWireFrame==false)return;
        //画所有子节点中的body
        //必须保证所有的子节点都是CmyObj，否则可能发生崩溃（而且有时崩溃有时不崩溃）
        cocos2d::ccDrawColor4B(255, 255, 255, 255);
        glLineWidth(2);
        cocos2d::CCArray* arr=this->getChildren();
        CCObject* obj = NULL;
        CCARRAY_FOREACH(arr, obj) {
            CmyObj* pObj = (CmyObj*) obj;
            b2Body*pBody=pObj->pb2Body;
            if(pBody==NULL)continue;
            b2Fixture* fixture=pBody->GetFixtureList();
            for(;fixture;fixture=fixture->GetNext()){
                b2Shape*shape=fixture->GetShape();
                if(shape==NULL)continue;
                if(shape->GetType()==b2Shape::e_polygon){
                    b2PolygonShape*polyShape=(b2PolygonShape*)shape;
                    int nv=polyShape->GetVertexCount();
                    const b2Vec2* verts=polyShape->m_vertices;
                    vector<cocos2d::CCPoint> vlist;
                    for(int i=0;i<nv;i++){
                        b2Vec2 vert=pBody->GetWorldPoint(verts[i]);
                        vlist.push_back(ccp(vert.x*PTM_RATIO,vert.y*PTM_RATIO));
                    }//得到vlist
                    ccDrawPoly(&vlist[0], nv, true);
                    
                }else if(shape->GetType()==b2Shape::e_circle){
                    b2CircleShape*circleShape=(b2CircleShape*)shape;
                    b2Vec2 c=circleShape->m_p;//圆心（body局部空间）
                    c=pBody->GetWorldPoint(c);//c转化到世界空间
                    float r=circleShape->m_radius;//半径
                    float rolleAngle=pBody->GetAngle();
                    ccDrawCircle(ccp(c.x*PTM_RATIO,c.y*PTM_RATIO), r*PTM_RATIO, rolleAngle, 10, true, 1, 1);
                    
                
                }
            }
            
            
        }

    }
    
};
#endif /* defined(__mybox2dtest__sceneRootNode__) */
