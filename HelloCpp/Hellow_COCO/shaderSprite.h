
#ifndef __mybox2dtest__shaderSprite__
#define __mybox2dtest__shaderSprite__

#include <iostream>
using namespace std;
#include "cocos2d.h"
#include "myfunc.h"
#include "myDef.h"
using namespace cocos2d;

class CshaderSprite:public cocos2d::CCSprite
{
public:
	~CshaderSprite(){
	}
	static CshaderSprite * create()
	{
		CshaderSprite * pRet = new CshaderSprite();
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
	virtual void draw(void)//重载CCSprite的draw函数
		//如果节点连在批处理节点上，则节点的draw不调用
	{
		//调用父节点绘图
		CCSprite::draw();
/*		//自己的绘图
		ccGLEnableVertexAttribs(kCCVertexAttribFlag_PosColorTex );
		ccBlendFunc blend = getBlendFunc();
		ccGLBlendFunc(blend.src, blend.dst);

		getShaderProgram()->use();
		getShaderProgram()->setUniformsForBuiltins();
	//	getShaderProgram()->setUniformLocationWith2f(blurLocation, blur_.x, blur_.y);
	//	getShaderProgram()->setUniformLocationWith4fv(subLocation, sub_, 1);

		ccGLBindTexture2D( getTexture()->getName());

		//
		// Attributes
		//
#define kQuadSize sizeof(m_sQuad.bl)

		long offset = (long)&m_sQuad;
		//cout<<"offset:"<<offset<<endl;

		// vertex
		int diff = offsetof( ccV3F_C4B_T2F, vertices);
		//cout<<"diff_vertex:"<<diff<<endl;
		glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

		// texCoods
		diff = offsetof( ccV3F_C4B_T2F, texCoords);
		//cout<<"diff_texCoord:"<<diff<<endl;
		glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

		// color
		diff = offsetof( ccV3F_C4B_T2F, colors);
		//cout<<"diff_color:"<<diff<<endl;
		glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));


		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		CC_INCREMENT_GL_DRAWS(1);*/
	};
	
};


#endif /* defined(__mybox2dtest__shaderSprite__) */
