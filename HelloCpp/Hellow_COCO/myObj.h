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
#include "tinyxml2.h"
#include "myfunc.h"
#include "myDef.h"
using namespace cocos2d;
using namespace tinyxml2;
//所有的info都是轻结构，不涉及资源，因此info结构均在栈上创建即可。
class CshapeInfo{
public:
	b2Shape::Type shapeType;
	b2Vec2 c;
	float r;
	float rx,ry;
	float rot;
	vector<b2Vec2> vertices;
	void print(){
		//shapeType
		{
			cout<<"shapeType:"<<shapeType<<endl;
		}
		//c
		{
			cout<<"c:"<<c.x<<","<<c.y<<endl;
		}
		//r
		{
			cout<<"r:"<<r<<endl;
		}
		//rx,ry
		{
			cout<<"rx,ry:"<<rx<<","<<ry<<endl;

		}
		//rot
		{
			cout<<"rot:"<<rot<<endl;
		}
		//vertices
		{
			cout<<"vertices:";
			int nv=(int)vertices.size();
			for(int i=0;i<nv;i++){
				cout<<"("<<vertices[i].x<<","<<vertices[i].y<<")";
			}
			cout<<endl;
		}
	}
	void initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="shape"){
			cout<<"error:不是shape节点!"<<endl;
			exit(0);
		}
		//shapeType
		string shapeTypeStr=p->FirstChildElement("shapeType")->GetText();
		if(shapeTypeStr=="circle"){
			shapeType=b2Shape::e_circle;
		}else if(shapeTypeStr=="edge"){
			shapeType=b2Shape::e_edge;
		}else if(shapeTypeStr=="polygon"){
			shapeType=b2Shape::e_polygon;
		}else if(shapeTypeStr=="e_chain"){
			shapeType=b2Shape::e_chain;
		}else{
			cout<<"error:未知类型!"<<endl;
			exit(0);
		}
		//c
		{
			vector<float> valist=strToValueList(p->FirstChildElement("c")->GetText());
			if((int)valist.size()!=2){
				cout<<"error:valist.size()!=2"<<endl;
				exit(0);
			}
			c.Set(valist[0]/PTM_RATIO, valist[1]/PTM_RATIO);
		}
		//r
		{
			vector<float> valist=strToValueList(p->FirstChildElement("r")->GetText());
			r=valist[0]/PTM_RATIO;
		}
		//rxry
		{
			vector<float> valist=strToValueList(p->FirstChildElement("rxy")->GetText());
			rx=valist[0]/PTM_RATIO;
			ry=valist[1]/PTM_RATIO;
		}
		//rot
		{
			vector<float> valist=strToValueList(p->FirstChildElement("rot")->GetText());
			rot=valist[0];

		}
		//vertices
		{
			//如果vertices节点没有内容，则p->FirstChildElement("vertices")->GetText()返回NULL,
			//直接将NULL传给string参数会崩溃，所以要加一个toString转一下，它将非空的char*转成string
			//将NULL转成空string
			vector<float> valist=strToValueList(toString(p->FirstChildElement("vertices")->GetText()));
			int nvalue=(int)valist.size();
			if(nvalue%2!=0){
				cout<<"error:数值个数不为偶数!"<<endl;
				exit(0);
			}
			for(int i=0;i<nvalue;i+=2){
				b2Vec2 v(valist[i]/PTM_RATIO,valist[i+1]/PTM_RATIO);
				vertices.push_back(v);
			}
		}
	}
};
class CfilterInfo{
public:
	uint16 categoryBits;
	uint16 maskBits;
	int16 groupIndex;
	void print(){

		//categoryBits
		{
			cout<<"categoryBits:"<<categoryBits<<endl;
		}
		//maskBits
		{
			cout<<"maskBits:"<<maskBits<<endl;
		}
		//groupIndex
		{
			cout<<"groupIndex:"<<groupIndex<<endl;
		}

	}
	void initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="filter"){
			cout<<"error:不是filter节点!"<<endl;
			exit(0);
		}
		//categoryBits
		{
			vector<float> valueList=strToValueList(p->FirstChildElement("categoryBits")->GetText());
			categoryBits=valueList[0];
		}
		//maskBits
		{
			vector<float> valueList=strToValueList(p->FirstChildElement("maskBits")->GetText());
			maskBits=valueList[0];

		}
		//groupIndex
		{
			vector<float> valueList=strToValueList(p->FirstChildElement("groupIndex")->GetText());
			groupIndex=valueList[0];
		}
	}
};
class CfixtureInfo{
public:
	CshapeInfo shapeInfo;
	float friction;
	float restitution;
	float density;
	bool isSensor;
	CfilterInfo filterInfo;
	void print(){
		//shapeInfo
		shapeInfo.print();
		//friction
		{
			cout<<"friction:"<<friction<<endl;
		}
		//restitution
		{
			cout<<"restitution:"<<restitution<<endl;
		}
		//density
		{
			cout<<"density:"<<density<<endl;
		}
		//isSensor
		{
			cout<<"isSensor:"<<isSensor<<endl;
		}
		//filterInfo
		filterInfo.print();
	}
	void initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="fixture"){
			cout<<"error:不是fixture节点!"<<endl;
			exit(0);
		}
		//shapeInfo
		shapeInfo.initWithXMLElement(p->FirstChildElement("shape"));
		//friction
		{
			vector<float> valueList=strToValueList(p->FirstChildElement("friction")->GetText());
			friction=valueList[0];
		}
		//restitution
		{
			vector<float> valueList=strToValueList(p->FirstChildElement("restitution")->GetText());
			restitution=valueList[0];
		}
		//density
		{
			vector<float> valueList=strToValueList(p->FirstChildElement("density")->GetText());
			density=valueList[0];
		}
		//isSensor
		{
			vector<float> valueList=strToValueList(p->FirstChildElement("isSensor")->GetText());
			isSensor=valueList[0];
		}
		//filterInfo
		filterInfo.initWithXMLElement(p->FirstChildElement("filter"));
	}
};
class CbodyInfo{
public:
	b2BodyType bodyType;
	vector<CfixtureInfo> fixtureInfoList;
	b2Vec2 pos;
	float rot;
	float gravityScale;
	void print(){
		//bodyType
		{
			cout<<"bodyType:"<<bodyType<<endl;
		}
		//fixtureInfoList
		{
			int nfixture=(int)fixtureInfoList.size();
			for(int i=0;i<nfixture;i++){
				fixtureInfoList[i].print();
			}
		}
		//pos
		{
			cout<<"pos:"<<pos.x<<","<<pos.y<<endl;
		}
		//rot
		{
			cout<<"rot:"<<rot<<endl;
		}
		//gravityScale
		{
			cout<<"gravityScale:"<<gravityScale<<endl;
		}

	}
	void initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="body"){
			cout<<"error:不是body节点!"<<endl;
			exit(0);
		}
		//bodyType
		bodyType=getBodyType(p->FirstChildElement("bodyType")->GetText());
		//fixtureInfoList
		XMLElement*fixture=p->FirstChildElement("fixture");
		while(fixture){
			CfixtureInfo fixtureInfo;
			fixtureInfo.initWithXMLElement(fixture);
			fixtureInfoList.push_back(fixtureInfo);
			fixture=fixture->NextSiblingElement("fixture");
		}
		//pos
		{
			vector<float> valueList=strToValueList(p->FirstChildElement("pos")->GetText());
			pos.Set(valueList[0]/PTM_RATIO, valueList[1]/PTM_RATIO);
		}
		//rot
		{
			vector<float> valueList=strToValueList(p->FirstChildElement("rot")->GetText());
			rot=valueList[0];
		}
		//gravityScale
		{
			vector<float> valueList=strToValueList(p->FirstChildElement("gravityScale")->GetText());
			gravityScale=valueList[0];

		}
	}
private:
	b2BodyType getBodyType(const string&str){
		if(str=="staticBody"){
			return b2_staticBody;
		}else if(str=="kinematicBody"){
			return b2_kinematicBody;
		}else if(str=="dynamicBody"){
			return b2_dynamicBody;
		}else{
			cout<<"error:未知类型"<<endl;
			exit(0);
		}
	}

};
class CquadTexCoordInfo{
public:
	CCPoint tl,tr,bl,br;
	void print(){
		//tl
		{
			cout<<"tl:"<<tl.x<<","<<tl.y<<endl;
		}
		//tr
		{
			cout<<"tr:"<<tr.x<<","<<tr.y<<endl;
		}
		//bl
		{
			cout<<"bl:"<<bl.x<<","<<bl.y<<endl;
		}
		//br
		{
			cout<<"br:"<<br.x<<","<<br.y<<endl;
		}

	}
	void initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="quadTexCoord"){
			cout<<"error:不是quadTexCoord节点!"<<endl;
			exit(0);
		}
		//tl
		{
			vector<float> valist=strToValueList(p->FirstChildElement("tl")->GetText());
			tl.setPoint(valist[0], valist[1]);
		}
		//tr
		{
			vector<float> valist=strToValueList(p->FirstChildElement("tr")->GetText());
			tr.setPoint(valist[0], valist[1]);
		}
		//bl
		{
			vector<float> valist=strToValueList(p->FirstChildElement("bl")->GetText());
			bl.setPoint(valist[0], valist[1]);
		}
		//br
		{
			vector<float> valist=strToValueList(p->FirstChildElement("br")->GetText());
			br.setPoint(valist[0], valist[1]);
		}


	}
};
class CtexInfo{
public:
	string texImage;
	GLuint wrapS;
	GLuint wrapT;
	bool specifyTexCoord;
	CquadTexCoordInfo quadTexCoordInfo;
	void print(){
		//texImage
		{
			cout<<"texImage:"<<texImage<<endl;
		}
		//wrapS
		{
			cout<<"wrapS:"<<wrapS<<endl;
		}
		//wrapT
		{
			cout<<"wrapT:"<<wrapT<<endl;
		}
		//specifyTexCoord
		{
			cout<<"specifyTexCoord:"<<specifyTexCoord<<endl;
		}
		//quadTexCoordInfo
		{
			quadTexCoordInfo.print();
		}

	}
	void initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="tex"){
			cout<<"error:不是tex节点!"<<endl;
			exit(0);
		}
		//texImage
		texImage=p->FirstChildElement("texImage")->GetText();
		//wrapS
		{
			wrapS=getWrap(p->FirstChildElement("wrapS")->GetText());
		}
		//wrapT
		{
			wrapT=getWrap(p->FirstChildElement("wrapT")->GetText());
		}
		//specifyTexCoord
		{
			vector<float> valist=strToValueList(p->FirstChildElement("specifyTexCoord")->GetText());
			specifyTexCoord=valist[0];
		}
		//quadTexCoordInfo
		{
			quadTexCoordInfo.initWithXMLElement(p->FirstChildElement("quadTexCoord"));
		}

	}
private:
	GLuint getWrap(const string&str){
		if(str=="repeat"){
			return GL_REPEAT;
		}else if(str=="clamp_to_edge"){
			return GL_CLAMP_TO_EDGE;
		}else if(str=="mirrored_repeat"){
			return GL_MIRRORED_REPEAT;
		}else{
			cout<<"error:未知类型!"<<endl;
			exit(0);
		}

	}
};
class CspriteInfo{
public:
	bool useTexRect;
	CCRect rect;
	CtexInfo texInfo;
	float zOrder;
	CCPoint anchorPoint;
	CCPoint pos;
	void print(){
		//useTexRect
		{
			cout<<"useTexRect:"<<useTexRect<<endl;
		}
		//rect
		{
			cout<<"rect:"<<rect.getMinX()<<","<<rect.getMaxX()<<","<<rect.getMinY()<<","<<rect.getMaxY()<<endl;
		}
		//texInfo
		{
			texInfo.print();
		}
		//zOrder
		{
			cout<<"zOrder:"<<zOrder<<endl;
		}
		//anchorPoint
		{
			cout<<"anchorPoint:"<<anchorPoint.x<<","<<anchorPoint.y<<endl;
		}
		//pos
		{
			cout<<"pos:"<<pos.x<<" "<<pos.y<<endl;
		}
	}
	void initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="sprite"){
			cout<<"error:不是sprite节点!"<<endl;
			exit(0);
		}
		//useTexRect
		{
			vector<float> vaList=strToValueList(p->FirstChildElement("useTexRect")->GetText());
			useTexRect=vaList[0];

		}
		//rect
		{
			vector<float> vaList=strToValueList(p->FirstChildElement("rect")->GetText());
			rect.setRect(vaList[0], vaList[1], vaList[2], vaList[3]);
		}
		//texInfo
		{
			texInfo.initWithXMLElement(p->FirstChildElement("tex"));
		}
		//zOrder
		{
			vector<float> valist=strToValueList(p->FirstChildElement("zOrder")->GetText());
			zOrder=valist[0];
		}
		//anchorPoint
		{
			vector<float> valist=strToValueList(p->FirstChildElement("anchorPoint")->GetText());
			anchorPoint.setPoint(valist[0], valist[1]);
		}
		//pos
		{
			vector<float> valist=strToValueList(p->FirstChildElement("pos")->GetText());
			pos.setPoint(valist[0], valist[1]);
		}

	}

};
class CmyObjInfo{
public:
	string name;
	string sceneNodeType;
	int tag;
	vector<CbodyInfo> bodyInfoList;//元素个数为0或1
	CspriteInfo spriteInfo;
	void print(){
		//name
		cout<<"name:"<<name<<endl;
		//sceneNodeType
		cout<<"sceneNodeType:"<<sceneNodeType<<endl;
		//tag
		cout<<"tag:"<<tag<<endl;
		//bodyInfoList
		int nBodyInfo=(int)bodyInfoList.size();
		for(int i=0;i<nBodyInfo;i++){
			bodyInfoList[i].print();
		}
		//spriteInfo
		spriteInfo.print();
	}
	void initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="myObj"){
			cout<<"error:不是myObj节点!"<<endl;
			exit(0);
		}
		//name
		name=p->Attribute("name");
		//sceneNodeType
		sceneNodeType=p->Attribute("sceneNodeType");
		//tag
		{
			const char* tagstr=p->Attribute("tag");
			tag=tagstr==NULL?0:getTag(tagstr);
		}
		//bodyList
		{
			XMLElement*body_elem=p->FirstChildElement("body");
			while(body_elem){
				CbodyInfo bodyInfo;
				bodyInfo.initWithXMLElement(body_elem);
				bodyInfoList.push_back(bodyInfo);
				body_elem=body_elem->NextSiblingElement("body");
			}
			if((int)bodyInfoList.size()>1){
				cout<<"error:单个myObj中body节点个数不得大于1!"<<endl;
				exit(0);
			}

		}
		//sprite
		spriteInfo.initWithXMLElement(p->FirstChildElement("sprite"));
	}
	int getTag(const string&str){
		if(str=="tag_backGround"){
			return tag_backGround;
		}else if(str=="tag_backGround2"){
			return tag_backGround2;
		}else if(str=="tag_player"){
			return tag_player;
		}else if(str=="tag_shotBall_right"){
			return tag_shotBall_right;
		}else if(str=="tag_shotBall_left"){
			return tag_shotBall_left;
		}else{
			cout<<"error:未知类型!"<<endl;
			//    cout<<str<<endl;
			exit(0);
		}

	}


};
class CmyObjListInfo{
public:
	vector<CmyObjInfo> myObjInfoList;
	void print(){
		int nMyObj=(int)myObjInfoList.size();
		for(int i=0;i<nMyObj;i++){
			myObjInfoList[i].print();
		}
	}
	void initWithXMLElement(XMLElement *p){//p需要myObjList节点
		if(toString(p->Name())!="myObjList"){
			cout<<"error:不是myObjList节点!"<<endl;
			exit(0);
		}
		XMLElement* myObj=p->FirstChildElement("myObj");
		while (myObj) {
			CmyObjInfo myObjInfo;
			myObjInfo.initWithXMLElement(myObj);
			myObjInfoList.push_back(myObjInfo);
			myObj=myObj->NextSiblingElement();
		}
	}
};
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
	~CmyObj(){
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
	void changeShader(){
		GLchar * fragSource = (GLchar*) CCString::createWithContentsOfFile(
		CCFileUtils::sharedFileUtils()->fullPathForFilename("data/global/shader/blackWhite.fsh").c_str())->getCString();
		CCGLProgram* pProgram = new CCGLProgram();
		pProgram->initWithVertexShaderByteArray(ccPositionTextureColor_vert, fragSource);
		this->setShaderProgram(pProgram);
		pProgram->release();

		CHECK_GL_ERROR_DEBUG();

		this->getShaderProgram()->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		this->getShaderProgram()->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
		this->getShaderProgram()->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

		CHECK_GL_ERROR_DEBUG();

		this->getShaderProgram()->link();

		CHECK_GL_ERROR_DEBUG();

		this->getShaderProgram()->updateUniforms();

		CHECK_GL_ERROR_DEBUG();

		fragColorLoc = glGetUniformLocation( getShaderProgram()->getProgram(), "basicFragColor");

		CHECK_GL_ERROR_DEBUG();
	}
	virtual void draw(void)//重载CCSprite的draw函数
		//如果节点连在批处理节点上，则节点的draw不调用
	{
/*		//uniform传值
		getShaderProgram()->use();//给program传值之前要保证当前使用的是这个program
		GLfloat fragColorValue[4]={1,0,0,1};
		getShaderProgram()->setUniformLocationWith4fv(fragColorLoc, fragColorValue, 1);
*/
		//调用父类draw
		CCSprite::draw();

	};
	void initWithInfo(const CmyObjInfo&objInfo){//由myObjInfo去创建myObj
		//注意：通过bodyDef.position.set(x,y)设定位置,x,y为左下角（待确认）
		//通过groundBody->SetTransform(pos, angle)设定位置,pos为中心
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
		//更换shader
	//	this->changeShader();

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
		CmyObjListInfo myObjListInfo;
		myObjListInfo.initWithXMLElement(root);
		//      myObjListInfo.print();
		//利用myObjListInfo中的信息生成objList
		vector<CmyObj*> objList;
		int nObj=(int)myObjListInfo.myObjInfoList.size();
		for(int i=0;i<nObj;i++){
			const CmyObjInfo&objInfo=myObjListInfo.myObjInfoList[i];
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

	
	virtual void create_pb2Body(float rx,float ry,float friction=0.5,const b2Filter*filter=NULL){
		// Define the dynamic body.
		//Set up a 1m squared box in the physics world
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.userData=this;

		b2Body *body = world_outref->CreateBody(&bodyDef);

		// Define another box shape for our dynamic body.
		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(rx,ry);//These are mid points for our 1m box

		// Define the dynamic body fixture.
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = friction;
		if(filter!=NULL){
			fixtureDef.filter =*filter;
		}
		body->CreateFixture(&fixtureDef);
		pb2Body=body;
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

class C_myObjBatchNode:public cocos2d::CCSpriteBatchNode
{
public:
	virtual void draw(void){//重载
		//    CCSpriteBatchNode::draw();

	}
	static C_myObjBatchNode* create(const char *fileImage, unsigned int capacity/* = kDefaultSpriteBatchCapacity*/)
		//覆盖掉CCSpriteBatchNode的create函数
	{
		C_myObjBatchNode *batchNode = new C_myObjBatchNode();
		batchNode->initWithFile(fileImage, capacity);
		batchNode->autorelease();

		return batchNode;
	}
};
#endif /* defined(__mybox2dtest__myObj__) */
