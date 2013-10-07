
#include "xmlLoader_myObj.h"
//////////////////////////////////////////// CshapeInfo
	void CshapeInfo::print(){
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
	void CshapeInfo::initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="shape"){
			cout<<"error:����shape�ڵ�!"<<endl;
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
			cout<<"error:δ֪����!"<<endl;
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
			//���vertices�ڵ�û�����ݣ���p->FirstChildElement("vertices")->GetText()����NULL,
			//ֱ�ӽ�NULL����string���������������Ҫ��һ��toStringתһ�£������ǿյ�char*ת��string
			//��NULLת�ɿ�string
			vector<float> valist=strToValueList(toString(p->FirstChildElement("vertices")->GetText()));
			int nvalue=(int)valist.size();
			if(nvalue%2!=0){
				cout<<"error:��ֵ������Ϊż��!"<<endl;
				exit(0);
			}
			for(int i=0;i<nvalue;i+=2){
				b2Vec2 v(valist[i]/PTM_RATIO,valist[i+1]/PTM_RATIO);
				vertices.push_back(v);
			}
		}
	}

//////////////////////////////////////////////// CfilterInfo
	void CfilterInfo::print(){

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
	void CfilterInfo::initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="filter"){
			cout<<"error:����filter�ڵ�!"<<endl;
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

/////////////////////////////////////////////// CfixtureInfo

	void CfixtureInfo::print(){
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
	void CfixtureInfo::initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="fixture"){
			cout<<"error:����fixture�ڵ�!"<<endl;
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
/////////////////////////////////////// CbodyInfo

	void CbodyInfo::print(){
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
	void CbodyInfo::initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="body"){
			cout<<"error:����body�ڵ�!"<<endl;
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

	b2BodyType CbodyInfo::getBodyType(const string&str){
		if(str=="staticBody"){
			return b2_staticBody;
		}else if(str=="kinematicBody"){
			return b2_kinematicBody;
		}else if(str=="dynamicBody"){
			return b2_dynamicBody;
		}else{
			cout<<"error:δ֪����"<<endl;
			exit(0);
		}
	}

////////////////////////////////////////////// CquadTexCoordInfo
	void CquadTexCoordInfo::print(){
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
	void CquadTexCoordInfo::initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="quadTexCoord"){
			cout<<"error:����quadTexCoord�ڵ�!"<<endl;
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

////////////////////////////////////////////////// CtexInfo
	void CtexInfo::print(){
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
	void CtexInfo::initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="tex"){
			cout<<"error:����tex�ڵ�!"<<endl;
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

	GLuint CtexInfo::getWrap(const string&str){
		if(str=="repeat"){
			return GL_REPEAT;
		}else if(str=="clamp_to_edge"){
			return GL_CLAMP_TO_EDGE;
		}else if(str=="mirrored_repeat"){
			return GL_MIRRORED_REPEAT;
		}else{
			cout<<"error:δ֪����!"<<endl;
			exit(0);
		}

	}
///////////////////////////////////////////// CspriteInfo
	void CspriteInfo::print(){
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
	void CspriteInfo::initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="sprite"){
			cout<<"error:����sprite�ڵ�!"<<endl;
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

/////////////////////////////////////////// CmyObjInfo

	void CmyObjInfo::print(){
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
	void CmyObjInfo::initWithXMLElement(XMLElement *p){
		if(toString(p->Name())!="myObj"){
			cout<<"error:����myObj�ڵ�!"<<endl;
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
				cout<<"error:����myObj��body�ڵ�������ô���1!"<<endl;
				exit(0);
			}

		}
		//sprite
		spriteInfo.initWithXMLElement(p->FirstChildElement("sprite"));
	}
	int CmyObjInfo::getTag(const string&str){
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
			cout<<"error:δ֪����!"<<endl;
			//    cout<<str<<endl;
			exit(0);
		}

	}

//////////////////////////////////////// CmyObjListInfo

	void CmyObjGroupInfo::print(){
		int nMyObj=(int)myObjInfoList.size();
		for(int i=0;i<nMyObj;i++){
			myObjInfoList[i].print();
		}
	}
	void CmyObjGroupInfo::initWithXMLElement(XMLElement *p){//p��ҪmyObjList�ڵ�
		if(toString(p->Name())!="myObjList"){
			cout<<"error:����myObjList�ڵ�!"<<endl;
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