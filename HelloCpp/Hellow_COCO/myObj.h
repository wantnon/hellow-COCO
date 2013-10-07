﻿//
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
#include "myBox2dFunc.h"
using namespace cocos2d;
class CmyObj:public cocos2d::CCSprite
{
private:
	bool showSprite;//�Ƿ���ʾsprite
	bool showb2Body;//�Ƿ���ʾ�����߿�
public:

	b2World* world_outref;//���ⲿ���룬���ɱ��ฺ������
	string sceneNodeType;
	b2Body* pb2Body;
	bool touchSolidf,touchSolid;
	GLuint fragColorLoc;
	CmyObj(b2World*world){
		showSprite=true;
		showb2Body=true;
		world_outref=world;
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
	void set_showSprite(bool value){
		showSprite=value;
	}
	bool get_showSprite(){
		return showSprite;
	}
	void set_showb2Body(bool value){
		showb2Body=value;
	}
	bool get_showb2Body(){
		return showb2Body;
	}
	virtual void draw(void)//����CCSprite��draw����
		//����ڵ�����������ڵ��ϣ���ڵ��draw������
	{
		if(showSprite){
			//���ø���draw
			CCSprite::draw();
		}
		if(showb2Body){
			//���߿�
			//��ҪԤ����һ��任������ê���Ӱ�죬����־��http://user.qzone.qq.com/350479720/blog/1380948589
			kmGLPushMatrix();//push
			kmGLTranslatef(m_obAnchorPointInPoints.x, m_obAnchorPointInPoints.y, 0 );
			cocos2d::ccDrawColor4B(255, 255, 255, 255);
			glLineWidth(2);
			//��this��λ��
		//	ccDrawCircle(ccp(0,0), 10, 0, 10, true, 1, 1);	
			//���߿�
			 //�������ӽڵ��е�body
			 //���뱣֤���е��ӽڵ㶼��CmyObj��������ܷ��������������Ƿ������ȷ����
			 b2Body*pBody=this->pb2Body;
			 if(pBody){
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
				           vlist.push_back(ccp(verts[i].x*PTM_RATIO,verts[i].y*PTM_RATIO));
					    }//�õ�vlist
		                ccDrawPoly(&vlist[0], nv, true);
				     }else if(shape->GetType()==b2Shape::e_circle){
						b2CircleShape*circleShape=(b2CircleShape*)shape;
					    b2Vec2 c=circleShape->m_p;//Բ�ģ�body�ֲ��ռ䣩
					    float r=circleShape->m_radius;//�뾶
				        float rolleAngle=pBody->GetAngle();
					    ccDrawCircle(ccp(c.x*PTM_RATIO,c.y*PTM_RATIO), r*PTM_RATIO, rolleAngle, 10, true, 1, 1);
			         }
			     }
			}
			
		    kmGLPopMatrix();//pop
		}


	};

	void initWithInfo(const CmyObjInfo&objInfo){//��myObjInfoȥ����myObj
		this->sceneNodeType=objInfo.sceneNodeType;
		vector<b2Body*> bodyList;
		//----����bodyList
		int nbody=(int)objInfo.bodyInfoList.size();
		for(int i=0;i<nbody;i++)
		{
			b2Body*body=NULL;
			const CbodyInfo&bodyInfo=objInfo.bodyInfoList[i];
			b2BodyDef bodyDef;
			bodyDef.type = bodyInfo.bodyType;
			body = world_outref->CreateBody(&bodyDef);//ͨ��������������
			//----������fixture
			int nfixture=(int)bodyInfo.fixtureInfoList.size();
			for(int i=0;i<nfixture;i++){
				const CfixtureInfo&fixtureInfo=bodyInfo.fixtureInfoList[i];
				b2FixtureDef fixtureDef;
				//���fixture�ĳ�shape��������Ա
				fixtureDef.density=fixtureInfo.density;
				fixtureDef.restitution=fixtureInfo.restitution;
				fixtureDef.friction=fixtureInfo.friction;
				fixtureDef.isSensor=fixtureInfo.isSensor;
				fixtureDef.filter.categoryBits=fixtureInfo.filterInfo.categoryBits;
				fixtureDef.filter.maskBits=fixtureInfo.filterInfo.maskBits;
				fixtureDef.filter.groupIndex=fixtureInfo.filterInfo.groupIndex;
				//���fixture��shape��Ա������fixture
				const CshapeInfo&shapeInfo=fixtureInfo.shapeInfo;
				switch (shapeInfo.shapeType) {
				case b2Shape::e_polygon:
					{
						b2PolygonShape shape;
						//���Ƿ��ṩ�˶����б�
						int nv=(int)shapeInfo.vertices.size();
						if(nv!=0){//�ṩ�˶����б��ʹ�ö����б��ʼ��
							shape.Set(&shapeInfo.vertices[0], nv);
						}else{//δ�ṩ�����б�����Ƿ��ṩ��rx,ry
							if(shapeInfo.rx!=0&&shapeInfo.ry!=0){//�ṩ��rx,ry
								shape.SetAsBox(shapeInfo.rx,shapeInfo.ry, shapeInfo.c, shapeInfo.rot);

							}else{//δ�ṩrx,ry
								cout<<"error:shapeTypeΪpolygonȴδ�ṩvertices��rxy!"<<endl;
								exit(0);
							}
						}//�õ�shape
						fixtureDef.shape=&shape;
						body->CreateFixture(&fixtureDef);//�����ڴ˴�createFixture���Ա���shapeʧЧ
					}break;
				case b2Shape::e_circle:
					{
						b2CircleShape circle;
						//���Ƿ��ṩ�˰뾶
						if(shapeInfo.r!=0){//�ṩ�˰뾶
							circle.m_p.Set(shapeInfo.c.x, shapeInfo.c.y);
							circle.m_radius=shapeInfo.r;
						}else{//δ�ṩ�뾶
							cout<<"error:shapeTypeΪcircleȴδ�ṩ�뾶!"<<endl;
							exit(0);
						}//�õ�shape
						fixtureDef.shape=&circle;
						body->CreateFixture(&fixtureDef);//�����ڴ˴�createFixture���Ա���shapeʧЧ
					}break;
				case b2Shape::e_edge:
					{
						cout<<"error:��δʵ��!"<<endl;
						exit(0);
					}break;
				case b2Shape::e_chain:
					{
						cout<<"error:��δʵ��!"<<endl;
						exit(0);
					}break;
				default:
					cout<<"error:δ֪����!"<<endl;
					exit(0);
					break;
				}
			}
			//----body������λ��
			{
				body->SetTransform(bodyInfo.pos, bodyInfo.rot);
			}
			//----gravityScale
			{
				body->SetGravityScale(bodyInfo.gravityScale);
			}
			//----��body����bodyList
			bodyList.push_back(body);
		}
		if((int)bodyList.size()>1){
			cout<<"error:���һ��body!"<<endl;
			exit(0);
		}
		//----����sprite
		{
			const CspriteInfo&spriteInfo=objInfo.spriteInfo;
			const CtexInfo&texInfo=spriteInfo.texInfo;
			CCTexture2D*texture=CCTextureCache::sharedTextureCache()->addImage(texInfo.texImage.c_str());
			CCRect rect;
			if(spriteInfo.useTexRect){//ʹ������rect
				CCSize texSize=texture->getContentSize();
				rect.setRect(0, 0, texSize.width, texSize.height);
			}else{//ʹ��ָ����rect
				rect.setRect(spriteInfo.rect.getMinX(),spriteInfo.rect.getMinY(),
					spriteInfo.rect.getMaxX()-spriteInfo.rect.getMinX(),
					spriteInfo.rect.getMaxY()-spriteInfo.rect.getMinY());

			}
			this->setTag(objInfo.tag);
			this->initWithTexture(texture, rect);
			this->setZOrder(spriteInfo.zOrder);
			ccTexParams texParams={GL_LINEAR,GL_LINEAR,texInfo.wrapS,texInfo.wrapT};//�ṹ��Ҳ����������ʼ��
			if(texInfo.specifyTexCoord){//����ֶ�ָ����������
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
	static vector<CmyObj*> createObjsWithXML(const string&xmlPathShort,b2World*world)
		//����xml�ļ�����һϵ�ж���
	{
		string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(xmlPathShort.c_str());
		tinyxml2::XMLDocument doc;
		doc.LoadFile(fullPath.c_str());
		XMLElement *root=doc.RootElement();//myObjList�ڵ�
		CmyObjGroupInfo myObjGroupInfo;
		myObjGroupInfo.initWithXMLElement(root);
		//      myObjListInfo.print();
		//����myObjListInfo�е���Ϣ����objList
		vector<CmyObj*> objList;
		int nObj=(int)myObjGroupInfo.myObjInfoList.size();
		for(int i=0;i<nObj;i++){
			const CmyObjInfo&objInfo=myObjGroupInfo.myObjInfoList[i];
			//����obj
			CmyObj*pObj=CmyObj::create(world);
			//��objInfo�е���Ϣȥ��ʼ��pObj
			pObj->initWithInfo(objInfo);
			objList.push_back(pObj);
		}

		return objList;


	}
	void buoyancyEffect(float waterSurfaceHeight,int tgFixtureIndex=0)//����Ч��
		//��this->pb2Body�ĵ�tgFixtureIndex���������˳�򣬶���fixtureList�е�˳��fixture���㸡��
	{
		CmyObj* pObj = this;
		if(pObj->pb2Body==NULL)return;
		if(pObj->pb2Body->GetType()!=b2_dynamicBody)return;//��̬������ܸ���
		float waterSurfaceHeight_b2d=waterSurfaceHeight/PTM_RATIO;
		b2Fixture*tgFixture=getFixtureAt_backWards(pObj->pb2Body,tgFixtureIndex);//Ŀ��fixture��ֻ�Դ�fixture���㸡��������fixture����
		assert(tgFixture);
		b2AABB aabb=tgFixture->GetAABB(0);
		if(aabb.lowerBound.y<=waterSurfaceHeight_b2d){//�Ӵ�ˮ��
			//���ٶ�˥��ģ��ˮ��Ħ������
			pObj->pb2Body->SetAngularDamping(0.3);//0.5
			pObj->pb2Body->SetLinearDamping(0.8);
			//��ˮ�潫tgFixture��shape�и��������
			b2Shape*shape=tgFixture->GetShape();
			//����shape����ˮ������
			float S=0;
			if(tgFixture->GetShape()->GetType()==b2Shape::e_polygon){//����Ƕ��������
				b2PolygonShape upperShape,lowerShape;
				bool upperShapeGot=false,lowerShapeGot=false;
				splitPolygonShapeWithHoriLine(*(b2PolygonShape*)shape, pObj->pb2Body->GetTransform(), waterSurfaceHeight,
					upperShapeGot,lowerShapeGot,
					upperShape, lowerShape);
				if(lowerShapeGot==false){
					//       cout<<"lowerShape not got!"<<endl;
					return;
				}
				//����lowerShape�����
				b2MassData massData;
				lowerShape.ComputeMass(&massData, 1);//���ܶ�ȡ1ʱ��õ�������Ϊ���
				S=massData.mass;//���

			}else if(tgFixture->GetShape()->GetType()==b2Shape::e_circle){//�����circle
				
				b2Vec2 c=pObj->pb2Body->GetWorldCenter();//Բ����������
				float r=tgFixture->GetShape()->m_radius;//Բ�뾶
				if(waterSurfaceHeight>c.y){//���Բ����ˮ��֮��
					float d=waterSurfaceHeight-c.y;
					float cos_A_rad=d/r;
					if(cos_A_rad>1)cos_A_rad=1;//���ǳ���Ҫ����������
					float A_rad=2*acosf(cos_A_rad);
					float fanPercentage=1-A_rad/pi;//���ΰٷֱ�
					if(fanPercentage>1)fanPercentage=1;//��Ҫ����1
					if(fanPercentage<0)fanPercentage=0;//��ҪС��0
					S=fanPercentage*pi*r*r;//�������
				}else{//���Բ����ˮ��֮��
					float d=c.y-waterSurfaceHeight;
					float cos_A_rad=d/r;
					if(cos_A_rad>1)cos_A_rad=1;//���ǳ���Ҫ����������
					float A_rad=2*acosf(cos_A_rad);
					float fanPercentage=A_rad/pi;//���ΰٷֱ�
					if(fanPercentage>1)fanPercentage=1;//��Ҫ����1
					if(fanPercentage<0)fanPercentage=0;//��ҪС��0
					S=fanPercentage*pi*r*r;//�������
				}
			}else{//������������
				//����
				return;
			}
			//�õ�S
			//ˮ���ܶ�
			float density_water=20;
			//���㸡��
			b2Vec2 buoyancyForce=density_water*S*fabsf(world_outref->GetGravity().y)*b2Vec2(0,1);//��������������
			//       cout<<"buoyancyForce:"<<buoyancyForce.y<<endl;
			//       cout<<"gravityForce:"<<pObj->pb2Body->GetMass()*fabsf(world->GetGravity().y)<<endl;
			//����������ʩ�ӵ�������
			pObj->pb2Body->ApplyForce(buoyancyForce, pObj->pb2Body->GetWorldCenter());
		}
		else{//���Ӵ�ˮ��
			//�ָ�˥��ֵ
			pObj->pb2Body->SetAngularDamping(0);
			pObj->pb2Body->SetLinearDamping(0);
		}
	}
	void removeMeFromSceneAndDestroyBody(bool cleanup){

		world_outref->DestroyBody(this->pb2Body);
		this->pb2Body=NULL;//Ϊ�˱���myObj����ʱ�ٴ�����body������һ��Ҫ��ָ����ΪNULL
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

    void create_pb2Body_circle(float r,float friction=0.5,const b2Filter*filter=NULL){
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
	void create_pb2Body_circle(float r,
								b2BodyType bodyType,
								float friction,float restitution,float density,bool isSensor,
								int categoryBits,int maskBits,int groupIndex,
								float gravityScale){
		assert(this->pb2Body==NULL);
		// Define the body.
		b2BodyDef bodyDef;
		bodyDef.type = bodyType;
		bodyDef.userData=this;

		b2Body *body = world_outref->CreateBody(&bodyDef);

		// Define box shape for our body.
		//shape
		b2CircleShape circleShape;
		circleShape.m_p.Set(0, 0);
		circleShape.m_radius=r;

		// Define the body fixture.
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &circleShape;
		fixtureDef.density =density;
		fixtureDef.friction = friction;
		b2Filter filter;
		{
			filter.categoryBits=categoryBits;
			filter.maskBits=maskBits;
			filter.groupIndex=groupIndex;
		}		
		fixtureDef.filter =filter;
		body->CreateFixture(&fixtureDef);
		body->SetGravityScale(gravityScale);
		this->pb2Body=body;
	}
	void create_pb2Body_polygon(const vector<b2Vec2>&vertList,b2BodyType bodyType,float friction=0.5,const b2Filter*filter=NULL){
		assert(this->pb2Body==NULL);
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
	void create_pb2Body_polygon(const vector<b2Vec2>&vertList,
								b2BodyType bodyType,
								float friction,float restitution,float density,bool isSensor,
								int categoryBits,int maskBits,int groupIndex,
								float gravityScale){
		assert(this->pb2Body==NULL);
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
		fixtureDef.density =density;
		fixtureDef.friction = friction;
		b2Filter filter;
		{
			filter.categoryBits=categoryBits;
			filter.maskBits=maskBits;
			filter.groupIndex=groupIndex;
		}		
		fixtureDef.filter =filter;
		body->CreateFixture(&fixtureDef);
		body->SetGravityScale(gravityScale);
		this->pb2Body=body;
	}



	void pushVxToTagvx(float tgvx){
		float m=pb2Body->GetMass();
		b2Vec2 v=pb2Body->GetLinearVelocity();
		//��v��x����Ū��vx
		b2Vec2 dv=b2Vec2(tgvx-v.x,0);
		b2Vec2 impluse=m*dv;
		pb2Body->ApplyLinearImpulse(impluse, pb2Body->GetWorldCenter());
	}
	void pushVyToTagvy(float tgvy){
		float m=pb2Body->GetMass();
		b2Vec2 v=pb2Body->GetLinearVelocity();
		//��v��y����Ū��vy
		b2Vec2 dv=b2Vec2(0,tgvy-v.y);
		b2Vec2 impluse=m*dv;
		pb2Body->ApplyLinearImpulse(impluse, pb2Body->GetWorldCenter());
	}
};
#endif /* defined(__mybox2dtest__myObj__) */
