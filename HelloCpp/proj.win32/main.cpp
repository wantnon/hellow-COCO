#include "main.h"
#include "../Classes/AppDelegate.h"
#include "CCEGLView.h"

USING_NS_CC;
/*
 #define CRTDBG_MAP_ALLOC 
 #include <stdlib.h>
#include <crtdbg.h>
 */
#include"myDef.h"
//#include "vld.h"
int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{

	int *leak=new int[10];

	AllocConsole();   // ��������̨
	SetConsoleTitle(_T("Debug Output"));      // ���ÿ���̨���ڱ���
	// �ض��� STDIN
	{
		FILE*fp;
		errno_t eno=freopen_s(&fp,"CONIN$", "r+t", stdin);
		if(eno!=0){//���ɹ�
			MessageBox(NULL,_T("�ض���stdinʧ��!"),_T("error"),MB_OK|MB_ICONEXCLAMATION);
			exit(0);
		}
	}
	// �ض���STDOUT
	{
		FILE*fp;
		errno_t eno=freopen_s(&fp,"CONOUT$", "w+t", stdout);
		if(eno!=0){//���ɹ�
			MessageBox(NULL,_T("�ض���stdoutʧ��!"),_T("error"),MB_OK|MB_ICONEXCLAMATION);
			exit(0);
		}

	}
	

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // create the application instance
    AppDelegate app;
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
    eglView->setViewName("HelloCpp");
    eglView->setFrameSize(1136, 640);//(960,640);//����������ʲôֵ���ܱ�֤��ȷ����
    // The resolution of ipad3 is very large. In general, PC's resolution is smaller than it.
    // So we need to invoke 'setFrameZoomFactor'(only valid on desktop(win32, mac, linux)) to make the window smaller.
    eglView->setFrameZoomFactor(0.8f);//(0.4f);
	int ret=CCApplication::sharedApplication()->run();

	//_CrtDumpMemoryLeaks();
    return ret; 

}
