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
#define PTM_RATIO 32
enum {
    //0要保留
    tag_root_scene_rolling = 1,
    tag_root_scene_nonRolling=2,//不卷动的scene
    tag_root_ui=3,
    tag_backGround=4,
    tag_backGround2=5,
    tag_player=6,
    tag_shotBall_right=7,
    tag_shotBall_left=8,
};
#endif /* defined(__mybox2dtest__myDef__) */
