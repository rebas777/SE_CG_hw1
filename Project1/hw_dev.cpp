#include <windows.h>                                                                                                            
#include<GL\glut.h>
#include<math.h>
#include<time.h>
#include"Object.h"


HGLRC           hRC = NULL;                         
HDC             hDC = NULL;                          
HWND            hWnd = NULL;                          
HINSTANCE       hInstance;                           

bool    keys[256];                            
bool    active = TRUE;                                
bool    fullscreen = TRUE;                           


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);               

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)         
{
	if (height == 0)                            
	{
		height = 1;                          
	}

	glViewport(0, 0, width, height);                    
	glMatrixMode(GL_PROJECTION);                      
	glLoadIdentity();                          

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);                   
	glLoadIdentity();                         
}

Object *obj;

int InitGL(GLvoid)                           
{
	glShadeModel(GL_SMOOTH);                       
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);                   

	glClearDepth(1.0f);                       
	glEnable(GL_DEPTH_TEST);                        
	glDepthFunc(GL_LEQUAL);                       

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);     

	obj = new Object();

	return TRUE;                              
}


float width = 20;
float planeY = -3.0f;

void DrawFloor()
{
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glPushMatrix();
	glTranslatef(0.0f, planeY, 0.0f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-width / 2, 0.0f, +width / 2);
	glVertex3f(+width / 2, 0.0f, +width / 2);
	glVertex3f(+width / 2, 0.0f, -width / 2);
	glVertex3f(-width / 2, 0.0f, -width / 2);
	glEnd();
	glPopMatrix();
}

void DrawWall()
{
	glDisable(GL_LIGHTING);

	glPushMatrix();
	glTranslatef(width / 2, planeY + width / 2, 0.0f);
	glColor3f(0.85f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -width / 2, +width / 2);
	glVertex3f(0.0f, +width / 2, +width / 2);
	glVertex3f(0.0f, +width / 2, -width / 2);
	glVertex3f(0.0f, -width / 2, -width / 2);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.85f, 1.0f);
	glTranslatef(-width / 2, planeY + width / 2, 0.0f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -width / 2, +width / 2);
	glVertex3f(0.0f, +width / 2, +width / 2);
	glVertex3f(0.0f, +width / 2, -width / 2);
	glVertex3f(0.0f, -width / 2, -width / 2);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, planeY + width / 2, -width / 2);
	glColor3f(1.0f, 1.0f, 0.85f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-width / 2, -width / 2, 0.0f);
	glVertex3f(+width / 2, -width / 2, 0.0f);
	glVertex3f(+width / 2, +width / 2, 0.0f);
	glVertex3f(-width / 2, +width / 2, 0.0f);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();

	glEnable(GL_LIGHTING);

}

clock_t tm1 = 0, tm2 = 0;

int DrawGLScene(GLvoid)                             
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         
	glLoadIdentity();            

	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	gluLookAt
	(0.0f, 20.0f, 20.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	float dt = ((float)(tm2 - tm1)) / ((float)CLOCKS_PER_SEC);
	if (dt > 0.02f)
		dt = 0.02f;

	//gravity change velocity
	obj->velocity[1] += -9.87f*dt;

	//velocity change position
	//(just 1Demension)
	obj->pos[1] += obj->velocity[1] * dt;

	//collisions 
	//(just 1 demension)
	if (obj->pos[1]<= planeY)
	{
		obj->pos[1] = planeY;
		obj->velocity[1] = fabs(obj->velocity[1])*0.85;
	}
	obj->draw(obj->pos[1]);

	//draw the image in the glass
	if (obj->status()) {
		glPushMatrix();
		glTranslatef(0.0f, planeY - 4.4, 0.0f);
		obj->draw(-obj->pos[1]);
		glPopMatrix();
	}

	glEnable(GL_NORMALIZE);
	glPushMatrix();
	glTranslatef(0.0f, -width, 0.0f);
	DrawWall();
	glPopMatrix();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	DrawFloor();

	glDisable(GL_BLEND);

	DrawWall();

	tm1 = tm2;
	tm2 = clock();

	return TRUE;                               
}

GLvoid KillGLWindow(GLvoid)                         
{
	if (fullscreen)                             
	{
		ChangeDisplaySettings(NULL, 0);                  
		ShowCursor(TRUE);                       
	}
	if (hRC)                             
	{
		if (!wglMakeCurrent(NULL, NULL))                
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(hRC))                 
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;                           
	}
	if (hDC && !ReleaseDC(hWnd, hDC))                    
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;                           
	}
	if (hWnd && !DestroyWindow(hWnd))                   
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;                          
	}
	if (!UnregisterClass("OpenGL", hInstance))               
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;                         
	}
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint      PixelFormat;                        

	WNDCLASS    wc;                         
	DWORD       dwExStyle;                      
	DWORD       dwStyle;                       
	RECT WindowRect;                            

	WindowRect.left = (long)0;                       
	WindowRect.right = (long)width;                      
	WindowRect.top = (long)0;                         
	WindowRect.bottom = (long)height;                    

	fullscreen = fullscreenflag;                      

	hInstance = GetModuleHandle(NULL);            
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;      
	wc.lpfnWndProc = (WNDPROC)WndProc;                
	wc.cbClsExtra = 0;                        
	wc.cbWndExtra = 0;                        
	wc.hInstance = hInstance;                   
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);         
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);         
	wc.hbrBackground = NULL;                    
	wc.lpszMenuName = NULL;                     
	wc.lpszClassName = "OpenGL";                 

	if (!RegisterClass(&wc))                        
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Exit And Return FALSE
	}

	if (fullscreen)                             // Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;                   
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));       
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);       
		dmScreenSettings.dmPelsWidth = width;           
		dmScreenSettings.dmPelsHeight = height;           
		dmScreenSettings.dmBitsPerPel = bits;             
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			
			if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "rebas", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;               
			}
			else
			{
				
				MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;                   // Exit And Return FALSE
			}
		}
	}
	if (fullscreen)                             // Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;                  
		dwStyle = WS_POPUP;                      
		ShowCursor(FALSE);                     
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;           
		dwStyle = WS_OVERLAPPEDWINDOW;                   
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);     
	if (!(hWnd = CreateWindowEx(dwExStyle,              
		"OpenGL",              
		title,                  
		WS_CLIPSIBLINGS |          
		WS_CLIPCHILDREN |          
		dwStyle,                
		0, 0,                   
		WindowRect.right - WindowRect.left,  
		WindowRect.bottom - WindowRect.top,  
		NULL,                   
		NULL,                  
		hInstance,              
		NULL)))                 
	{
		KillGLWindow();                        
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           
	}

	static  PIXELFORMATDESCRIPTOR pfd =                 
	{
		sizeof(PIXELFORMATDESCRIPTOR),                  
		1,                              
		PFD_DRAW_TO_WINDOW |                        
		PFD_SUPPORT_OPENGL |                        
		PFD_DOUBLEBUFFER,                      
		PFD_TYPE_RGBA,                          
		bits,                               
		0, 0, 0, 0, 0, 0,                      
		0,                              
		0,                              
		0,                              
		0, 0, 0, 0,                        
		16,                             
		0,                              
		0,                            
		PFD_MAIN_PLANE,                        
		0,                             
		0, 0, 0                            
	};

	if (!(hDC = GetDC(hWnd)))                         // Did We Get A Device Context?
	{
		KillGLWindow();                        
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))             // Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();                         
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))               // Are We Able To Set The Pixel Format?
	{
		KillGLWindow();                        
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                          
	}

	if (!(hRC = wglCreateContext(hDC)))                   // Are We Able To Get A Rendering Context?
	{
		KillGLWindow();                        
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                          
	}

	if (!wglMakeCurrent(hDC, hRC))                        // Try To Activate The Rendering Context
	{
		KillGLWindow();                         
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           
	}

	ShowWindow(hWnd, SW_SHOW);                       
	SetForegroundWindow(hWnd);                      
	SetFocus(hWnd);                             
	ReSizeGLScene(width, height);                      

	if (!InitGL())                             
	{
		KillGLWindow();                         
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                          
	}

	return TRUE;                                
}

LRESULT CALLBACK WndProc(HWND    hWnd,                   
	UINT    uMsg,                   
	WPARAM  wParam,                 
	LPARAM  lParam)                 
{
	switch (uMsg)                               
	{
	case WM_ACTIVATE:                       
	{
		if (!HIWORD(wParam))                   
		{
			active = TRUE;                   
		}
		else
		{
			active = FALSE;                  
		}

		return 0;                      
	}
	case WM_SYSCOMMAND:                    
	{
		switch (wParam)                   
		{
		case SC_SCREENSAVE:            
		case SC_MONITORPOWER:             
			return 0;                  
		}
		break;                         
	}
	case WM_CLOSE:                         
	{
		PostQuitMessage(0);                
		return 0;                      
	}
	case WM_KEYDOWN:                        
	{
		keys[wParam] = TRUE;                    
		return 0;                      
	}
	case WM_KEYUP:                         
	{
		keys[wParam] = FALSE;                 
		return 0;                      
	}
	case WM_SIZE:                          
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));      
		return 0;                      
	}
	case WM_LBUTTONDOWN:
	{
		obj->velocity[1] += 10.0;
		DrawGLScene();
		SwapBuffers(hDC);
	}
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE   hInstance,              
	HINSTANCE   hPrevInstance,             
	LPSTR       lpCmdLine,             
	int     nCmdShow)              
{
	MSG msg;                                
	BOOL    done = FALSE;                        
												 
												  
												 
												 
												
	fullscreen = false;

	if (!CreateGLWindow("Bouncy Ball", 640, 480, 16, fullscreen))
	{
		return 0;                          
	}
	while (!done)                                
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))         
		{
			if (msg.message == WM_QUIT)              
			{
				done = TRUE;                  
			}
			else                          
			{
				TranslateMessage(&msg);            
				DispatchMessage(&msg);             
			}
		}
		else                               
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active)                     
			{
				if (keys[VK_ESCAPE])                // Was ESC Pressed?
				{
					done = TRUE;            
				}
				else if (keys[VK_SPACE]) {//jump
					obj->jump();
					DrawGLScene();
					SwapBuffers(hDC);
				}
				else if (keys[VK_UP]) {//change to a loaded object
					obj->toObj();
					DrawGLScene();
					SwapBuffers(hDC);
				}
				//else if (keys[VK_DOWN]) {//change to another loaded object
				//	obj->load(objName2);
				//	DrawGLScene();
				//	SwapBuffers(hDC);
				//}
				else if (keys[VK_LEFT]) {//change back to the ball
					obj->toBall();
					DrawGLScene();
					SwapBuffers(hDC);
				}
				else if (keys[VK_RIGHT]) {//reset the position and velocity to 0.
					obj->reset();
					DrawGLScene();
					SwapBuffers(hDC);
				}
				else if (keys[WM_LBUTTONDOWN]) {
					obj->jump();
					DrawGLScene();
					SwapBuffers(hDC);
				}
				else                       
				{
					DrawGLScene();              // Draw The Scene
					SwapBuffers(hDC);           // Swap Buffers (Double Buffering)
				}
			}
			if (keys[VK_F1])                   
			{
				keys[VK_F1] = FALSE;              
				KillGLWindow();                 
				fullscreen = !fullscreen;            
													  
				if (!CreateGLWindow("Bouncy Ball", 640, 480, 16, fullscreen))
				{
					return 0;              
				}
			}
		}
	}

	KillGLWindow();                             // Kill The Window
	return (msg.wParam);                           
}