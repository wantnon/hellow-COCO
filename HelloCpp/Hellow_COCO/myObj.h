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
#include "myBox2dFunc.h"
using namespace cocos2d;
class CmyObj:public cocos2d::CCSprite
{
private:
	bool showSprite;//是否显示sprite
	bool showb2Body;//是否显示刚体线框
public:

	b2World* world_outref;//由外部引入，不由本类负责销毁
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
	virtual void draw(void)//重载CCSprite的draw函数
		//如果节点连在批处理节点上，则节点的draw不调用
	{
		if(showSprite){
			//调用父类draw
			CCSprite::draw();
		}
		if(showb2Body){
			//画线框
			//需要预铺设一层变换抵消掉锚点的影响，见日志：http://user.qzone.qq.com/350479720/blog/1380948589
			kmGLPushMatrix();//push
			kmGLTranslatef(m_obAnchorPointInPoints.x, m_obAnchorPointInPoints.y, 0 );
			cocos2d::ccDrawColor4B(255, 255, 255, 255);
			glLineWidth(2);
			//画this的位置
		//	ccDrawCircle(ccp(0,0), 10, 0, 10, true, 1, 1);	
			//画线框
			 //画所有子节点中的body
			 //必须保证所有的子节点都是CmyObj，否则可能发生崩溃（而且是否崩溃不确定）
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
					    }//得到vlist
		                ccDrawPoly(&vlist[0], nv, true);
				     }else if(shape->GetType()==b2Shape::e_circle){
						b2CircleShape*circleShape=(b2CircleShape*)shape;
					    b2Vec2 c=circleShape->m_p;//圆心（body局部空间）
					    float r=circleShape->m_radius;//半径
				        float rolleAngle=pBody->GetAngle();
					    ccDrawCircle(ccp(c.x*PTM_RATIO,c.y*PTM_RATIO), r*PTM_RATIO, rolleAngle, 10, true, 1, 1);
			         }
			     }
			}
			
		    kmGLPopMatrix();//pop
		}


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
	static vector<CmyObj*> createObjsWithXML(const string&xmlPathShort,b2World*world)
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
	void buoyancyEffect(float waterSurfaceHeight,int tgFixtureIndex=0)//浮力效果
		//对this->pb2Body的第tgFixtureIndex个（按添加顺序，而非fixtureList中的顺序）fixture计算浮力
	{
		CmyObj* pObj = this;
		if(pObj->pb2Body==NULL)return;
		if(pObj->pb2Body->GetType()!=b2_dynamicBody)return;//动态物体才受浮力
		float waterSurfaceHeight_b2d=waterSurfaceHeight/PTM_RATIO;
		b2Fixture*tgFixture=getFixtureAt_backWards(pObj->pb2Body,tgFixtureIndex);//目标fixture，只对此fixture计算浮力，其它fixture忽略
		assert(tgFixture);
		b2AABB aabb=tgFixture->GetAABB(0);
		if(aabb.lowerBound.y<=waterSurfaceHeight_b2d){//接触水体
			//用速度衰减模拟水的摩擦阻力
			pObj->pb2Body->SetAngularDamping(0.25);//0.5
			pObj->pb2Body->SetLinearDamping(0.8);
			//用水面将tgFixture的shape切割成两部分
			b2Shape*shape=tgFixture->GetShape();
			//计算shape沉入水面的面积
			float S=0;
			if(tgFixture->GetShape()->GetType()==b2Shape::e_polygon){//如果是多边形物体
				b2PolygonShape upperShape,lowerShape;
				bool upperShapeGot=false,lowerShapeGot=false;
				splitPolygonShapeWithHoriLine(*(b2PolygonShape*)shape, pObj->pb2Body->GetTransform(), waterSurfaceHeight,
					upperShapeGot,lowerShapeGot,
					upperShape, lowerShape);
				if(lowerShapeGot==false){
					//       cout<<"lowerShape not got!"<<endl;
					return;
				}
				//计算lowerShape的面积
				b2MassData massData;
				lowerShape.ComputeMass(&massData, 1);//当密度取1时算得的质量即为面积
				S=massData.mass;//面积

			}else if(tgFixture->GetShape()->GetType()==b2Shape::e_circle){//如果是circle
				
				b2Vec2 c=pObj->pb2Body->GetWorldCenter();//圆心世界坐标
				float r=tgFixture->GetShape()->m_radius;//圆半径
				if(waterSurfaceHeight>c.y){//如果圆心在水面之下
					float d=waterSurfaceHeight-c.y;
					float cos_A_rad=d/r;
					if(cos_A_rad>1)cos_A_rad=1;//这句非常重要，否则会崩溃
					float A_rad=2*acosf(cos_A_rad);
					float fanPercentage=1-A_rad/pi;//扇形百分比
					if(fanPercentage>1)fanPercentage=1;//不要超过1
					if(fanPercentage<0)fanPercentage=0;//不要小于0
					S=fanPercentage*pi*r*r;//扇形面积
				}else{//如果圆心在水面之上
					float d=c.y-waterSurfaceHeight;
					float cos_A_rad=d/r;
					if(cos_A_rad>1)cos_A_rad=1;//这句非常重要，否则会崩溃
					float A_rad=2*acosf(cos_A_rad);
					float fanPercentage=A_rad/pi;//扇形百分比
					if(fanPercentage>1)fanPercentage=1;//不要超过1
					if(fanPercentage<0)fanPercentage=0;//不要小于0
					S=fanPercentage*pi*r*r;//扇形面积
				}
			}else{//其它类型物体
				//跳过
				return;
			}
			//得到S
			//水的密度
			float density_water=20;
			//计算浮力
			b2Vec2 buoyancyForce=density_water*S*fabsf(world_outref->GetGravity().y)*b2Vec2(0,1);//向量必须放在最后
			//       cout<<"buoyancyForce:"<<buoyancyForce.y<<endl;
			//       cout<<"gravityForce:"<<pObj->pb2Body->GetMass()*fabsf(world->GetGravity().y)<<endl;
			//将浮力冲量施加到物体上
			pObj->pb2Body->ApplyForce(buoyancyForce, pObj->pb2Body->GetWorldCenter());
		}
		else{//不接触水体
			//恢复衰减值
			pObj->pb2Body->SetAngularDamping(0);
			pObj->pb2Body->SetLinearDamping(0);
		}
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
#endif /* defined(__mybox2dtest__myObj__) */
