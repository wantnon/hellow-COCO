//
//  myObj.h
//  mybox2dtest
//
//  Created by apple on 13-7-8.
//
//

#ifndef __mybox2dtest__myObj__
#define __mybox2dtest__myObj__

#include <iostream>
using namespace std;
#include "cocos2d.h"
#include "Box2D.h"
#include "myfunc.h"
#include "myDef.h"
#include "xmlLoader_myObj.h"
using namespace cocos2d;
class CmyObj:public cocos2d::CCSprite
{
public:
	b2World* world_outref;//由外部引入，不由本类负责销毁
	string sceneNodeType;
	b2Body* pb2Body;
	bool touchSolidf,touchSolid;
	float angularDamping_normal;
	float linearDamping_normal;
	float angularDamping_inWater;
	float linearDamping_inWater;
	GLuint fragColorLoc;
	CmyObj(b2World*world){
		world_outref=world;
		angularDamping_normal=0;
		linearDamping_normal=0;
		angularDamping_inWater=0.5;
		linearDamping_inWater=0.8;
		touchSolid=touchSolidf=false;
		pb2Body=NULL;
		fragColorLoc=0;
	}
	virtual ~CmyObj(){
		if(pb2Body!=NULL){
			world_outref->DestroyBody(pb2Body);
			pb2Body=NULL;
		}
	}
	static CmyObj * create(b2World*world)
	{
		CmyObj * pRet = new CmyObj(world);
	
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
	virtual void draw(void)//重载CCSprite的draw函数
		//如果节点连在批处理节点上，则节点的draw不调用
	{
		//调用父类draw
		CCSprite::draw();

	};

	void initWithInfo(const CmyObjInfo&objInfo){//由myObjInfo去创建myObj
		this->sceneNodeType=objInfo.sceneNodeType;
		vector<b2Body*> bodyList;
		//----创建bodyList
		int nbody=(int)objInfo.bodyInfoList.size();
		for(int i=0;i<nbody;i++)
		{
			b2Body*body=NULL;
			const CbodyInfo&bodyInfo=objInfo.bodyInfoList[i];
			b2BodyDef bodyDef;
			bodyDef.type = bodyInfo.bodyType;
			body = world_outref->CreateBody(&bodyDef);//通过工厂创建物体
			//----创建各fixture
			int nfixture=(int)bodyInfo.fixtureInfoList.size();
			for(int i=0;i<nfixture;i++){
				const CfixtureInfo&fixtureInfo=bodyInfo.fixtureInfoList[i];
				b2FixtureDef fixtureDef;
				//填充fixture的除shape外其它成员
				fixtureDef.density=fixtureInfo.density;
				fixtureDef.restitution=fixtureInfo.restitution;
				fixtureDef.friction=fixtureInfo.friction;
				fixtureDef.isSensor=fixtureInfo.isSensor;
				fixtureDef.filter.categoryBits=fixtureInfo.filterInfo.categoryBits;
				fixtureDef.filter.maskBits=fixtureInfo.filterInfo.maskBits;
				fixtureDef.filter.groupIndex=fixtureInfo.filterInfo.groupIndex;
				//填充fixture的shape成员并创建fixture
				const CshapeInfo&shapeInfo=fixtureInfo.shapeInfo;
				switch (shapeInfo.shapeType) {
				case b2Shape::e_polygon:
					{
						b2PolygonShape shape;
						//看是否提供了顶点列表
						int nv=(int)shapeInfo.vertices.size();
						if(nv!=0){//提供了顶点列表，使用顶点列表初始化
							shape.Set(&shapeInfo.vertices[0], nv);
						}else{//未提供顶点列表，看是否提供了rx,ry
							if(shapeInfo.rx!=0&&shapeInfo.ry!=0){//提供了rx,ry
								shape.SetAsBox(shapeInfo.rx,shapeInfo.ry, shapeInfo.c, shapeInfo.rot);

							}else{//未提供rx,ry
								cout<<"error:shapeType为polygon却未提供vertices或rxy!"<<endl;
								exit(0);
							}
						}//得到shape
						fixtureDef.shape=&shape;
						body->CreateFixture(&fixtureDef);//必须在此处createFixture，以避免shape失效
					}break;
				case b2Shape::e_circle:
					{
						b2CircleShape circle;
						//看是否提供了半径
						if(shapeInfo.r!=0){//提供了半径
							circle.m_p.Set(shapeInfo.c.x, shapeInfo.c.y);
							circle.m_radius=shapeInfo.r;
						}else{//未提供半径
							cout<<"error:shapeType为circle却未提供半径!"<<endl;
							exit(0);
						}//得到shape
						fixtureDef.shape=&circle;
						body->CreateFixture(&fixtureDef);//必须在此处createFixture，以避免shape失效
					}break;
				case b2Shape::e_edge:
					{
						cout<<"error:尚未实现!"<<endl;
						exit(0);
					}break;
				case b2Shape::e_chain:
					{
						cout<<"error:尚未实现!"<<endl;
						exit(0);
					}break;
				default:
					cout<<"error:未知类型!"<<endl;
					exit(0);
					break;
				}
			}
			//----body的整体位姿
			{
				body->SetTransform(bodyInfo.pos, bodyInfo.rot);
			}
			//----gravityScale
			{
				body->SetGravityScale(bodyInfo.gravityScale);
			}
			//----将body加入bodyList
			bodyList.push_back(body);
		}
		if((int)bodyList.size()>1){
			cout<<"error:最多一个body!"<<endl;
			exit(0);
		}
		//----创建sprite
		{
			const CspriteInfo&spriteInfo=objInfo.spriteInfo;
			const CtexInfo&texInfo=spriteInfo.texInfo;
			CCTexture2D*texture=CCTextureCache::sharedTextureCache()->addImage(texInfo.texImage.c_str());
			CCRect rect;
			if(spriteInfo.useTexRect){//使用纹理rect
				CCSize texSize=texture->getContentSize();
				rect.setRect(0, 0, texSize.width, texSize.height);
			}else{//使用指定的rect
				rect.setRect(spriteInfo.rect.getMinX(),spriteInfo.rect.getMinY(),
					spriteInfo.rect.getMaxX()-spriteInfo.rect.getMinX(),
					spriteInfo.rect.getMaxY()-spriteInfo.rect.getMinY());

			}
			this->setTag(objInfo.tag);
			this->initWithTexture(texture, rect);
			this->setZOrder(spriteInfo.zOrder);
			ccTexParams texParams={GL_LINEAR,GL_LINEAR,texInfo.wrapS,texInfo.wrapT};//结构体也可以这样初始化
			if(texInfo.specifyTexCoord){//如果手动指定纹理坐标
				const CquadTexCoordInfo&quadTexCoordInfo=texInfo.quadTexCoordInfo;
				this->set_quadTexCoords(quadTexCoordInfo.tl, quadTexCoordInfo.tr, quadTexCoordInfo.bl, quadTexCoordInfo.br);
			}
			this->setAnchorPoint(spriteInfo.anchorPoint);
			this->setPosition(spriteInfo.pos);
			this->getTexture()->setTexParameters(&texParams);
			this->pb2Body=bodyList.empty()?NULL:bodyList[0];
			if(this->pb2Body!=NULL){
				this->pb2Body->SetUserData(this);
			}
		}

	}
	void set_quadTexCoords(const CCPoint&tl,const CCPoint&tr,const CCPoint&bl,const CCPoint&br){
		this->m_sQuad.tl.texCoords.u=tl.x;
		this->m_sQuad.tl.texCoords.v=tl.y;

		this->m_sQuad.tr.texCoords.u=tr.x;
		this->m_sQuad.tr.texCoords.v=tr.y;

		this->m_sQuad.bl.texCoords.u=bl.x;
		this->m_sQuad.bl.texCoords.v=bl.y;

		this->m_sQuad.br.texCoords.u=br.x;
		this->m_sQuad.br.texCoords.v=br.y;

	}
	static vector<CmyObj*> createObjsWithXML(b2World*world,const string&xmlPathShort)
		//根据xml文件创建一系列对象
	{
		string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(xmlPathShort.c_str());
		tinyxml2::XMLDocument doc;
		doc.LoadFile(fullPath.c_str());
		XMLElement *root=doc.RootElement();//myObjList节点
		CmyObjGroupInfo myObjGroupInfo;
		myObjGroupInfo.initWithXMLElement(root);
		//      myObjListInfo.print();
		//利用myObjListInfo中的信息生成objList
		vector<CmyObj*> objList;
		int nObj=(int)myObjGroupInfo.myObjInfoList.size();
		for(int i=0;i<nObj;i++){
			const CmyObjInfo&objInfo=myObjGroupInfo.myObjInfoList[i];
			//创建obj
			CmyObj*pObj=CmyObj::create(world);
			//用objInfo中的信息去初始化pObj
			pObj->initWithInfo(objInfo);
			objList.push_back(pObj);
		}

		return objList;


	}
	void removeMeFromSceneAndDestroyBody(bool cleanup){

		world_outref->DestroyBody(this->pb2Body);
		this->pb2Body=NULL;//为了避免myObj析构时再次销毁body，这里一定要将指针置为NULL
		this->removeFromParentAndCleanup(cleanup);

	}

	bool isChange_touchSolid(){
		return (touchSolid!=touchSolidf);
	}
	void update_touchSolid(bool newTouchSolid){
		touchSolidf=touchSolid;
		touchSolid=newTouchSolid;

	}

	
	void create_pb2Body_box(float rx,float ry,b2BodyType bodyType,float friction=0.5,const b2Filter*filter=NULL){
		// Define the body.
		b2BodyDef bodyDef;
		bodyDef.type = bodyType;
		bodyDef.userData=this;

		b2Body *body = world_outref->CreateBody(&bodyDef);

		// Define box shape for our body.
		b2PolygonShape Box;
		Box.SetAsBox(rx,ry);

		// Define the body fixture.
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &Box;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = friction;
		if(filter!=NULL){
			fixtureDef.filter =*filter;
		}
		body->CreateFixture(&fixtureDef);
		this->pb2Body=body;
	}
	virtual void create_pb2Body_circle(float r,float friction=0.5,const b2Filter*filter=NULL){
		// Define the dynamic body.
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.userData=this;

		b2Body *body = world_outref->CreateBody(&bodyDef);

		//shape
		b2CircleShape circle;
		circle.m_p.Set(0, 0);
		circle.m_radius=r;

		// Define fixture.
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &circle;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = friction;
		if(filter!=NULL){
			fixtureDef.filter =*filter;
		}
		body->CreateFixture(&fixtureDef);
		pb2Body=body;
	}
	void create_pb2Body_polygon(const vector<b2Vec2>&vertList,b2BodyType bodyType,float friction=0.5,const b2Filter*filter=NULL){
		// Define the body.
		b2BodyDef bodyDef;
		bodyDef.type = bodyType;
		bodyDef.userData=this;

		b2Body *body = world_outref->CreateBody(&bodyDef);

		// Define box shape for our body.
		b2PolygonShape polygonShape;
		int nv=(int)vertList.size();
		polygonShape.Set(&vertList[0],nv);

		// Define the body fixture.
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &polygonShape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = friction;
		if(filter!=NULL){
			fixtureDef.filter =*filter;
		}
		body->CreateFixture(&fixtureDef);
		this->pb2Body=body;
	}


	void pushVxToTagvx(float tgvx){
		float m=pb2Body->GetMass();
		b2Vec2 v=pb2Body->GetLinearVelocity();
		//将v的x分量弄成vx
		b2Vec2 dv=b2Vec2(tgvx-v.x,0);
		b2Vec2 impluse=m*dv;
		pb2Body->ApplyLinearImpulse(impluse, pb2Body->GetWorldCenter());
	}
	void pushVyToTagvy(float tgvy){
		float m=pb2Body->GetMass();
		b2Vec2 v=pb2Body->GetLinearVelocity();
		//将v的y分量弄成vy
		b2Vec2 dv=b2Vec2(0,tgvy-v.y);
		b2Vec2 impluse=m*dv;
		pb2Body->ApplyLinearImpulse(impluse, pb2Body->GetWorldCenter());
	}
};
/*
class C_myObjBatchNode:public cocos2d::CCSpriteBatchNode
{
public:
	virtual void draw(void){//重载
		//    CCSpriteBatchNode::draw();

	}
	static C_myObjBatchNode* create(const char *fileImage, unsigned int capacity/* = kDefaultSpriteBatchCapacity*/
/*	)
		//覆盖掉CCSpriteBatchNode的create函数
	{
		C_myObjBatchNode *batchNode = new C_myObjBatchNode();
		batchNode->initWithFile(fileImage, capacity);
		batchNode->autorelease();

		return batchNode;
	}
};*/
#endif /* defined(__mybox2dtest__myObj__) */
