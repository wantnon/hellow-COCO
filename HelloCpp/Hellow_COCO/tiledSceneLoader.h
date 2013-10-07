
#ifndef __mybox2dtest__tiledSceneLoader__
#define __mybox2dtest__tiledSceneLoader__

#include <iostream>
using namespace std;
#include "cocos2d.h"
#include "Box2D.h"
#include "myfunc.h"
#include "myDef.h"
#include "myBox2dFunc.h"
#include "myObj.h"
#include "thingManager.h"
using namespace cocos2d;
class CtiledSceneLoader
{
public:
	CtiledSceneLoader(){
	}
	virtual ~CtiledSceneLoader(){
	}
	CCNode*loadScene(string tmxFilePathShort,b2World*world)//����tmx����,����tiled�����ĸ��ڵ�
	{
		CCNode*tiledSceneRootNode=CCNode::create();
		//
		CCPoint mapOffset(0,-100);//��ͼƫ����
		CCTMXTiledMap*map=CCTMXTiledMap::create(tmxFilePathShort.c_str());
		tiledSceneRootNode->addChild(map);
		map->setPosition(mapOffset);
		map->setVisible(false);
		//���map��Ϣ
		CCSize mapSize=map->getMapSize();//s.height��layer������s.width��layer����
		CCSize tileSize=map->getTileSize();//ÿ��tile�Ĵ�С
		//��ò��objGroup
		CCTMXLayer*layer=map->layerNamed("groundLayer");
		assert(layer);
		CCTMXObjectGroup* objGroup=map->objectGroupNamed("thingLayer_polygon");
		assert(objGroup);
		CCTMXLayer*layer_thing_tile=map->layerNamed("thingLayer_tile");
		assert(layer_thing_tile);
		//����objGroup�еĸ�object
		cout<<"--------------------tiledmap thingLayer:"<<endl;
		CCObject* pObj = NULL;
        CCARRAY_FOREACH(objGroup->getObjects(), pObj)
        {
			//----����
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
			//----�������
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
			cout<<"--------"<<endl;
			cout<<"name:"<<name<<endl;
			cout<<"type:"<<type<<endl;
			cout<<"bodyType:"<<bodyType<<endl;
			cout<<"friction:"<<friction<<endl;
			cout<<"restitution:"<<restitution<<endl;
			cout<<"density:"<<density<<endl;
			cout<<"isSensor:"<<isSensor<<endl;
			cout<<"categoryBits:"<<categoryBits<<endl;
			cout<<"maskBits:"<<maskBits<<endl;
			cout<<"groupIndex:"<<groupIndex<<endl;
			cout<<"gravityScale:"<<gravityScale<<endl;
			//--------����myObj
			CmyObj*myObj=CmyObj::create(world);
			CthingManager::GetInstance()->addThing(myObj);
			//--------����b2Body
			if(type=="ellipse"){//����Բ
				//----��ȡ��Ϣ
				//x,y����Բ��Χ�е����½�
				float x = pDict->valueForKey("x")->floatValue();
				float y = pDict->valueForKey("y")->floatValue();//��ֵ��tmx�ļ��д��ֵ��ͬ������mapSize.height*tileSize.height��ȥ��ֵ�ŵ�tmx�ļ��е�ֵ
				float width = pDict->valueForKey("width")->floatValue();
				float height = pDict->valueForKey("height")->floatValue();
				//----�����Ϣ
				cout<<"xy:"<<x<<","<<y<<endl;
				cout<<"wh:"<<width<<" "<<height<<endl;
				//----ת����Ϣ
				//��Բ��
				float cx_inWorld_b2d=(x+width/2+map->getPositionX())/PTM_RATIO;
				float cy_inWorld_b2d=(y+height/2+map->getPositionY())/PTM_RATIO;
				//��뾶
				float r_b2d=(width/2+height/2)/2/PTM_RATIO;
				//----����body
				myObj->create_pb2Body_circle(r_b2d,bodyType,friction,restitution,density,isSensor,categoryBits,maskBits,groupIndex,gravityScale);
				myObj->pb2Body->SetTransform(b2Vec2(cx_inWorld_b2d ,cy_inWorld_b2d),0);
			}else if(type=="polygon"){//�Ƕ����
				//----��ȡ��Ϣ
				float x = pDict->valueForKey("x")->floatValue();
				float y = pDict->valueForKey("y")->floatValue();//��ֵ��tmx�ļ��д��ֵ��ͬ������mapSize.height*tileSize.height��ȥ��ֵ�ŵ�tmx�ļ��е�ֵ
				CCArray* points = (CCArray*)pDict->objectForKey("points");//�õ������б�
				assert(points);
				//----�����Ϣ
				cout<<"xy:"<<x<<","<<y<<endl;
				//points
				{
					cout<<"points:";
					int npoint=points->count();
					for(int i=0;i<npoint;i++){
						CCDictionary* dict = (CCDictionary*)points->objectAtIndex(i);
						float x=dict->valueForKey("x")->floatValue();
						float y=dict->valueForKey("y")->floatValue();
						cout<<x<<" "<<y<<" ";
					}cout<<endl;
				}
				//----ת����Ϣ
				//����polygon
				vector<b2Vec2> polygon;
				int npoint=points->count();
				float begx=x;
				float begy=mapSize.height*tileSize.height-y;
				for(int i=0;i<npoint;i++){
					CCDictionary* dict = (CCDictionary*)points->objectAtIndex(i);
					float x_inMap=begx+ dict->valueForKey("x")->floatValue();
					float y_inMap=mapSize.height*tileSize.height-(begy+ dict->valueForKey("y")->floatValue());
					//(x_inMap,y_inMap)����map���½�Ϊԭ������꣬�������mapƫ�ƣ������������е�����
					float x_inWorld=map->getPositionX()+x_inMap;
					float y_inWorld=map->getPositionY()+y_inMap;
					b2Vec2 v(x_inWorld/PTM_RATIO,y_inWorld/PTM_RATIO);
					polygon.push_back(v);
				}//�õ�polygon
				//��polygon������
				b2Vec2 wc=getAveragePoint(polygon);
				//��polygon���ж����תΪ�����wc
				for(int i=0;i<npoint;i++)polygon[i]-=wc;
				//----����body
				myObj->create_pb2Body_polygon(polygon,bodyType,friction,restitution,density,isSensor,categoryBits,maskBits,groupIndex,gravityScale);
				myObj->pb2Body->SetTransform(b2Vec2(wc.x ,wc.y),0);
			}
			//----����sprite
			//��layer_thing_tile��������myObj->pb2Body�ཻ�Ŀ�
			//������ռ��е�aabb
			b2AABB aabb=getAABB(myObj->pb2Body->GetFixtureList()->GetShape(),myObj->pb2Body->GetTransform());
			//��aabbת��Ϊccrect
			CCRect rect(aabb.lowerBound.x*PTM_RATIO,aabb.lowerBound.y*PTM_RATIO,
				(aabb.upperBound.x-aabb.lowerBound.x)*PTM_RATIO,(aabb.upperBound.y-aabb.lowerBound.y)*PTM_RATIO);
			//��layer_thing_tile��������rect�ཻ�Ŀ�
			//��rect��map�У���map���½�Ϊԭ�㣩������
			CCRect rect_inMap_OatLD=rect;
			rect_inMap_OatLD.origin=rect_inMap_OatLD.origin-map->getPosition();
			//��rect��map�У���map���Ͻ�Ϊԭ�㣩������(ע���ʱrect��originҪ������rect�����Ͻ�)
			CCRect rect_inMap_OatLU=rect_inMap_OatLD;
			rect_inMap_OatLU.origin.y=rect_inMap_OatLU.origin.y+rect_inMap_OatLU.size.height;//ԭ�����rect���Ͻǣ���mapԭ����ȡmap���½�
			rect_inMap_OatLU.origin.y=mapSize.height*tileSize.height-rect_inMap_OatLU.origin.y;//��mapԭ��������Ͻǣ��õ����յ�rect_inMap_OatLU
			float xmin=rect_inMap_OatLU.getMinX();
			float ymin=rect_inMap_OatLU.getMinY();
			float xmax=rect_inMap_OatLU.getMaxX();
			float ymax=rect_inMap_OatLU.getMaxY();
			int jmin=xmin/tileSize.width;
			int imin=ymin/tileSize.height;
			int jmax=xmax/tileSize.width;
			int imax=ymax/tileSize.height;
			cout<<"AABB cover tilemap:"<<endl;
			cout<<" i range:"<<imin<<" "<<imax<<endl;
			cout<<" j range:"<<jmin<<" "<<jmax<<endl;
			//��layer_thing_tile��(imin,jmin)��(imax,jmax)�����ڸ����sprite�Ŀ�������ΪmyObj���ӽڵ㣨֮����Ҫ�ÿ���������Ϊ��Щsprite�Ѿ�ͨ��map���뵽�����ˣ������ظ���ӵ�������
			CCSpriteBatchNode*batchNode_small=CCSpriteBatchNode::createWithTexture(layer_thing_tile->getTexture());
			myObj->addChild(batchNode_small);
			myObj->set_showb2Body(showb2BodyWireFrame);
			myObj->set_showSprite(false);
			for(int i=imin;i<=imax;i++){
				for(int j=jmin;j<=jmax;j++){
					CCSprite*sprite=layer_thing_tile->tileAt(ccp(j,i));//ע�⣬jΪ�ᣬiΪ�ݣ�����ccp(j,i)
					if(sprite==NULL)continue;
					CCSprite*_sprite=CCSprite::create();
					_sprite->setTexture(sprite->getTexture());
					_sprite->setTextureRect(sprite->getTextureRect());
					_sprite->setAnchorPoint(sprite->getAnchorPoint());
					//��sprite�����body������
					b2Vec2 bodyPos_inWorld_box2d=myObj->pb2Body->GetPosition();
					CCPoint bodyPos_inWorld(bodyPos_inWorld_box2d.x*PTM_RATIO,bodyPos_inWorld_box2d.y*PTM_RATIO);
					CCPoint spritePos_inWorld=sprite->getPosition()+map->getPosition();
					CCPoint offset_spriteRelativeToBody=spritePos_inWorld-bodyPos_inWorld;
					_sprite->setPosition(offset_spriteRelativeToBody);//ת��Ϊ�����
					batchNode_small->addChild(_sprite);
				}
			}
			tiledSceneRootNode->addChild(myObj);



        }
		//��Ǹ����Ƿ�������
		vector<vector<bool> > mat;
		mat.resize(mapSize.height);
		for(int i=0;i<mapSize.height;i++){
			mat[i].resize(mapSize.width);
			for(int j=0;j<mapSize.width;j++){
				mat[i][j]=false;
			}
		}
		//���mat
		for(int i=0;i<mapSize.height;i++){
			for(int j=0;j<mapSize.width;j++){
				CCPoint gridxy(j,i);
				int grid=layer->tileGIDAt(gridxy);
				CCDictionary* poperDic=(CCDictionary*)map->propertiesForGID(grid);
				//���isNoContent
				if(poperDic!=NULL){
					CCObject*ccobj=poperDic->objectForKey("nothing");
					if(ccobj!=NULL){
						CCString *ccs_value=(CCString*)ccobj;
						string str_value=ccs_value->getCString();
						if(str_value=="true")continue;
					}
				}
				//���sprite
				CCSprite*sprite=layer->tileAt(ccp(j,i));//ע�⣬jΪ�ᣬiΪ�ݣ�����ccp(j,i)
				if(sprite==NULL)continue;
				mat[i][j]=true;

			}
		}
		//��Ǹ����Ƿ��ڱ�Ե
		vector<vector<bool> > mat_isOnEdge;
		mat_isOnEdge.resize(mapSize.height);
		for(int i=0;i<mapSize.height;i++){
			mat_isOnEdge[i].resize(mapSize.width);
			for(int j=0;j<mapSize.width;j++){
				mat_isOnEdge[i][j]=false;
			}
		}
		//���mat_isOnEdge
		for(int i=0;i<(int)mat_isOnEdge.size();i++)
		{
			for(int j=0;j<(int)mat_isOnEdge[i].size();j++)
			{
				if(mat[i][j]==true){
					//�ж�mat[i][j]�Ƿ�Ϊ��Ե��
					int up=i-1>0?mat[i-1][j]:0;
					int dn=i+1<(int)mat.size()?mat[i+1][j]:0;
					int left=j-1>0?mat[i][j-1]:0;
					int right=j+1<(int)mat[i].size()?mat[i][j+1]:0;
					if(up!=0&&dn!=0&&left!=0&&right!=0){//�ǷǱ�Ե��
						//�޶���
					}else{//�Ǳ�Ե��
						mat_isOnEdge[i][j]=true;
					}
				}
			}
		}
		//��¼�����polygon�����ǳ������������Щ��
		vector<vector<vector<Cij> > > mat_polygonCoveredTileSet;
		mat_polygonCoveredTileSet.resize(mapSize.height);
		for(int i=0;i<mapSize.height;i++){
			mat_polygonCoveredTileSet[i].resize(mapSize.width);
		}
		//���mat_polygonCoveredTileSet
		for(int i=0;i<(int)mat_polygonCoveredTileSet.size();i++)
		{
			for(int j=0;j<(int)mat_polygonCoveredTileSet[i].size();j++)
			{
				int grid=layer->tileGIDAt(ccp(j,i));//��Ϊccp������j,i
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

		//����myObjs

		CCSpriteBatchNode*batchNode=CCSpriteBatchNode::createWithTexture(layer->getTexture());//��batchNode���ڴ�ŷǱ�Ե��
		tiledSceneRootNode->addChild(batchNode);

		for(int i=0;i<mapSize.height;i++)
		{
			for(int j=0;j<mapSize.width;j++)
			{	
				if(mat_isOnEdge[i][j]==true){//�Ǳ�Ե��		
					CCPoint gridxy(j,i);
					int grid=layer->tileGIDAt(gridxy);
					CCDictionary* poperDic=(CCDictionary*)map->propertiesForGID(grid);
					CCSprite*sprite=layer->tileAt(ccp(j,i));
					//----��sprite����myObj
					CmyObj*pmyObj=CmyObj::create(world);
					CCSize s=sprite->getContentSize();
					CCPoint p=sprite->convertToWorldSpace(ccp(0,0));
					//���ɶ����
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
							}//�õ�polygon
							polygonGot=true;
						}
					}
					if(polygonGot==false){//ֱ�����ɾ���
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
					
					//----��sprite����myObj��sprite
					//��pmyObj��polygon���ǳ���������Щ����
					vector<Cij> polygonCoveredTileSet=mat_polygonCoveredTileSet[i][j];
					//������Ҳ��������
					polygonCoveredTileSet.push_back(Cij(i,j));
					int nCoveredTile=(int)polygonCoveredTileSet.size();
					//��polygonCoveredTileSet�����и��ӵ�sprite���ΪmyObj���ӽڵ�(������Щsprite��ͨ��map��ӵ��˳��������Բ����ظ���ӣ�ֻ���������������)
					CCSpriteBatchNode*batchNode_small=CCSpriteBatchNode::createWithTexture(layer->getTexture());
					pmyObj->addChild(batchNode_small);
					pmyObj->set_showb2Body(showb2BodyWireFrame);
					pmyObj->set_showSprite(false);
					for(int k=0;k<nCoveredTile;k++){
						int I=polygonCoveredTileSet[k].i;
						int J=polygonCoveredTileSet[k].j;
						CCSprite*sprite=layer->tileAt(ccp(J,I));
						if(sprite==NULL)continue;
						CCSprite*_sprite=CCSprite::create();
						_sprite->setTexture(sprite->getTexture());
						_sprite->setTextureRect(sprite->getTextureRect());
						_sprite->setAnchorPoint(sprite->getAnchorPoint());
						//��sprite�����body������
						b2Vec2 bodyPos_inWorld_box2d=pmyObj->pb2Body->GetPosition();
						CCPoint bodyPos_inWorld(bodyPos_inWorld_box2d.x*PTM_RATIO,bodyPos_inWorld_box2d.y*PTM_RATIO);
						CCPoint spritePos_inWorld=sprite->getPosition()+map->getPosition();
						CCPoint offset_spriteRelativeToBody=spritePos_inWorld-bodyPos_inWorld;
						_sprite->setPosition(offset_spriteRelativeToBody);//ת��Ϊ�����
						batchNode_small->addChild(_sprite);
					}
					tiledSceneRootNode->addChild(pmyObj);
				}else{//���Ǳ�Ե��
					if(mat[i][j]){
						CCSprite*sprite=layer->tileAt(ccp(j,i));
						assert(sprite);
						//����sprite��ͨ��map���뵽�˳��������Բ����ظ���ӣ�ֻ�ܸ���һ�������
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
		//map�Ѿ�û���ˣ�����map
		map->removeFromParentAndCleanup(true);
		return tiledSceneRootNode;
	}

};
#endif /* defined(__mybox2dtest__tiledSceneLoader__) */
