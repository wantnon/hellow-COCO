//
//  myDef.h
//  mybox2dtest
//
//  Created by apple on 13-7-20.
//
//

#ifndef __mybox2dtest__myDef__
#define __mybox2dtest__myDef__

#include <iostream>
const float inf=1000000;
const float pi=3.1415926;
#define PTM_RATIO 32



enum {
    //0Ҫ����
    tag_root_scene_rolling = 1,
    tag_root_scene_nonRolling=2,
    tag_root_ui=3,
    tag_backGround=4,
    tag_backGround2=5,
    tag_player=6,
    tag_shotBall_right=7,
    tag_shotBall_left=8,
};
class Cij{	
public:
	int i,j;

	Cij(){};
	Cij(int _i,int _j){
		set(_i,_j);
	}
	void set(int _i,int _j){
		i=_i;
		j=_j;
	}
};

static bool showb2BodyWireFrame=false;
#endif /* defined(__mybox2dtest__myDef__) */
