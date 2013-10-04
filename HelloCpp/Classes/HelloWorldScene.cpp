//
//  HelloWorldScene.cpp
//  mybox2dtest
//
//  Created by apple on 13-7-4.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#include <vector>
#include <string>
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "CCControlButton.h"
#include "myContactListener.h"
#include "myDef.h"
#include "player.h"
#include "myBox2dFunc.h"
#include "sceneRootNode.h"
#include "tinyxml2.h"
#include "myfunc.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace tinyxml2;
using namespace std;




Cmouse mos;



HelloWorld::HelloWorld()

{
   
	
    


    //----
    m_waterSurfaceHeight=0;
    //----
    setTouchEnabled( true );
    setAccelerometerEnabled( true );
  //  CCDirector::sharedDirector()->setDepthTest(true);
   // glClearColor(0, 0, 1, 1);
    CCDirector::sharedDirector()->setDisplayStats(false);//不显示帧率等信息

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();

    //----创建物理world
    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);
    world = new b2World(gravity);
    world->SetAllowSleeping(true); // Do we want to let bodies sleep?
    world->SetContinuousPhysics(true);
    world->SetContactListener(&myContactListener);
	//----创建卷动场景node
	CCNode *node_rolling=CCNode::create();
	addChild(node_rolling,0,tag_root_scene_rolling);

    //----创建myObj场景
    CsceneRootNode *node =new CsceneRootNode();
    node->set_isShowBodyWireFrame(false);//是否显示box2d线框
    node_rolling->addChild(node, 0, tag_root_scene_rolling_myObj);
	
    //----创建无卷动场景node
    CsceneRootNode *node_nonRolling= new CsceneRootNode();
    addChild(node_nonRolling,-10,tag_root_scene_nonRolling);
    //----创建UI node
    CCNode*node_ui=CCNode::create();
    addChild(node_ui,0,tag_root_ui);
    //----根据tiled map创建场景
	{
		CCPoint mapOffset(0,-100);//地图偏移量
		CCTMXTiledMap*map=CCTMXTiledMap::create("data/levels/level1/scene/map/map.tmx");
		node_rolling->addChild(map,0,tag_tiledMap);
		map->setPosition(mapOffset);
		map->setVisible(false);
		//获得map信息
		CCSize mapSize=map->getMapSize();//s.height是layer行数，s.width是layer列数
		CCSize tileSize=map->getTileSize();//每个tile的大小
		//获得层或objGroup
		CCTMXLayer*layer=map->layerNamed("groundLayer");
		assert(layer);
		CCTMXObjectGroup* objGroup=map->objectGroupNamed("thingLayer_polygon");
		assert(objGroup);
		CCTMXLayer*layer_thing_tile=map->layerNamed("thingLayer_tile");
		assert(layer_thing_tile);
		//遍历objGroup中的各object
		CCObject* pObj = NULL;
        CCARRAY_FOREACH(objGroup->getObjects(), pObj)
        {
			//----属性
			string name;
			string type;//polygon,ellipse,...
			b2BodyType bodyType;
			float friction;
			float restitution;
			float density;
			bool isSensor;
			int categoryBits;
			int maskBits;
			int groupIndex;
			float gravityScale;
			//----获得属性
            CCDictionary* pDict = (CCDictionary*)pObj;
			name= ((CCString*)pDict->objectForKey("name"))->getCString();
			type= ((CCString*)pDict->objectForKey("type"))->getCString();
		    bodyType=bodyTypeStrTobodyTypeValue(((CCString*)pDict->objectForKey("bodyType"))->getCString());
			friction= ((CCString*)pDict->objectForKey("friction"))->floatValue();
			restitution= ((CCString*)pDict->objectForKey("restitution"))->floatValue();
			density= ((CCString*)pDict->objectForKey("density"))->floatValue();
			isSensor= ((CCString*)pDict->objectForKey("isSensor"))->boolValue();
			categoryBits= ((CCString*)pDict->objectForKey("categoryBits"))->intValue();
			maskBits= ((CCString*)pDict->objectForKey("maskBits"))->intValue();
			groupIndex= ((CCString*)pDict->objectForKey("groupIndex"))->intValue();
			gravityScale= ((CCString*)pDict->objectForKey("gravityScale"))->floatValue();
			cout<<"name:"<<name<<endl;
			cout<<"categoryBits:"<<categoryBits<<endl;
			cout<<"maskBits:"<<maskBits<<endl;
			cout<<"groupIndex:"<<groupIndex<<endl;
			//根据type不同，作不同处理
			if(type=="ellipse"){//是椭圆
				//x,y是椭圆包围盒的左下角
				float x = pDict->valueForKey("x")->floatValue();
				float y = pDict->valueForKey("y")->floatValue();//此值与tmx文件中存的值不同，需用mapSize.height*tileSize.height减去此值才得tmx文件中的值
				cout<<"xy:"<<x<<","<<y<<endl;
				float width = pDict->valueForKey("width")->floatValue();
				float height = pDict->valueForKey("height")->floatValue();
				cout<<"wh:"<<width<<" "<<height<<endl;
				//求圆心
				float cx_inWorld_b2d=(x+width/2+map->getPositionX())/PTM_RATIO;
				float cy_inWorld_b2d=(y+height/2+map->getPositionY())/PTM_RATIO;
				//求半径
				float r_b2d=(width/2+height/2)/2/PTM_RATIO;
				//--------生成myObj
				CmyObj*myObj=CmyObj::create(world);
				//----生成body
				myObj->create_pb2Body_circle(r_b2d,bodyType,friction,restitution,density,isSensor,categoryBits,maskBits,groupIndex,gravityScale);
				myObj->pb2Body->SetTransform(b2Vec2(cx_inWorld_b2d ,cy_inWorld_b2d),0);
				//----生成sprite
				//求layer_thing_tile中所有与myObj->pb2Body相交的块
				//获得shape
				b2CircleShape*shape=(b2CircleShape*)myObj->pb2Body->GetFixtureList()->GetShape();
				//求shape的变换矩阵
				b2Transform transform=myObj->pb2Body->GetTransform();
				//求shape变换后的aabb
				b2AABB aabb;
				{
					int32 childIndex = 0;
					shape->ComputeAABB(&aabb, transform, childIndex);  
				}
				//将aabb转化为ccrect
				CCRect rect(aabb.lowerBound.x*PTM_RATIO,aabb.lowerBound.y*PTM_RATIO,
					(aabb.upperBound.x-aabb.lowerBound.x)*PTM_RATIO,(aabb.upperBound.y-aabb.lowerBound.y)*PTM_RATIO);
				//求layer_thing_tile中所有与rect相交的块
				//求rect在map中（以map左下角为原点）的样子
				CCRect rect_inMap_OatLD=rect;
				rect_inMap_OatLD.origin=rect_inMap_OatLD.origin-map->getPosition();
				//求rect在map中（以map左上角为原点）的样子(注意此时rect的origin要改作用rect的左上角)
				CCRect rect_inMap_OatLU=rect_inMap_OatLD;
				rect_inMap_OatLU.origin.y=rect_inMap_OatLU.origin.y+rect_inMap_OatLU.size.height;//原点改用rect左上角，但map原点仍取map左下角
				rect_inMap_OatLU.origin.y=mapSize.height*tileSize.height-rect_inMap_OatLU.origin.y;//将map原点改用左上角，得到最终的rect_inMap_OatLU

				float xmin=rect_inMap_OatLU.getMinX();
				float ymin=rect_inMap_OatLU.getMinY();
				float xmax=rect_inMap_OatLU.getMaxX();
				float ymax=rect_inMap_OatLU.getMaxY();
				int jmin=xmin/tileSize.width;
				int imin=ymin/tileSize.height;
				int jmax=xmax/tileSize.width;
				int imax=ymax/tileSize.height;
				cout<<"i range:"<<imin<<" "<<imax<<endl;
				cout<<"j range:"<<jmin<<" "<<jmax<<endl;
				//将layer_thing_tile的(imin,jmin)至(imax,jmax)区域内各格的sprite的拷贝均加为myObj的子节点（之所以要用拷贝，是因为这些sprite已经通过map加入到场景了，不能重复添加到场景）
				for(int i=imin;i<=imax;i++){
					for(int j=jmin;j<=jmax;j++){
						CCSprite*sprite=layer_thing_tile->tileAt(ccp(j,i));//注意，j为横，i为纵，所以ccp(j,i)
						if(sprite==NULL)continue;
						CCSprite*_sprite=CCSprite::create();
						_sprite->setTexture(sprite->getTexture());
						_sprite->setTextureRect(sprite->getTextureRect());
						_sprite->setAnchorPoint(sprite->getAnchorPoint());
						//求sprite相对于body的坐标
						b2Vec2 bodyPos_inWorld_box2d=myObj->pb2Body->GetPosition();
						CCPoint bodyPos_inWorld(bodyPos_inWorld_box2d.x*PTM_RATIO,bodyPos_inWorld_box2d.y*PTM_RATIO);
						CCPoint spritePos_inWorld=sprite->getPosition()+map->getPosition();
						CCPoint offset_spriteRelativeToBody=spritePos_inWorld-bodyPos_inWorld;
						_sprite->setPosition(offset_spriteRelativeToBody);//转化为相对于
						myObj->addChild(_sprite);
					}
				}
				this->getChildByTag(tag_root_scene_rolling)->getChildByTag(tag_root_scene_rolling_myObj)->addChild(myObj);

			}else if(type=="polygon"){//是多边形
				float x = pDict->valueForKey("x")->floatValue();
				float y = pDict->valueForKey("y")->floatValue();//此值与tmx文件中存的值不同，需用mapSize.height*tileSize.height减去此值才得tmx文件中的值
				cout<<"xy:"<<x<<","<<y<<endl;
				CCArray* points = (CCArray*)pDict->objectForKey("points");//得到顶点列表
				assert(points);
				//生成polygon
				vector<b2Vec2> polygon;
				int npoint=points->count();
				float begx=x;
				float begy=mapSize.height*tileSize.height-y;
				for(int i=0;i<npoint;i++){
					CCDictionary* dict = (CCDictionary*)points->objectAtIndex(i);
					float x_inMap=begx+ dict->valueForKey("x")->floatValue();
					float y_inMap=mapSize.height*tileSize.height-(begy+ dict->valueForKey("y")->floatValue());
					//(x_inMap,y_inMap)是以map左下角为原点的坐标，下面加上map偏移，即得在世界中的坐标
					float x_inWorld=map->getPositionX()+x_inMap;
					float y_inWorld=map->getPositionY()+y_inMap;
					b2Vec2 v(x_inWorld/PTM_RATIO,y_inWorld/PTM_RATIO);
					polygon.push_back(v);
				}//得到polygon
				//求polygon的重心
				b2Vec2 wc(0,0);//重心,一定要初始化，否则会是随机值
				for(int i=0;i<npoint;i++){
					const b2Vec2&v=polygon[i];
					wc+=v;
				}
				wc*=1.0/npoint;//得到wc
				//将polygon所有顶点均转为相对于wc
				for(int i=0;i<npoint;i++){
					b2Vec2&v=polygon[i];
					v-=wc;
				}
				//--------生成myObj
				CmyObj*myObj=CmyObj::create(world);
				//----生成body
				myObj->create_pb2Body_polygon(polygon,bodyType,friction,restitution,density,isSensor,categoryBits,maskBits,groupIndex,gravityScale);
				myObj->pb2Body->SetTransform(b2Vec2(wc.x ,wc.y),0);
				//----生成sprite
				//求layer_thing_tile中所有与myObj->pb2Body相交的块
				//获得shape
				b2PolygonShape*shape=(b2PolygonShape*)myObj->pb2Body->GetFixtureList()->GetShape();
				//求shape的变换矩阵
				b2Transform transform=myObj->pb2Body->GetTransform();
				//求shape变换后的aabb
				b2AABB aabb;
				{
					int32 childIndex = 0;
					shape->ComputeAABB(&aabb, transform, childIndex);  
				}
				//将aabb转化为ccrect
				CCRect rect(aabb.lowerBound.x*PTM_RATIO,aabb.lowerBound.y*PTM_RATIO,
					(aabb.upperBound.x-aabb.lowerBound.x)*PTM_RATIO,(aabb.upperBound.y-aabb.lowerBound.y)*PTM_RATIO);
				//求layer_thing_tile中所有与rect相交的块
				//求rect在map中（以map左下角为原点）的样子
				CCRect rect_inMap_OatLD=rect;
				rect_inMap_OatLD.origin=rect_inMap_OatLD.origin-map->getPosition();
				//求rect在map中（以map左上角为原点）的样子(注意此时rect的origin要改作用rect的左上角)
				CCRect rect_inMap_OatLU=rect_inMap_OatLD;
				rect_inMap_OatLU.origin.y=rect_inMap_OatLU.origin.y+rect_inMap_OatLU.size.height;//原点改用rect左上角，但map原点仍取map左下角
				rect_inMap_OatLU.origin.y=mapSize.height*tileSize.height-rect_inMap_OatLU.origin.y;//将map原点改用左上角，得到最终的rect_inMap_OatLU

				float xmin=rect_inMap_OatLU.getMinX();
				float ymin=rect_inMap_OatLU.getMinY();
				float xmax=rect_inMap_OatLU.getMaxX();
				float ymax=rect_inMap_OatLU.getMaxY();
				int jmin=xmin/tileSize.width;
				int imin=ymin/tileSize.height;
				int jmax=xmax/tileSize.width;
				int imax=ymax/tileSize.height;
				cout<<"i range:"<<imin<<" "<<imax<<endl;
				cout<<"j range:"<<jmin<<" "<<jmax<<endl;
				//将layer_thing_tile的(imin,jmin)至(imax,jmax)区域内各格的sprite的拷贝均加为myObj的子节点（之所以要用拷贝，是因为这些sprite已经通过map加入到场景了，不能重复添加到场景）
				CCSpriteBatchNode*batchNode_small=CCSpriteBatchNode::createWithTexture(layer_thing_tile->getTexture());
				myObj->addChild(batchNode_small);
				for(int i=imin;i<=imax;i++){
					for(int j=jmin;j<=jmax;j++){
						CCSprite*sprite=layer_thing_tile->tileAt(ccp(j,i));//注意，j为横，i为纵，所以ccp(j,i)
						if(sprite==NULL)continue;
						CCSprite*_sprite=CCSprite::create();
						_sprite->setTexture(sprite->getTexture());
						_sprite->setTextureRect(sprite->getTextureRect());
						_sprite->setAnchorPoint(sprite->getAnchorPoint());
						//求sprite相对于body的坐标
						b2Vec2 bodyPos_inWorld_box2d=myObj->pb2Body->GetPosition();
						CCPoint bodyPos_inWorld(bodyPos_inWorld_box2d.x*PTM_RATIO,bodyPos_inWorld_box2d.y*PTM_RATIO);
						CCPoint spritePos_inWorld=sprite->getPosition()+map->getPosition();
						CCPoint offset_spriteRelativeToBody=spritePos_inWorld-bodyPos_inWorld;
						_sprite->setPosition(offset_spriteRelativeToBody);//转化为相对于
						batchNode_small->addChild(_sprite);
					}
				}
				this->getChildByTag(tag_root_scene_rolling)->getChildByTag(tag_root_scene_rolling_myObj)->addChild(myObj);

			
			}
		


        }
		//标记各格是否有内容
		vector<vector<bool> > mat;
		mat.resize(mapSize.height);
		for(int i=0;i<mapSize.height;i++){
			mat[i].resize(mapSize.width);
			for(int j=0;j<mapSize.width;j++){
				mat[i][j]=false;
			}
		}
		//填充mat
		for(int i=0;i<mapSize.height;i++){
			for(int j=0;j<mapSize.width;j++){
				CCPoint gridxy(j,i);
				int grid=layer->tileGIDAt(gridxy);
				CCDictionary* poperDic=(CCDictionary*)map->propertiesForGID(grid);
				//检查isNoContent
				if(poperDic!=NULL){
					CCObject*ccobj=poperDic->objectForKey("nothing");
					if(ccobj!=NULL){
						CCString *ccs_value=(CCString*)ccobj;
						string str_value=ccs_value->getCString();
						if(str_value=="true")continue;
					}
				}
				//获得sprite
				CCSprite*sprite=layer->tileAt(ccp(j,i));//注意，j为横，i为纵，所以ccp(j,i)
				if(sprite==NULL)continue;
				mat[i][j]=true;

			}
		}
		//标记各格是否在边缘
		vector<vector<bool> > mat_isOnEdge;
		mat_isOnEdge.resize(mapSize.height);
		for(int i=0;i<mapSize.height;i++){
			mat_isOnEdge[i].resize(mapSize.width);
			for(int j=0;j<mapSize.width;j++){
				mat_isOnEdge[i][j]=false;
			}
		}
		//填充mat_isOnEdge
		for(int i=0;i<(int)mat_isOnEdge.size();i++)
		{
			for(int j=0;j<(int)mat_isOnEdge[i].size();j++)
			{
				if(mat[i][j]==true){
					//判断mat[i][j]是否为边缘格
					int up=i-1>0?mat[i-1][j]:0;
					int dn=i+1<(int)mat.size()?mat[i+1][j]:0;
					int left=j-1>0?mat[i][j-1]:0;
					int right=j+1<(int)mat[i].size()?mat[i][j+1]:0;
					if(up!=0&&dn!=0&&left!=0&&right!=0){//是非边缘格
						//无动作
					}else{//是边缘格
						mat_isOnEdge[i][j]=true;
					}
				}
			}
		}
		//记录各格的polygon都覆盖除自身以外的哪些格
		vector<vector<vector<Cij> > > mat_polygonCoveredTileSet;
		mat_polygonCoveredTileSet.resize(mapSize.height);
		for(int i=0;i<mapSize.height;i++){
			mat_polygonCoveredTileSet[i].resize(mapSize.width);
		}
		//填充mat_polygonCoveredTileSet
		for(int i=0;i<(int)mat_polygonCoveredTileSet.size();i++)
		{
			for(int j=0;j<(int)mat_polygonCoveredTileSet[i].size();j++)
			{
				int grid=layer->tileGIDAt(ccp(j,i));//因为ccp，所以j,i
				CCDictionary* poperDic=(CCDictionary*)map->propertiesForGID(grid);
				if(poperDic!=NULL){
					CCObject*ccobj=poperDic->objectForKey("polygon");
					if(ccobj!=NULL){
						CCString *ccs_value=(CCString*)ccobj;
						string str=ccs_value->getCString();
						if(str.substr(0,3)=="use"){
							string valueListStr=str.substr(3,-1);
							vector<float> valueList=strToValueList(valueListStr);
							assert((int)valueList.size()==2);
							float dix=valueList[0];
							float diy=valueList[1];
							mat_polygonCoveredTileSet[i+diy][j+dix].push_back(Cij(i,j));
						}
					}
				}
			}
		}

		//生成myObjs

		CCSpriteBatchNode*batchNode=CCSpriteBatchNode::createWithTexture(layer->getTexture());//此batchNode用于存放非边缘块
		this->getChildByTag(tag_root_scene_rolling)->addChild(batchNode);

		for(int i=0;i<mapSize.height;i++)
		{
			for(int j=0;j<mapSize.width;j++)
			{	
				if(mat_isOnEdge[i][j]==true){//是边缘块		
					CCPoint gridxy(j,i);
					int grid=layer->tileGIDAt(gridxy);
					CCDictionary* poperDic=(CCDictionary*)map->propertiesForGID(grid);
					CCSprite*sprite=layer->tileAt(ccp(j,i));
					//----将sprite做成myObj
					CmyObj*pmyObj=CmyObj::create(world);
					CCSize s=sprite->getContentSize();
					CCPoint p=sprite->convertToWorldSpace(ccp(0,0));
					//生成多边形
					vector<b2Vec2> polygon;
					bool polygonGot=false;
					if(poperDic!=NULL){
						CCObject*ccobj=poperDic->objectForKey("polygon");
						if(ccobj!=NULL){
							CCString *ccs_value=(CCString*)ccobj;
							string str=ccs_value->getCString();
							if(str.substr(0,3)=="use")continue;
							vector<float> valueList=strToValueList(str);
							int nvalue=(int)valueList.size();
							for(int k=0;k<nvalue;k+=2){
								float x=valueList[k];
								float y=valueList[k+1];
								x*=tileSize.width/PTM_RATIO;
								y*=tileSize.height/PTM_RATIO;
								b2Vec2 v=b2Vec2(x,y);
								polygon.push_back(v);
							}//得到polygon
							polygonGot=true;
						}
					}
					if(polygonGot==false){//直接生成矩形
						if((int)polygon.size()!=0){
							cout<<"error:polygon.size()!=0"<<endl;
							exit(0);
						}
						float w=tileSize.width/PTM_RATIO;
						float h=tileSize.height/PTM_RATIO;
						b2Vec2 v0=b2Vec2(0,0);
						b2Vec2 v1=b2Vec2(w,0);
						b2Vec2 v2=b2Vec2(w,h);
						b2Vec2 v3=b2Vec2(0,h);
						polygon.push_back(v0);
						polygon.push_back(v1);
						polygon.push_back(v2);
						polygon.push_back(v3);
					
					}
					b2Filter filter;
					{
						filter.categoryBits=1;
						filter.maskBits=65535;
						filter.groupIndex=0;
					}
					pmyObj->create_pb2Body_polygon(polygon,b2_staticBody,0.5,&filter);
					pmyObj->pb2Body->SetTransform(b2Vec2(p.x/PTM_RATIO ,p.y/PTM_RATIO),0);
					
					//----用sprite生成myObj的sprite
					//求pmyObj的polygon覆盖除自身外哪些格子
					vector<Cij> polygonCoveredTileSet=mat_polygonCoveredTileSet[i][j];
					//将自身也加入其中
					polygonCoveredTileSet.push_back(Cij(i,j));
					int nCoveredTile=(int)polygonCoveredTileSet.size();
					//将polygonCoveredTileSet中所有格子的sprite添加为myObj的子节点(由于这些sprite已通过map添加到了场景，所以不能重复添加，只能制作副本再添加)
					CCSpriteBatchNode*batchNode_small=CCSpriteBatchNode::createWithTexture(layer->getTexture());
					pmyObj->addChild(batchNode_small);
					for(int k=0;k<nCoveredTile;k++){
						int I=polygonCoveredTileSet[k].i;
						int J=polygonCoveredTileSet[k].j;
						CCSprite*sprite=layer->tileAt(ccp(J,I));
						if(sprite==NULL)continue;
						CCSprite*_sprite=CCSprite::create();
						_sprite->setTexture(sprite->getTexture());
						_sprite->setTextureRect(sprite->getTextureRect());
						_sprite->setAnchorPoint(sprite->getAnchorPoint());
						//求sprite相对于body的坐标
						b2Vec2 bodyPos_inWorld_box2d=pmyObj->pb2Body->GetPosition();
						CCPoint bodyPos_inWorld(bodyPos_inWorld_box2d.x*PTM_RATIO,bodyPos_inWorld_box2d.y*PTM_RATIO);
						CCPoint spritePos_inWorld=sprite->getPosition()+map->getPosition();
						CCPoint offset_spriteRelativeToBody=spritePos_inWorld-bodyPos_inWorld;
						_sprite->setPosition(offset_spriteRelativeToBody);//转化为相对于
						batchNode_small->addChild(_sprite);
					}
					this->getChildByTag(tag_root_scene_rolling)->getChildByTag(tag_root_scene_rolling_myObj)->addChild(pmyObj);
				}else{//不是边缘块
					if(mat[i][j]){
						CCSprite*sprite=layer->tileAt(ccp(j,i));
						assert(sprite);
						//由于sprite已通过map加入到了场景，所以不能重复添加，只能复制一份再添加
						CCSprite*_sprite=CCSprite::create();
						_sprite->setTexture(sprite->getTexture());
						_sprite->setTextureRect(sprite->getTextureRect());
						_sprite->setAnchorPoint(sprite->getAnchorPoint());
						_sprite->setPosition(sprite->getPosition()+map->getPosition());
						batchNode->addChild(_sprite);
					}
				}

			}
		}
		//map已经没用了，销毁map
		map->removeFromParentAndCleanup(true);
		
	}
    //----根据xml创建对象
    {
        //场景
        {
            vector<CmyObj*> objList=CmyObj::createObjsWithXML(world,"data/levels/level1/scene/objGroup.xml");
            int nObj=(int)objList.size();
            //加入到场景树
            for(int i=0;i<nObj;i++){
                CmyObj*obj=objList[i];
                if(obj->sceneNodeType=="scene_rolling"){
                    node->addChild(obj);
                }else if(obj->sceneNodeType=="scene_nonRolling"){
                    node_nonRolling->addChild(obj);
                }else if(obj->sceneNodeType=="ui"){
                    node_ui->addChild(obj);
                }else{
                    cout<<"error:未知类型!"<<endl;
                    exit(0);
                }
            }
        }
        //子弹模板(向右)
        {
            string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("data/global/objs/shotBall_right_template/objGroup.xml");
            tinyxml2::XMLDocument doc;
            doc.LoadFile(fullPath.c_str());
            XMLElement *root=doc.RootElement();
            CmyObjGroupInfo objInfoList;
            objInfoList.initWithXMLElement(root);
            if((int)objInfoList.myObjInfoList.size()!=1){
                cout<<"error:obj个数不等于1!"<<endl;
                exit(0);
            }
            this->objInfo_shotBall_right=objInfoList.myObjInfoList[0];
        }
        //子弹模板(向左)
        {
            string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("data/global/objs/shotBall_left_template/objGroup.xml");
            tinyxml2::XMLDocument doc;
            doc.LoadFile(fullPath.c_str());
            XMLElement *root=doc.RootElement();
            CmyObjGroupInfo objInfoList;
            objInfoList.initWithXMLElement(root);
            if((int)objInfoList.myObjInfoList.size()!=1){
                cout<<"error:obj个数不等于1!"<<endl;
                exit(0);
            }
            this->objInfo_shotBall_left=objInfoList.myObjInfoList[0];
        }


    }
    //----创建player
    //创建myObj
    const float rx=64;
    const float ry=64;
    CCPoint p(300, 300+300);
    pPlayer = Cplayer::create(world);
    pPlayer->tex_still_right=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/stand_right.png");
    pPlayer->tex_still_left=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/stand_left.png");
    pPlayer->tex_jump_still_right=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/jump_right.png");
    pPlayer->tex_jump_still_left=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/jump_left.png");
    pPlayer->tex_jump_forward_right=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/jump_right.png");
    pPlayer->tex_jump_forward_left=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/jump_left.png");
    pPlayer->tex_swim_still_right=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/stand_right.png");
    pPlayer->tex_swim_still_left=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/stand_left.png");
    pPlayer->tex_forward_right_first=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/run_right_first.png");
    pPlayer->tex_forward_left_first=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/run_left_first.png");
    pPlayer->initWithTexture(pPlayer->tex_still_right, CCRectMake( 0,0,2*rx,2*ry));

    b2Filter filter;
    filter.categoryBits=0x0004;//必须只有一个二进制位为1
    filter.maskBits=0xFFFF & ~0x0008;
    pPlayer->create_pb2Body(world,rx/PTM_RATIO,ry/PTM_RATIO,&filter);
 
    pPlayer->setZOrder(100);
    node->addChild(pPlayer,100);

    //设定位置
    pPlayer->setPosition( CCPointMake( p.x, p.y) );
    pPlayer->pb2Body->SetTransform(b2Vec2(p.x/PTM_RATIO, p.y/PTM_RATIO), 0);
    pPlayer->pb2Body->SetType(b2_dynamicBody);
    pPlayer->pb2Body->SetFixedRotation(true);
    pPlayer->setTag(tag_player);
    //tex
    pPlayer->tex_shotBall_left=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/shotBall.png");
    pPlayer->tex_shotBall_right=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/shotBall.png");
    //----定义控件
    //--label
    CCLabelTTF *label = CCLabelTTF::create("Hellow COCO -- demo 1.0 -- by wantnon", "Helvetica", 32);
    addChild(label, 0);
    label->setColor(ccc3(0,0,255));
    label->setPosition(ccp( winSize.width/2, winSize.height-50));
    //--按钮left
    {
        cocos2d::extension::CCScale9Sprite* btnUp=cocos2d::extension::CCScale9Sprite::create("data/global/tex/button.png");
        cocos2d::extension::CCScale9Sprite* btnDn=cocos2d::extension::CCScale9Sprite::create("data/global/tex/button_dn.png");
        CCLabelTTF*title=CCLabelTTF::create("<--", "Helvetica", 30);
        cocos2d::extension::CCControlButton* controlButton=cocos2d::extension::CCControlButton::create(title, btnUp);
        controlButton->setBackgroundSpriteForState(btnDn,cocos2d::extension::CCControlStateHighlighted);
        controlButton->setPreferredSize(CCSize(100,100));
        controlButton->setPosition(ccp(120,60));
        controlButton->addTargetWithActionForControlEvents(this, (cocos2d::extension::SEL_CCControlHandler)(&HelloWorld::leftKeyDown), cocos2d::extension::CCControlEventTouchDown);
        controlButton->addTargetWithActionForControlEvents(this, (cocos2d::extension::SEL_CCControlHandler)(&HelloWorld::leftKeyUpInside), cocos2d::extension::CCControlEventTouchUpInside);
        controlButton->addTargetWithActionForControlEvents(this, (cocos2d::extension::SEL_CCControlHandler)(&HelloWorld::leftKeyDragEnter), cocos2d::extension::CCControlEventTouchDragEnter);
        controlButton->addTargetWithActionForControlEvents(this, (cocos2d::extension::SEL_CCControlHandler)(&HelloWorld::leftKeyDragExit), cocos2d::extension::CCControlEventTouchDragExit);
        this->getChildByTag(tag_root_ui)->addChild(controlButton);
    }
    //--按钮right
    {
        cocos2d::extension::CCScale9Sprite* btnUp=cocos2d::extension::CCScale9Sprite::create("data/global/tex/button.png");
        cocos2d::extension::CCScale9Sprite* btnDn=cocos2d::extension::CCScale9Sprite::create("data/global/tex/button_dn.png");
        CCLabelTTF*title=CCLabelTTF::create("-->", "Helvetica", 30);
        cocos2d::extension::CCControlButton* controlButton=cocos2d::extension::CCControlButton::create(title, btnUp);
        controlButton->setBackgroundSpriteForState(btnDn,cocos2d::extension::CCControlStateHighlighted);
        controlButton->setPreferredSize(CCSize(100,100));
        controlButton->setPosition(ccp(300,60));
        controlButton->addTargetWithActionForControlEvents(this, (cocos2d::extension::SEL_CCControlHandler)(&HelloWorld::rightKeyDown), cocos2d::extension::CCControlEventTouchDown);
        controlButton->addTargetWithActionForControlEvents(this, (cocos2d::extension::SEL_CCControlHandler)(&HelloWorld::rightKeyUpInside), cocos2d::extension::CCControlEventTouchUpInside);
        controlButton->addTargetWithActionForControlEvents(this, (cocos2d::extension::SEL_CCControlHandler)(&HelloWorld::rightKeyDragEnter), cocos2d::extension::CCControlEventTouchDragEnter);
        controlButton->addTargetWithActionForControlEvents(this, (cocos2d::extension::SEL_CCControlHandler)(&HelloWorld::rightKeyDragExit), cocos2d::extension::CCControlEventTouchDragExit);
        this->getChildByTag(tag_root_ui)->addChild(controlButton);
    }
    //--按钮jump
    {
        cocos2d::extension::CCScale9Sprite* btnUp=cocos2d::extension::CCScale9Sprite::create("data/global/tex/button.png");
        cocos2d::extension::CCScale9Sprite* btnDn=cocos2d::extension::CCScale9Sprite::create("data/global/tex/button_dn.png");
        CCLabelTTF*title=CCLabelTTF::create("jump", "Helvetica", 30);
        cocos2d::extension::CCControlButton* controlButton=cocos2d::extension::CCControlButton::create(title, btnUp);
        controlButton->setBackgroundSpriteForState(btnDn,cocos2d::extension::CCControlStateHighlighted);
        controlButton->setPreferredSize(CCSize(100,100));
        controlButton->setPosition(ccp(850,60));
        controlButton->addTargetWithActionForControlEvents(this, (cocos2d::extension::SEL_CCControlHandler)(&HelloWorld::jumpKeyDown), cocos2d::extension::CCControlEventTouchDown);
       
        this->getChildByTag(tag_root_ui)->addChild(controlButton);
    }
    //--按钮shot
    {
        cocos2d::extension::CCScale9Sprite* btnUp=cocos2d::extension::CCScale9Sprite::create("data/global/tex/button.png");
        cocos2d::extension::CCScale9Sprite* btnDn=cocos2d::extension::CCScale9Sprite::create("data/global/tex/button_dn.png");
        CCLabelTTF*title=CCLabelTTF::create("shot", "Helvetica", 30);
        cocos2d::extension::CCControlButton* controlButton=cocos2d::extension::CCControlButton::create(title, btnUp);
        controlButton->setBackgroundSpriteForState(btnDn,cocos2d::extension::CCControlStateHighlighted);
        controlButton->setPreferredSize(CCSize(100,100));
        controlButton->setPosition(ccp(850,200));
        controlButton->addTargetWithActionForControlEvents(this, (cocos2d::extension::SEL_CCControlHandler)(&HelloWorld::shotKeyDown), cocos2d::extension::CCControlEventTouchDown);
        
        this->getChildByTag(tag_root_ui)->addChild(controlButton);
    }
	/*
    //----定义悬挂点
    const float32 y = 640.0/PTM_RATIO;//15.0f;
    const float32 x= 1136.0/PTM_RATIO;
    //创建myObj 
    CmyObj *pinnerObj=NULL;
    
    {
        // Define the  body.
        b2BodyDef BodyDef;
        BodyDef.position.Set(x/2, y/2); // bottom-left corner
        
        BodyDef.type = b2_staticBody;
        
        // Call the body factory which allocates memory for the ground body
        // from a pool and creates the ground box shape (also from a pool).
        // The body is also added to the world.
        b2Body* Body = world->CreateBody(&BodyDef);
        
        
        const float w=0.5;
        const float h=0.5;
        
        b2PolygonShape gBox;
        gBox.SetAsBox(w,h);//These are mid points for our 1m box
        
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &gBox;
        fixtureDef.density =10.0f;
        fixtureDef.friction = 0.3f;
        Body->CreateFixture(&fixtureDef);
        
        //创建绘图节点
        CCNode* node = getChildByTag(tag_root_scene_rolling);
        pinnerObj= CmyObj::create(world);
        CCTexture2D*texture=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/hungPoint.png");
        pinnerObj->initWithTexture(texture, CCRectMake( 0,0,w*2*PTM_RATIO,h*2*PTM_RATIO));
        pinnerObj->set_quadTexCoords(ccp(0,0), ccp(1,0),
                                  ccp(0,1), ccp(1,1));
        
        pinnerObj->pb2Body=Body;
        pinnerObj->pb2Body->SetUserData(pinnerObj);
        node->addChild(pinnerObj);
        
    }

    //----定义绳子
    b2RopeJointDef m_ropeDef;
    b2Joint* m_rope;
    {
        const float halfSegLen=0.5f;
        const float halfSegThick=0.25f;//0.125f;
        const float halfBoxW=1.5f;
        const float halfBoxH=1.5f;
        
        b2PolygonShape shape;
        shape.SetAsBox(halfSegLen, halfSegThick);//一节绳子
        
        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = 40.0f;//各节绳子质量不能太小，否则容易被打散
        fd.friction = 0;//0.2f;
    //    fd.filter.categoryBits = 0x0001;
     //   fd.filter.maskBits = 0xFFFF & ~0x0002;//唯独不与0x0002碰撞
        
        b2RevoluteJointDef jd;
        jd.collideConnected = false;
        
        const int32 N = 10;

        m_ropeDef.localAnchorA.Set(0,0);//(x/2,y/2);//(0.0f, y);
        
        vector<float> halfWList;
        vector<float> halfHList;
        for(int i=0;i<N;i++){
            if(i==N-1){
                halfWList.push_back(halfBoxW);
                halfHList.push_back(halfBoxH);
            }else{
                halfWList.push_back(halfSegLen);
                halfHList.push_back(halfSegThick);
            }
        }
        
        b2Body* prevBody = pinnerObj->pb2Body;
        for (int32 i = 0; i < N; ++i)
        {
            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.position.Set(x/2+0.5f + 1.0f * i, y/2);
            if (i == N - 1)
            {
                shape.SetAsBox(1.5f, 1.5f);
                fd.density = 6.0f;
                fd.friction=0.8;
            //    fd.filter.categoryBits = 0x0002;
            //    fd.filter.maskBits = 0xFFFF;
                bd.position.Set(x/2+1.0f * i+1.5f, y/2);
                bd.angularDamping = 0.4f;//角衰减，防止转动过快
            }

            
            b2Body* body = world->CreateBody(&bd);
            
            body->CreateFixture(&fd);
            
            b2Vec2 anchor(x/2+float32(i), y/2);
            jd.Initialize(prevBody, body, anchor);
            world->CreateJoint(&jd);
            
            prevBody = body;
            //创建绘图节点
            CCNode* node = getChildByTag(tag_root_scene_rolling);
            //创建myObj
            CmyObj *sprite = CmyObj::create(world);
            CCTexture2D*_texture=NULL;
            if(i==N-1){
                CCTexture2D*texture=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/stone.png");
                _texture=texture;
            }else{
                CCTexture2D*texture=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/ropeSeg.png");
                _texture=texture;
            }
            sprite->initWithTexture(_texture,CCRect(0,0,halfWList[i]*2*PTM_RATIO,halfHList[i]*2*PTM_RATIO));
            sprite->set_quadTexCoords(ccp(0,0), ccp(1,0),
                                      ccp(0,1), ccp(1,1));

            sprite->pb2Body=body;
            sprite->pb2Body->SetUserData(sprite);
            node->addChild(sprite);
        }
        
        m_ropeDef.localAnchorB.SetZero();
        m_ropeDef.collideConnected=true;//让悬点与摆锤碰撞（默认是不碰撞的）
        float32 extraLength = 0.01f;//此有何用
        m_ropeDef.maxLength = N - 1.0f + extraLength;
        m_ropeDef.bodyB = prevBody;
    }
    
    {
        m_ropeDef.bodyA = pinnerObj->pb2Body;
        m_rope = world->CreateJoint(&m_ropeDef);
    }*/
    //----开始更新
    scheduleUpdate();
   
}

HelloWorld::~HelloWorld()
{
	//先清楚场景，再释放box2d的world，原因是场景中的某些节点的析构函数中引用了world，如果先清除world，会导致这部分场景节点析构时出错
	this->removeAllChildrenWithCleanup(true);
    delete world;
    world = NULL;
    
}
void HelloWorld::rightKeyDown(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent){
    inputSignal.rightKeyDown=true;
    if(pPlayer->is_jump()){
        pPlayer->update_state(player_state_jump_forward_right);
    }else if(pPlayer->is_forward()||pPlayer->is_still()){
        pPlayer->update_state(player_state_forward_right);
    }else if(pPlayer->is_swim()){
        pPlayer->update_state(player_state_swim_forward_right);
    }
    //----动画（animation只能现用现创建，且runAction一次后不能再次使用）
    pPlayer->stopAllActions();
    CCAnimation*animation_runRight=CCAnimation::create();
    CCTexture2D*texture_run;
    int nFrame;
    if(pPlayer->state==player_state_swim_forward_right){
        texture_run=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_swim_right.png");
        nFrame=4;
    }else{
        texture_run=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_run_right.png");
        nFrame=6;
    }
    for(int i=0;i<nFrame;i++){
        animation_runRight->addSpriteFrameWithTexture(texture_run, CCRect(i*128,0,128,128));
    }
    animation_runRight->setDelayPerUnit(0.13f);
    animation_runRight->setRestoreOriginalFrame(true);
    CCAnimate* animate_runRight=CCAnimate::create(animation_runRight);
    CCRepeatForever*repeatForever=CCRepeatForever::create(animate_runRight);
    pPlayer->runAction(repeatForever);

}

void HelloWorld::rightKeyUpInside(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent){
    b2Vec2 v=pPlayer->pb2Body->GetLinearVelocity();
    pPlayer->pb2Body->SetLinearVelocity(b2Vec2(0,v.y));
    inputSignal.rightKeyDown=false;
    if(pPlayer->is_jump()){
        pPlayer->update_state(player_state_jump_still_right);
    }else if(pPlayer->is_forward()||pPlayer->is_still()){
        pPlayer->update_state(player_state_still_right);
    }else if(pPlayer->is_swim()){
        pPlayer->update_state(player_state_swim_still_right);
    }
    //---停止动画
    pPlayer->stopAllActions();
    //重设贴图
    pPlayer->setTexture(pPlayer->tex_still_right);
    pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));

}
void HelloWorld::rightKeyDragExit(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent){
    b2Vec2 v=pPlayer->pb2Body->GetLinearVelocity();
    pPlayer->pb2Body->SetLinearVelocity(b2Vec2(0,v.y));
    inputSignal.rightKeyDown=false;
    if(pPlayer->is_jump()){
        pPlayer->update_state(player_state_jump_still_right);
    }else if(pPlayer->is_forward()||pPlayer->is_still()){
        pPlayer->update_state(player_state_still_right);
    }else if(pPlayer->is_swim()){
        pPlayer->update_state(player_state_swim_still_right);
    }
    //---停止动画
    pPlayer->stopAllActions();
    //重设贴图
    pPlayer->setTexture(pPlayer->tex_still_right);
    pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));

}
void HelloWorld::rightKeyDragEnter(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent){
    inputSignal.rightKeyDown=true;
    if(pPlayer->is_jump()){
        pPlayer->update_state(player_state_jump_forward_right);
    }else if(pPlayer->is_forward()||pPlayer->is_still()){
        pPlayer->update_state(player_state_forward_right);
    }else if(pPlayer->is_swim()){
        pPlayer->update_state(player_state_swim_forward_right);
    }
    //----动画（animation只能现用现创建，且runAction一次后不能再次使用）
        pPlayer->stopAllActions();
    CCAnimation*animation_runRight=CCAnimation::create();
    CCTexture2D*texture_run;
    int nFrame;
    if(pPlayer->state==player_state_swim_forward_right){
        texture_run=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_swim_right.png");
        nFrame=4;
    }else{
        texture_run=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_run_right.png");
        nFrame=6;
    }

    for(int i=0;i<nFrame;i++){
        animation_runRight->addSpriteFrameWithTexture(texture_run, CCRect(i*128,0,128,128));
    }
    animation_runRight->setDelayPerUnit(0.13f);
    animation_runRight->setRestoreOriginalFrame(true);
    CCAnimate* animate_runRight=CCAnimate::create(animation_runRight);
    CCRepeatForever*repeatForever=CCRepeatForever::create(animate_runRight);
    pPlayer->runAction(repeatForever);
  
}

void HelloWorld::leftKeyDown(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent){
    
    inputSignal.leftKeyDown=true;
    if(pPlayer->is_jump()){
        pPlayer->update_state(player_state_jump_forward_left);
    }else if(pPlayer->is_forward()||pPlayer->is_still()){
        pPlayer->update_state(player_state_forward_left);
    }else if(pPlayer->is_swim()){
        pPlayer->update_state(player_state_swim_forward_left);
    
    }
    //----动画（animation只能现用现创建，且runAction一次后不能再次使用）
        pPlayer->stopAllActions();
    CCAnimation*animation_runRight=CCAnimation::create();
    CCTexture2D*texture_run;
    int nFrame;
    if(pPlayer->state==player_state_swim_forward_left){
        texture_run=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_swim_left.png");
        nFrame=4;
    }else{
        texture_run=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_run_left.png");
        nFrame=6;
    }
    CCSize texSize=texture_run->getContentSize();
    for(int i=0;i<nFrame;i++){
        animation_runRight->addSpriteFrameWithTexture(texture_run, CCRect(texSize.width-128-i*128,0,128,128));
    }
    animation_runRight->setDelayPerUnit(0.13f);
    animation_runRight->setRestoreOriginalFrame(true);
    CCAnimate* animate_runRight=CCAnimate::create(animation_runRight);
    CCRepeatForever*repeatForever=CCRepeatForever::create(animate_runRight);
    pPlayer->runAction(repeatForever);
}

void HelloWorld::leftKeyUpInside(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent){
    b2Vec2 v=pPlayer->pb2Body->GetLinearVelocity();
    pPlayer->pb2Body->SetLinearVelocity(b2Vec2(0,v.y));
    inputSignal.leftKeyDown=false;
    if(pPlayer->is_jump()){
        pPlayer->update_state(player_state_jump_still_left);
    }else if(pPlayer->is_forward()||pPlayer->is_still()){
        pPlayer->update_state(player_state_still_left);
    }else if(pPlayer->is_swim()){
        pPlayer->update_state(player_state_swim_still_left);
    }
    //---停止动画
    pPlayer->stopAllActions();
    //重设贴图
    pPlayer->setTexture(pPlayer->tex_still_left);
    pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));

}
void HelloWorld::leftKeyDragExit(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent){
    b2Vec2 v=pPlayer->pb2Body->GetLinearVelocity();
    pPlayer->pb2Body->SetLinearVelocity(b2Vec2(0,v.y));
    inputSignal.leftKeyDown=false;
    if(pPlayer->is_jump()){
        pPlayer->update_state(player_state_jump_still_left);
    }else if(pPlayer->is_forward()||pPlayer->is_still()){
        pPlayer->update_state(player_state_still_left);
        
    }else if(pPlayer->is_swim()){
        pPlayer->update_state(player_state_swim_still_left);
    }
    //---停止动画
    pPlayer->stopAllActions();
    //重设贴图
    pPlayer->setTexture(pPlayer->tex_still_left);
    pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));

}
void HelloWorld::leftKeyDragEnter(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent){
    
    inputSignal.leftKeyDown=true;
    if(pPlayer->is_jump()){
        pPlayer->update_state(player_state_jump_forward_left);
    }else if(pPlayer->is_forward()||pPlayer->is_still()){
        pPlayer->update_state(player_state_forward_left);
        
    }else if(pPlayer->is_swim()){
        pPlayer->update_state(player_state_swim_forward_left);
    }
    //----动画（animation只能现用现创建，且runAction一次后不能再次使用）
        pPlayer->stopAllActions();
    CCAnimation*animation_runRight=CCAnimation::create();
    CCTexture2D*texture_run;
    int nFrame;
    if(pPlayer->state==player_state_swim_forward_left){
        texture_run=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_swim_left.png");
        nFrame=4;
    }else{
        texture_run=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_run_left.png");
        nFrame=6;
    }
    CCSize texSize=texture_run->getContentSize();
    for(int i=0;i<nFrame;i++){
        animation_runRight->addSpriteFrameWithTexture(texture_run, CCRect(texSize.width-128-i*128,0,128,128));
    }
    animation_runRight->setDelayPerUnit(0.13f);
    animation_runRight->setRestoreOriginalFrame(true);
    CCAnimate* animate_runRight=CCAnimate::create(animation_runRight);
    CCRepeatForever*repeatForever=CCRepeatForever::create(animate_runRight);
    pPlayer->runAction(repeatForever);
}


void HelloWorld::jumpKeyDown(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent){

    if(pPlayer->is_jump()==false){

            pPlayer->pushVyToTagvy(10.8);
     
    }

}
void HelloWorld::shotKeyDown(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent){
    CCPoint playerPos=pPlayer->getPosition();//由于知道父节点与世界无偏移，所以就不用再进一步转到世界了
    CshotBall *pShotBall=CshotBall::create(world);
    

    pShotBall->initWithInfo(pPlayer->isFacingRight()?this->objInfo_shotBall_right:this->objInfo_shotBall_left);
    this->getChildByTag(tag_root_scene_rolling)->getChildByTag(tag_root_scene_rolling_myObj)->addChild(pShotBall,99);
    pShotBall->pb2Body->SetTransform(b2Vec2(playerPos.x/PTM_RATIO,playerPos.y/PTM_RATIO), 0);
    pShotBall->pb2Body->SetAngularVelocity(pPlayer->isFacingRight()?-7:7);
    pShotBall->pushVxToTagvx(pPlayer->isFacingRight()?10:-10);
    pShotBall->schedule(schedule_selector(CshotBall::removeMeFromSceneAndDestroyBodyCallBack),2.5);
	
}




void HelloWorld::draw()
//注意，重载的draw只处理本节点的渲染，不管子节点
{
    //----画本节点
    CCLayer::draw();//如果本Layer没有贴图，则此句画不出任何东西
   


}

void HelloWorld::update(float dt)
{
    //It is recommended that a fixed time step is used with Box2D for stability
    //of the simulation, however, we are using a variable time step here.
    //You need to make an informed choice, the following URL is useful
    //http://gafferongames.com/game-physics/fix-your-timestep/
    
    int velocityIterations = 8;
    int positionIterations = 1;
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    world->Step(dt, velocityIterations, positionIterations);
    
    //Iterate over the bodies in the physics world
    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
    {
        if (b->GetUserData() != NULL) {
            //Synchronize the AtlasSprites position and rotation with the corresponding body
            CmyObj* myActor = (CmyObj*)b->GetUserData();
            myActor->setPosition( CCPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO) );
            myActor->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
        }    
    }//cout<<endl;
      
    //----计算pPlayer->touchSolid（必须放在是否在水中之后，必须放在其它之前）
    {
        //统计player的foot上碰撞的个数
        int nContact=0;//player的foot上碰撞的个数
        for(b2ContactEdge*ce=pPlayer->pb2Body->GetContactList();ce;ce=ce->next){
            b2Contact* contact=ce->contact;
            b2Fixture*fixtureA=contact->GetFixtureA();
            b2Fixture*fixtureB=contact->GetFixtureB();
            if(fixtureA==pPlayer->fixture_foot){
                nContact++;
            }
            if(fixtureB==pPlayer->fixture_foot){
                nContact++;
            }
        }//得到nContact
        pPlayer->touchSolid=(nContact>0);
    }
    
    //----着陆
    if(pPlayer->is_jump()&&pPlayer->touchSolid){
        //转为非跳起
        if(pPlayer->state==player_state_jump_forward_right){
            pPlayer->update_state(player_state_forward_right);
        }else if(pPlayer->state==player_state_jump_still_right){
            pPlayer->update_state(player_state_still_right);
        }else if(pPlayer->state==player_state_jump_forward_left){
            pPlayer->update_state(player_state_forward_left);
        }else if(pPlayer->state==player_state_jump_still_left){
            pPlayer->update_state(player_state_still_left);
        }
    }
    if(pPlayer->is_jump()==false&&pPlayer->touchSolid==false){
        //转为跳起或swim
        //判断是否swim
        const float sinkH=0;
        CCRect playerRect=pPlayer->boundingBox();
        if(playerRect.getMinY()<=m_waterSurfaceHeight-sinkH){//swim
            if(pPlayer->is_forward()){
                if(pPlayer->isFacingRight()){
                    if(pPlayer->state!=player_state_swim_forward_right){
                        pPlayer->state=player_state_swim_forward_right;
                        //播放动画
                        pPlayer->stopAllActions();//先停止先前的动画
                        CCAnimation*animation=CCAnimation::create();
                        CCTexture2D*texture;
                        texture=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_swim_right.png");
                        int nFrame=4;
                        CCSize texSize=texture->getContentSize();
                        for(int i=0;i<nFrame;i++){
                            animation->addSpriteFrameWithTexture(texture, CCRect(i*128,0,128,128));
                        }
                        animation->setDelayPerUnit(0.13f);
                        animation->setRestoreOriginalFrame(true);
                        CCAnimate* animate=CCAnimate::create(animation);
                        CCRepeatForever*repeatForever=CCRepeatForever::create(animate);
                        pPlayer->runAction(repeatForever);

                    }
                }else{
                    if(pPlayer->state!=player_state_swim_forward_left){
                        pPlayer->state=player_state_swim_forward_left;
                        //播放动画
                        pPlayer->stopAllActions();//先停止先前的动画
                        CCAnimation*animation=CCAnimation::create();
                        CCTexture2D*texture;
                        texture=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_swim_left.png");
                        int nFrame=4;
                        CCSize texSize=texture->getContentSize();
                        for(int i=0;i<nFrame;i++){
                            animation->addSpriteFrameWithTexture(texture, CCRect(texSize.width-128-i*128,0,128,128));
                        }
                        animation->setDelayPerUnit(0.13f);
                        animation->setRestoreOriginalFrame(true);
                        CCAnimate* animate=CCAnimate::create(animation);
                        CCRepeatForever*repeatForever=CCRepeatForever::create(animate);
                        pPlayer->runAction(repeatForever);

                    }
                }
            }else if(pPlayer->is_still()){
                if(pPlayer->isFacingRight()){
                    pPlayer->state=player_state_swim_still_right;
                }else{
                    pPlayer->state=player_state_swim_still_left;
                }
                
            }
        }else{
            if(pPlayer->state==player_state_still_right){
                pPlayer->update_state(player_state_jump_still_right);
            }else if(pPlayer->state==player_state_forward_right){
                pPlayer->update_state(player_state_jump_forward_right);
            }else if(pPlayer->state==player_state_still_left){
                pPlayer->update_state(player_state_jump_still_left);
            }else if(pPlayer->state==player_state_forward_left){
                pPlayer->update_state(player_state_jump_forward_left);
            }
   
        }
    }


    //----计算主角是否swim（必须放在判断是否着陆之前）
    //脚接触水面不算swim状态，要吃水到一定深度才算swim
    {
        const float sinkH=20;
        CCRect playerRect=pPlayer->boundingBox();
        if(playerRect.getMinY()<=m_waterSurfaceHeight-sinkH){//swim
            if(pPlayer->isFacingRight()){
                if(pPlayer->is_xx_still()){
                    pPlayer->state=player_state_swim_still_right;
                }else if(pPlayer->is_xx_forward()){
                    if(pPlayer->state!=player_state_swim_forward_right){
                        pPlayer->state=player_state_swim_forward_right;
                        //播放动画
                        pPlayer->stopAllActions();//先停止先前的动画
                        CCAnimation*animation=CCAnimation::create();
                        CCTexture2D*texture;
                        texture=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_swim_right.png");
                        int nFrame=4;
                        CCSize texSize=texture->getContentSize();
                        for(int i=0;i<nFrame;i++){
                            animation->addSpriteFrameWithTexture(texture, CCRect(i*128,0,128,128));
                        }
                        animation->setDelayPerUnit(0.13f);
                        animation->setRestoreOriginalFrame(true);
                        CCAnimate* animate=CCAnimate::create(animation);
                        CCRepeatForever*repeatForever=CCRepeatForever::create(animate);
                        pPlayer->runAction(repeatForever);
                        
                    }
                }
            }else{//facingLeft
                if(pPlayer->is_xx_still()){
                    pPlayer->state=player_state_swim_still_left;
                }else if(pPlayer->is_xx_forward()){
                    if(pPlayer->state!=player_state_swim_forward_left){
                        pPlayer->state=player_state_swim_forward_left;
                        //播放动画
                        pPlayer->stopAllActions();//先停止先前的动画
                        CCAnimation*animation=CCAnimation::create();
                        CCTexture2D*texture;
                        texture=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_swim_left.png");
                        int nFrame=4;
                        CCSize texSize=texture->getContentSize();
                        for(int i=0;i<nFrame;i++){
                            animation->addSpriteFrameWithTexture(texture, CCRect(texSize.width-128-i*128,0,128,128));
                        }
                        animation->setDelayPerUnit(0.13f);
                        animation->setRestoreOriginalFrame(true);
                        CCAnimate* animate=CCAnimate::create(animation);
                        CCRepeatForever*repeatForever=CCRepeatForever::create(animate);
                        pPlayer->runAction(repeatForever);
                        
                    }
                }
            }
        }else{//不swim
            if(pPlayer->isFacingRight()){
                if(pPlayer->is_xx_still()){
                    if(/*pPlayer->touchSolid*/pPlayer->is_jump()==false){
                        pPlayer->state=player_state_still_right;
                    }else{
                        pPlayer->state=player_state_jump_still_right;
                    }
                }else if(pPlayer->is_xx_forward()){
                    if(/*pPlayer->touchSolid*/pPlayer->is_jump()==false){
                        if(pPlayer->state!=player_state_forward_right){
                            pPlayer->state=player_state_forward_right;
                            //播放动画
                            pPlayer->stopAllActions();//先停止先前的动画
                            CCAnimation*animation=CCAnimation::create();
                            CCTexture2D*texture;
                            texture=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_run_right.png");
                            int nFrame=6;
                            CCSize texSize=texture->getContentSize();
                            for(int i=0;i<nFrame;i++){
                                animation->addSpriteFrameWithTexture(texture, CCRect(i*128,0,128,128));
                            }
                            animation->setDelayPerUnit(0.13f);
                            animation->setRestoreOriginalFrame(true);
                            CCAnimate* animate=CCAnimate::create(animation);
                            CCRepeatForever*repeatForever=CCRepeatForever::create(animate);
                            pPlayer->runAction(repeatForever);
                            
                        }
                    }else{
                        
                        pPlayer->state=player_state_jump_forward_right;
                    }
                    
                }
            }else{//facingLeft
                if(pPlayer->is_xx_still()){
                    if(/*pPlayer->touchSolid*/pPlayer->is_jump()==false){
                        pPlayer->state=player_state_still_left;
                    }else{
                        pPlayer->state=player_state_jump_still_left;
                    }
                }else if(pPlayer->is_xx_forward()){
                    if(/*pPlayer->touchSolid*/pPlayer->is_jump()==false){
                        if(pPlayer->state!=player_state_forward_left){
                            pPlayer->state=player_state_forward_left;
                            //播放动画
                            pPlayer->stopAllActions();//先停止先前的动画
                            CCAnimation*animation=CCAnimation::create();
                            CCTexture2D*texture;
                            texture=CCTextureCache::sharedTextureCache()->addImage("data/global/tex/player_run_left.png");
                            int nFrame=6;
                            CCSize texSize=texture->getContentSize();
                            for(int i=0;i<nFrame;i++){
                                animation->addSpriteFrameWithTexture(texture, CCRect(texSize.width-128-i*128,0,128,128));
                            }
                            animation->setDelayPerUnit(0.13f);
                            animation->setRestoreOriginalFrame(true);
                            CCAnimate* animate=CCAnimate::create(animation);
                            CCRepeatForever*repeatForever=CCRepeatForever::create(animate);
                            pPlayer->runAction(repeatForever);
                        }
                    }else{
                        pPlayer->state=player_state_jump_forward_left;
                    }
                    
                }
            }
            
        }
    }
 
    //----根据player状态设置player的动画，贴图和动作
  /*  cout<<"state:";
    pPlayer->print_sate();
    cout<<endl;*/
    if(pPlayer->isChange_state()){//如果player状态有变
        //根据变到的状态state更新player
        switch(pPlayer->state){
            case player_state_still_right:
            {
                //重设贴图
                pPlayer->setTexture(pPlayer->tex_still_right);
                pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));
            }break;
            case player_state_still_left:
                //重设贴图
                pPlayer->setTexture(pPlayer->tex_still_left);
                pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));
            {
            }break;
            case player_state_forward_right:
            {
                float m=pPlayer->pb2Body->GetMass();
                b2Vec2 v=pPlayer->pb2Body->GetLinearVelocity();
                const float vx=5;
                //将v的x分量弄成vx
                b2Vec2 dv=b2Vec2(vx-v.x,0);
                b2Vec2 impluse=m*dv;
                pPlayer->pb2Body->ApplyLinearImpulse(impluse, pPlayer->pb2Body->GetWorldCenter());
         /*       //重设贴图
                pPlayer->setTexture(pPlayer->tex_forward_right_first);
                pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));*/
                
            }break;
            case player_state_forward_left:
            {
                float m=pPlayer->pb2Body->GetMass();
                b2Vec2 v=pPlayer->pb2Body->GetLinearVelocity();
                const float vx=-5;
                //将v的x分量弄成vx
                b2Vec2 dv=b2Vec2(vx-v.x,0);
                b2Vec2 impluse=m*dv;
                pPlayer->pb2Body->ApplyLinearImpulse(impluse, pPlayer->pb2Body->GetWorldCenter());
            /*    //重设贴图
                pPlayer->setTexture(pPlayer->tex_forward_left_first);
                pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));*/
            }break;
            case player_state_jump_still_right:
            {
                //重设贴图
                pPlayer->setTexture(pPlayer->tex_jump_still_right);
                pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));
            }break;
            case player_state_jump_still_left:
            {
                //重设贴图
                pPlayer->setTexture(pPlayer->tex_jump_still_left);
                pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));
            }break;
            case player_state_jump_forward_right:
            {
                float m=pPlayer->pb2Body->GetMass();
                b2Vec2 v=pPlayer->pb2Body->GetLinearVelocity();
                const float vx=5;
                //将v的x分量弄成vx
                b2Vec2 dv=b2Vec2(vx-v.x,0);
                b2Vec2 impluse=m*dv;
                pPlayer->pb2Body->ApplyLinearImpulse(impluse, pPlayer->pb2Body->GetWorldCenter());
                //重设贴图
                pPlayer->setTexture(pPlayer->tex_jump_forward_right);
                pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));
            }break;
            case player_state_jump_forward_left:
            {
                float m=pPlayer->pb2Body->GetMass();
                b2Vec2 v=pPlayer->pb2Body->GetLinearVelocity();
                const float vx=-5;
                //将v的x分量弄成vx
                b2Vec2 dv=b2Vec2(vx-v.x,0);
                b2Vec2 impluse=m*dv;
                pPlayer->pb2Body->ApplyLinearImpulse(impluse, pPlayer->pb2Body->GetWorldCenter());
                //重设贴图
                pPlayer->setTexture(pPlayer->tex_jump_forward_left);
                pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));
            }break;
            case player_state_swim_still_right:
            {
                //重设贴图
                pPlayer->setTexture(pPlayer->tex_swim_still_right);
                pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));

               
     
            }break;
            case player_state_swim_still_left:
            {
                //重设贴图
                pPlayer->setTexture(pPlayer->tex_swim_still_left);
                pPlayer->set_quadTexCoords(ccp(0,0), ccp(1,0), ccp(0,1), ccp(1,1));

                
            }break;
            case player_state_swim_forward_right:
            {
                float m=pPlayer->pb2Body->GetMass();
                b2Vec2 v=pPlayer->pb2Body->GetLinearVelocity();
                const float vx=5;
                //将v的x分量弄成vx
                b2Vec2 dv=b2Vec2(vx-v.x,0);
                b2Vec2 impluse=m*dv;
                pPlayer->pb2Body->ApplyLinearImpulse(impluse, pPlayer->pb2Body->GetWorldCenter());
                
            }break;
            case player_state_swim_forward_left:
            {
                float m=pPlayer->pb2Body->GetMass();
                b2Vec2 v=pPlayer->pb2Body->GetLinearVelocity();
                const float vx=-5;
                //将v的x分量弄成vx
                b2Vec2 dv=b2Vec2(vx-v.x,0);
                b2Vec2 impluse=m*dv;
                pPlayer->pb2Body->ApplyLinearImpulse(impluse, pPlayer->pb2Body->GetWorldCenter());
            
            }break;
                default:
                cout<<"warning:未知状态!"<<endl;
                break;
        }
        
    }
  //  cout<<"player state:";pPlayer->print_sate();cout<<endl;
    //----相机跟随主角
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    CCPoint playerPos=pPlayer->getParent()->convertToWorldSpace(pPlayer->getPosition());
 //   cout<<"playerPos:"<<playerPos.x<<" "<<playerPos.y<<" "<<endl;
    CCCamera*camera=this->getChildByTag(tag_root_scene_rolling)->getCamera();
    float cx,cy,cz;
    float eyex,eyey,eyez;
 //   cout<<eyex<<" "<<eyey<<" "<<eyez<<endl;
    camera->getCenterXYZ(&cx, &cy, &cz);
    camera->getEyeXYZ(&eyex, &eyey, &eyez);
    //
    camera->setCenterXYZ(playerPos.x-s.width/2, playerPos.y-s.height/2, cz);
    camera->setEyeXYZ(playerPos.x-s.width/2, playerPos.y-s.height/2, eyez);
    //----背景循环
    {
        vector<CCSprite*> pBackGroundList;
        pBackGroundList.push_back((CCSprite*)this->getChildByTag(tag_root_scene_rolling)->getChildByTag(tag_root_scene_rolling_myObj)->getChildByTag(tag_backGround));
        pBackGroundList.push_back((CCSprite*)this->getChildByTag(tag_root_scene_rolling)->getChildByTag(tag_root_scene_rolling_myObj)->getChildByTag(tag_backGround2));
        //看player处在backGroundList中哪个元素上
        int index=-1;
        CCPoint playerPos=pPlayer->getParent()->convertToWorldSpace(pPlayer->getPosition());
        int nBackGround=(int)pBackGroundList.size();
        for(int i=0;i<nBackGround;i++){
            CCSprite*pBackGround=pBackGroundList[i];
            CCRect rect=pBackGround->boundingBox();//在父坐标系中的坐标
            if(playerPos.x>rect.getMinX()&&playerPos.x<rect.getMaxX()){//只要横坐标在此范围内就算，不考虑纵坐标
                index=i;
                break;
            }
        }//得到index
        if(index!=-1){//如果找到
       //     cout<<"index:"<<index<<endl;
            CCSprite*pBackGround1=pBackGroundList[index];
            CCSprite*pBackGround2=pBackGroundList[1-index];
            //看pBackGround1是在边缘暴漏还是右边缘暴露
            //求相机视野范围
            CCSize winSize = CCDirector::sharedDirector()->getWinSize();
            CCCamera*camera=this->getChildByTag(tag_root_scene_rolling)->getCamera();
            float eyex,eyey,eyez;
            camera->getEyeXYZ(&eyex, &eyey, &eyez);
            CCRect viewRect(eyex,eyey,winSize.width,winSize.height);
            CCRect rect1=pBackGround1->boundingBox();
            if(rect1.getMaxX()<viewRect.getMaxX()){//右边缘暴露
                //把pBackGround2补到右边
              //     cout<<"右边缘暴露"<<endl;
                pBackGround2->setPosition(ccp(rect1.getMaxX(),rect1.getMinY()));
            }
            if(rect1.getMinX()>viewRect.getMinX()){//左边缘暴露
                //把pBackGround2补充到左边
             //      cout<<"左边缘暴露"<<endl;
                pBackGround2->setPosition(ccp(rect1.getMinX()-rect1.size.width,rect1.getMinY()));
            }
        }
    }
    //一旦子弹超出画面，就删除
    {
        //求相机视野范围
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        CCCamera*camera=this->getChildByTag(tag_root_scene_rolling)->getCamera();
        float eyex,eyey,eyez;
        camera->getEyeXYZ(&eyex, &eyey, &eyez);
        CCRect viewRect(eyex,eyey,winSize.width,winSize.height);
        //将超出视野的子弹删除
        CCNode *sceneRoot=this->getChildByTag(tag_root_scene_rolling);
        CCArray *arr=sceneRoot->getChildren();
        CCObject* obj = NULL;
        CCARRAY_FOREACH(arr, obj) {
            CmyObj* pObj = (CmyObj*) obj;
            if(pObj->getTag()!=tag_shotBall_right&&pObj->getTag()!=tag_shotBall_left)continue;
            //看pObj是否超出视野
            CCRect rect=pObj->boundingBox();
            if(rect.getMinX()>viewRect.getMaxX()||rect.getMaxX()<viewRect.getMinX()){//超出视野
                pObj->removeMeFromSceneAndDestroyBody(true);
            }
        }
    }
    //---
//    cout<<pPlayer->numberOfRunningActions()<<endl;
    
    //----所有物体受浮力
    //只考虑各物体的第一个fixture
    {
        CCNode *sceneRoot=this->getChildByTag(tag_root_scene_rolling)->getChildByTag(tag_root_scene_rolling_myObj);
        CCArray *arr=sceneRoot->getChildren();
        CCObject* obj = NULL;
        CCARRAY_FOREACH(arr, obj) {
            CmyObj* pObj = (CmyObj*) obj;
            if(pObj->pb2Body==NULL)continue;
            if(pObj->pb2Body->GetType()!=b2_dynamicBody)continue;//动态物体才受浮力
            if((pObj->getTag()==tag_shotBall_left||pObj->getTag()==tag_shotBall_right)
               &&((CshotBall*)pObj)->isAfterHit==false)continue;//如果是子弹且还未击中物体，则跳过
            float waterSurfaceHeight=m_waterSurfaceHeight/PTM_RATIO;
            b2AABB aabb=pObj->pb2Body->GetFixtureList()->GetAABB(0);
            if(aabb.lowerBound.y<=waterSurfaceHeight){//接触水体
                //用速度衰减模拟水的摩擦阻力
                pObj->pb2Body->SetAngularDamping(pObj->angularDamping_inWater);
                pObj->pb2Body->SetLinearDamping(pObj->linearDamping_inWater);
                //如果是子弹，要将其重力缩小
                if(pObj->getTag()==tag_shotBall_right||pObj->getTag()==tag_shotBall_left){
                    pObj->pb2Body->SetGravityScale(0.2);
                }
                //用水面将pObj的第一个shape切割成两部分
                b2Fixture*fixture=NULL;
                if(pObj->getTag()==tag_player){
                    fixture=pPlayer->fixture_body;
                }else{
                    fixture=pObj->pb2Body->GetFixtureList();
                }
                b2Shape*shape=fixture->GetShape();
                //计算shape沉入水面的面积
                float S=0;
                if(pObj->pb2Body->GetFixtureList()->GetShape()->GetType()==b2Shape::e_polygon){//如果是多边形物体
                    b2PolygonShape upperShape,lowerShape;
                    bool upperShapeGot=false,lowerShapeGot=false;
                    splitPolygonShapeWithHoriLine(*(b2PolygonShape*)shape, pObj->pb2Body->GetTransform(), waterSurfaceHeight,
                                                  upperShapeGot,lowerShapeGot,
                                                  upperShape, lowerShape);
                    if(lowerShapeGot==false){
                        //       cout<<"lowerShape not got!"<<endl;
                        continue;
                    }
                    
                    //计算lowerShape的面积
                    b2MassData massData;
                    lowerShape.ComputeMass(&massData, 1);
                    S=massData.mass;//面积
                    //      cout<<"S:"<<S<<endl;
                
                }else if(pObj->pb2Body->GetFixtureList()->GetShape()->GetType()==b2Shape::e_circle){//如果是circle
                    const float pi=3.1415926;
                    b2Vec2 c=pObj->pb2Body->GetWorldCenter();//圆心世界坐标
                    float r=pObj->pb2Body->GetFixtureList()->GetShape()->m_radius;//圆半径
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
                    continue;
                }
                //得到S
                //水的密度
                float density_water=20;
                //计算浮力
                b2Vec2 buoyancyForce=density_water*S*fabsf(world->GetGravity().y)*b2Vec2(0,1);//向量必须放在最后
         //       cout<<"buoyancyForce:"<<buoyancyForce.y<<endl;
         //       cout<<"gravityForce:"<<pObj->pb2Body->GetMass()*fabsf(world->GetGravity().y)<<endl;
                //将浮力冲量施加到物体上
                pObj->pb2Body->ApplyForce(buoyancyForce, pObj->pb2Body->GetWorldCenter());
        /*        pObj->pb2Body->DestroyFixture(fixture);
                b2FixtureDef fixtureDef;
                fixtureDef.shape=&lowerShape;
                pObj->pb2Body->CreateFixture(&fixtureDef);
         pObj->pb2Body->SetLinearVelocity(b2Vec2(0,0));
         pObj->pb2Body->SetAngularVelocity(0);
                     pObj->pb2Body->SetType(b2_staticBody);
         */
            }
            else{//不接触水体
                //恢复衰减值
                pObj->pb2Body->SetAngularDamping(pObj->angularDamping_normal);
                pObj->pb2Body->SetLinearDamping(pObj->linearDamping_normal);
                //如果是子弹，要将其重力值恢复
                if(pObj->getTag()==tag_shotBall_right||pObj->getTag()==tag_shotBall_left){
                    pObj->pb2Body->SetGravityScale(1);
                }
            
            }
         
        }

        
    }
 //    cout<<dt<<endl;
/*    //审计world中的body数
    {
        cout<<"bodyCount:"<<world->GetBodyCount()<<endl;
    }
*/
  //  cout<<pPlayer->touchSolid<<endl;
    
    
    
    

}

/*void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    //Add a new body/atlas sprite at the touched location
    CCSetIterator it;
    CCTouch* touch;
    
 /*   for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        
        CCPoint location = touch->getLocationInView();
        
        location = CCDirector::sharedDirector()->convertToGL(location);
   //     cout<<"mos pos:"<<location.x<<" "<<location.y<<endl;
    }*/
  /*  //获得单点坐标
    touch=(CCTouch*)(*touches->begin());
    if(!touch)return;
    CCPoint location=touch->getLocationInView();
    location=CCDirector::sharedDirector()->convertToGL(location);
    mos.x=location.x;
    mos.y=location.y;
    //移动主角
    if(pPlayer!=NULL){
        pPlayer->pb2Body->SetLinearVelocity(b2Vec2(1.0,0));
    }else{
        cout<<"pPlayer==NULL!"<<endl;
    }
}*/
/*void HelloWorld::ccTouchesMoved(cocos2d::CCSet* touches , cocos2d::CCEvent* event)
{
    CCSetIterator it;
    CCTouch* touch;
    //获得单点坐标
    touch=(CCTouch*)(*touches->begin());
    if(!touch){
        return;
    }
    CCPoint location=touch->getLocationInView();
    location=CCDirector::sharedDirector()->convertToGL(location);
    mos.x=location.x;
    mos.y=location.y;
    
   
}*/
/*
void HelloWorld::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
    CCSetIterator it;
    CCTouch* touch;
 /*   for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        
        CCPoint location = touch->getLocationInView();
        
        location = CCDirector::sharedDirector()->convertToGL(location);
        
        addNewSpriteAtPosition( location );
        //遍历body，看哪个被选中
   }*/
  /*   //获得单点坐标
    touch=(CCTouch*)(*touches->begin());
    if(!touch)return;
    CCPoint location=touch->getLocationInView();
    location=CCDirector::sharedDirector()->convertToGL(location);
    mos.x=location.x;
    mos.y=location.y;
}*/


CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // add layer as a child to scene
    CCLayer* layer = new HelloWorld();
    scene->addChild(layer);
    layer->release();
    
    return scene;
}
