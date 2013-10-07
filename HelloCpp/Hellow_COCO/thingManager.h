
#ifndef THINGMANAGER_H_H
#define THINGMANAGER_H_H

#include <iostream>
#include <vector>
using namespace std;
#include "cocos2d.h"
#include "myDef.h"
#include "myObj.h"
using namespace cocos2d;
class CthingManager
{
private:
	vector<CmyObj*> thingList; 
public:
	void addThing(CmyObj*myObj){
		thingList.push_back(myObj);
	}
	CmyObj*getThingAt(int index)const {
		if(index>=(int)thingList.size())return NULL;
		return thingList[index];
	}
	int getThingCount()const {
		return (int)thingList.size();
	}

	static CthingManager* GetInstance(){
		if(m_pInstance == NULL){
			m_pInstance = new CthingManager();
		}
        return m_pInstance;
	}
	//virtual~CthingManager(){}
private:
	static CthingManager*m_pInstance;
	CthingManager(){}
};

#endif
