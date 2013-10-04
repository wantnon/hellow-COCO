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
#define PTM_RATIO 32
enum {
    //0要保留
    tag_root_scene_rolling = 1,
    tag_root_scene_nonRolling=2,
    tag_root_ui=3,
    tag_backGround=4,
    tag_backGround2=5,
    tag_player=6,
    tag_shotBall_right=7,
    tag_shotBall_left=8,
	tag_tiledMap=9,
	tag_root_scene_rolling_myObj=10,
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
#endif /* defined(__mybox2dtest__myDef__) */
