
#ifndef __xmlLoader_myObj__
#define __xmlLoader_myObj__

#include <iostream>
using namespace std;
#include "cocos2d.h"
#include "Box2D.h"
#include "tinyxml2.h"
#include "myfunc.h"
#include "myDef.h"
using namespace cocos2d;
using namespace tinyxml2;
//���е�info������ṹ�����漰��Դ�����info�ṹ����ջ�ϴ������ɡ�
class CshapeInfo{
public:
	b2Shape::Type shapeType;
	b2Vec2 c;
	float r;
	float rx,ry;
	float rot;
	vector<b2Vec2> vertices;
	void print();
	void initWithXMLElement(XMLElement *p);
};
class CfilterInfo{
public:
	uint16 categoryBits;
	uint16 maskBits;
	int16 groupIndex;
	void print();
	void initWithXMLElement(XMLElement *p);
};
class CfixtureInfo{
public:
	CshapeInfo shapeInfo;
	float friction;
	float restitution;
	float density;
	bool isSensor;
	CfilterInfo filterInfo;
	void print();
	void initWithXMLElement(XMLElement *p);
};
class CbodyInfo{
public:
	b2BodyType bodyType;
	vector<CfixtureInfo> fixtureInfoList;
	b2Vec2 pos;
	float rot;
	float gravityScale;
	void print();
	void initWithXMLElement(XMLElement *p);
private:
	b2BodyType getBodyType(const string&str);

};
class CquadTexCoordInfo{
public:
	CCPoint tl,tr,bl,br;
	void print();
	void initWithXMLElement(XMLElement *p);
};
class CtexInfo{
public:
	string texImage;
	GLuint wrapS;
	GLuint wrapT;
	bool specifyTexCoord;
	CquadTexCoordInfo quadTexCoordInfo;
	void print();
	void initWithXMLElement(XMLElement *p);
private:
	GLuint getWrap(const string&str);
};
class CspriteInfo{
public:
	bool useTexRect;
	CCRect rect;
	CtexInfo texInfo;
	float zOrder;
	CCPoint anchorPoint;
	CCPoint pos;
	void print();
	void initWithXMLElement(XMLElement *p);
};
class CmyObjInfo{
public:
	string name;
	string sceneNodeType;
	int tag;
	vector<CbodyInfo> bodyInfoList;//Ԫ�ظ���Ϊ0��1
	CspriteInfo spriteInfo;
	void print();
	void initWithXMLElement(XMLElement *p);
	int getTag(const string&str);


};
class CmyObjGroupInfo{
public:
	vector<CmyObjInfo> myObjInfoList;
	void print();
	void initWithXMLElement(XMLElement *p);
};
#endif