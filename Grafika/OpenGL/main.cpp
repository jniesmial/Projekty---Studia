#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
//#include <gl\glaux.h>		// Header File For The Glaux Library

HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active = TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen = TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
bool	pytajFullScreen = FALSE; // Czy pytac o tryb okna 

//-----------------------------------------------------------------------------------------------------------
int mode = 1;							//tryb 1-powiekszanie/zmniejszanie, 2-obracanie obiektow, 3-obracanie grupy, 4-zmiana kolorow
GLint iloscSzescianow = 3;				//ilosc rysowanych szescianow w jednej lini
GLfloat katCalosc = 0.0f;				//kat obrotu szescianow
GLfloat katPojednczy = 0.0f;			//kat obrotu pojedynczego szescianu
GLfloat odleglosc = 1.8f;				//odleglasc miedzy szescianami
GLfloat odlegloscMin = 1.2f;			// minimalna odleglasc miedzy szescianami
GLfloat odlegloscMax = 2.0f;			//maksymalna odleglasc miedzy szescianami
GLfloat rozmiar = 1.0f;					//rozmiar szescianu
bool powiekszanie = true;				//powiekszac/zmniejszac
GLfloat kolory[3] = { 0.2f,0.5f,1.0f };	//skladowe kolorow 3 szescianów RGB
bool k0 = true;							//pierwsza skladowa koloru rosnie/maleje
bool k1 = true;
bool k2 = true;
GLfloat predkoscKatCalosc = -1.0f;		//predkosc obrotu calosci
GLfloat predkoscKatPojedynczy = 2.0f;	//predkosc obrotu pojedynczego szescianu
GLfloat predkoscOdleglosc = 0.01f;		//predkosc powiekszania/zmniejszania
GLfloat predkoscKolor = 0.02f;			//szybkosc zmiany kolorow

GLfloat odlegloscObserwacji = -12;
//-------------------------------------------------------------------------------------------------------------

void rysujSzescian(GLfloat rozmiar, GLfloat rotation) { //rysowanie pojedynczego szescianu
	rozmiar = rozmiar / 2;
	glPushMatrix();						//wrzucenie macierzy przeksztalcen na stos
	glRotatef(rotation, 0.0f, 1.0f, 0.0f); //obrot pojedynczego szescianu
	glBegin(GL_QUADS);



	glNormal3f(0.0f, 0.0f, -1.0f);			//ustawienie normalniej

	glVertex3f(-rozmiar, rozmiar, -rozmiar);		//rysowanie przedniej sciany
	glVertex3f(rozmiar, rozmiar, -rozmiar);
	glVertex3f(rozmiar, -rozmiar, -rozmiar);
	glVertex3f(-rozmiar, -rozmiar, -rozmiar);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-rozmiar, rozmiar, rozmiar);			//rysowanie tylnej sciany
	glVertex3f(-rozmiar, -rozmiar, rozmiar);
	glVertex3f(rozmiar, -rozmiar, rozmiar);
	glVertex3f(rozmiar, rozmiar, rozmiar);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-rozmiar, -rozmiar, rozmiar);		// rysowanie dolnej sciany
	glVertex3f(rozmiar, -rozmiar, rozmiar);
	glVertex3f(rozmiar, -rozmiar, -rozmiar);
	glVertex3f(-rozmiar, -rozmiar, -rozmiar);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-rozmiar, rozmiar, rozmiar);			//rysowanie gornej sciany
	glVertex3f(-rozmiar, rozmiar, -rozmiar);
	glVertex3f(rozmiar, rozmiar, -rozmiar);
	glVertex3f(rozmiar, rozmiar, rozmiar);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-rozmiar, -rozmiar, rozmiar);	//rysowanie lewej sciany
	glVertex3f(-rozmiar, rozmiar, rozmiar);
	glVertex3f(-rozmiar, rozmiar, -rozmiar);
	glVertex3f(-rozmiar, -rozmiar, -rozmiar);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(rozmiar, -rozmiar, rozmiar);		//rysowanie prawej sciany
	glVertex3f(rozmiar, -rozmiar, -rozmiar);
	glVertex3f(rozmiar, rozmiar, -rozmiar);
	glVertex3f(rozmiar, rozmiar, rozmiar);

	glEnd();
	glPopMatrix();							//zdjecie macierzy przeksztalcen na stos
	return;
}
//----------------------------------------

GLfloat LightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };		// swiatlo otoczenia
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };		// swiatlo rozproszone
GLfloat LightPosition[] = { -10.0f, 0.0f, -10.0f, -1.0f };  // pozycja zrodla swiatla

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);		// Declaration For WndProc

// zmiana rozmiaru okna
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

// zainicjowanie sceny OpenGL
int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.2f, 0.2f, 0.2f, 0.5f);				// Grey Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);	// Position The Light
	glEnable(GL_LIGHT1);								// Enable Light One
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	return TRUE;										// Initialization Went OK
}

// rysuje scenê OpenGL
int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix

	glTranslatef(0.0f, 0.0f, odlegloscObserwacji);					//przeson w przód
	glRotatef(20, 1.0f, 0.0f, 0.0f);					//obróc o 20stopni w dó³


	glRotatef(katCalosc, 0.0f, 1.0f, 0.0f);				//obróæ ca³y szyk o katCalosc

	glTranslatef(-(iloscSzescianow - 1) * odleglosc / 2, -(iloscSzescianow - 1) * odleglosc / 2, -(iloscSzescianow - 1) * odleglosc / 2);  //przesun calosc tak, zeby srodek szyku byl w srodku obrotu

	for (int x = 0; x < iloscSzescianow; x++)
	{
		glPushMatrix();									//wrzuc aktualne polozenie na stos
		glTranslatef(x * odleglosc, 0.0f, 0.0f);			//przesuñ w osi x
		for (int y = 0; y < iloscSzescianow; y++)
		{
			glPushMatrix();								//wrzuc aktualne polozenie na stos
			glTranslatef(0.0f, y * odleglosc, 0.0f);		//przesun w osi y
			for (int z = 0; z < iloscSzescianow; z++)
			{
				glPushMatrix();							//wrzuc aktualne polozenie na stos
				glTranslatef(0.0f, 0.0f, z * odleglosc);	//przesun w osi z


				if (x == 0 && z == 0 && y == 2)						//ustaw kolor w zaleznosci ktory szescian
					glColor3f(kolory[0], kolory[1], kolory[2]);
				else if (x == 1 && z == 1 && y == 2)
					glColor3f(kolory[2], kolory[0], kolory[1]);
				else if (x == 2 && z == 2 && y == 2)
					glColor3f(kolory[1], kolory[2], kolory[0]);
				else
					glColor3f(0.8f, 0.6f, 1.0f);					// jeszeli zaden z powyzszych, ustaw kolor fioletowy
				rysujSzescian(rozmiar, katPojednczy);
				glPopMatrix();						//sciagnij polozenie ze stosu
			}
			glPopMatrix();							//sciagnij polozenie ze stosu
		}
		glPopMatrix();								//sciagnij polozenie ze stosu
	}
	
	if (mode == 1)													//jezeli tryb1 obracanie calego szyku
	{
		katCalosc = katCalosc + predkoscKatCalosc;					//zwieksz kat obrotu
		if (katCalosc > 360)										//jezeli kat wiekszy jak 360 odejmij 360
			katCalosc = katCalosc - 360;
		if (katCalosc < 0)										    //jezeli kat mniejszy jak 0 dodaj 360
			katCalosc = katCalosc + 360;
	}
	else if (mode == 2)												//jezeli tryb2 obracanie pojedynczegp szescianu
	{
		katPojednczy = katPojednczy + predkoscKatPojedynczy;		//zwieksz kat obrotu
		if (katPojednczy > 360)										// jezeli kat wiekszy jak 360 odejmij 360
			katPojednczy = katPojednczy - 360;
		if (katPojednczy < 0)										// jezeli kat mniejszy jak 0 dodaj 360
			katPojednczy = katPojednczy + 360;
	}
	else if (mode == 3)												//jezeli tryb3 powiekszanie i zmniejszanie szyku
	{
		if (powiekszanie)											//jezeli powiekszanie = true zwiekszamy odleglosc pomiedzy szescianami
			odleglosc = odleglosc + predkoscOdleglosc;
		else														//jezeli powiekszanie = false zwiekszamy odleglosc pomiedzy szescianami
			odleglosc = odleglosc - predkoscOdleglosc;

		if (odleglosc >= odlegloscMax || odleglosc <= odlegloscMin)  //je¿eli odleglosc przekroczyla zakres zacznij zwiekszac/zmniejszac
			powiekszanie = !powiekszanie;
	}
	else if (mode == 4)												//jezeli tryb4 zmiana koloru
	{
		if (k0)														//jezeli k0 = true (skladowa koloru [0] rosnie)
		{
			kolory[0] = kolory[0] + predkoscKolor;					//zwieksz wartosc skladowej
			if (kolory[0] > 1.0f)									// jezeli wieksza jak 1, ustaw na 1 i zacznij zmniejszac
			{
				kolory[0] = 1.0f;
				k0 = !k0;
			}
		}
		else														//jezeli k0 = false (skladowa koloru [0] maleje)
		{
			kolory[0] = kolory[0] - predkoscKolor;					//zmniejsz wartosc skladowej
			if (kolory[0] < 0.0f)									//jezeli skladowa mniejsza od 0, ustaw na 0 i zacznij zwiekszac
			{
				kolory[0] = 0.0f;
				k0 = !k0;
			}
		}
		if (k1)
		{
			kolory[1] = kolory[1] + predkoscKolor;
			if (kolory[1] > 1.0f)
			{
				kolory[1] = 1.0f;
				k1 = !k1;
			}
		}
		else
		{
			kolory[1] = kolory[1] - predkoscKolor;
			if (kolory[1] < 0.0f)
			{
				kolory[1] = 0.0f;
				k1 = !k1;
			}
		}
		if (k2)
		{
			kolory[2] = kolory[2] + predkoscKolor;
			if (kolory[2] > 1.0f)
			{
				kolory[2] = 1.0f;
				k2 = !k2;
			}
		}
		else
		{
			kolory[2] = kolory[2] - predkoscKolor;
			if (kolory[2] < 0.0f)
			{
				kolory[2] = 0.0f;
				k2 = !k2;
			}
		}
	}
	return TRUE;										// Everything Went OK
}

// zamkniecie okna OpenGL
GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

// utworzenie okna OpenGL
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;				// Set Top Value To 0
	WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

	fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu
	wc.lpszClassName = "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}

	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle = WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		"OpenGL",							// Class Name
		title,								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

// obs³uguje komunikaty systemu Windows
LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
	UINT	uMsg,			// Message For This Window
	WPARAM	wParam,			// Additional Message Information
	LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
	case WM_ACTIVATE:							// Watch For Window Activate Message
	{
		if (!HIWORD(wParam))					// Check Minimization State
		{
			active = TRUE;						// Program Is Active
		}
		else
		{
			active = FALSE;						// Program Is No Longer Active
		}

		return 0;								// Return To The Message Loop
	}

	case WM_SYSCOMMAND:							// Intercept System Commands
	{
		switch (wParam)							// Check System Calls
		{
		case SC_SCREENSAVE:					// Screensaver Trying To Start?
		case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
			return 0;							// Prevent From Happening
		}
		break;									// Exit
	}

	case WM_CLOSE:								// Did We Receive A Close Message?
	{
		PostQuitMessage(0);						// Send A Quit Message
		return 0;								// Jump Back
	}

	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
		keys[wParam] = TRUE;					// If So, Mark It As TRUE
		return 0;								// Jump Back
	}

	case WM_KEYUP:								// Has A Key Been Released?
	{
		keys[wParam] = FALSE;					// If So, Mark It As FALSE
		return 0;								// Jump Back
	}

	case WM_SIZE:								// Resize The OpenGL Window
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;								// Jump Back
	}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// przetwarzanie komunikatów systemowych i wywo³ywanie funkcji rysuj¹cej scenê OpenGL
int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
	HINSTANCE	hPrevInstance,		// Previous Instance
	LPSTR		lpCmdLine,			// Command Line Parameters
	int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done = FALSE;								// Bool Variable To Exit Loop

	// Okno z informacja o zmianie trybow 
	MessageBox(NULL, "Przelaczanie trybow - klawisze: 1, 2, 3, 4", "Przelaczanie trybow animacji", MB_OK);


	// Sprawdzanie czy pytac o tryb okienkowy - jesli nie to male okno -----------------------------------------
	if (pytajFullScreen == TRUE)
	{
		// Ask The User Which Screen Mode They Prefer
		if (MessageBox(NULL, "Would You Like To Run In Fullscreen Mode?", "Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			fullscreen = FALSE;							// Windowed Mode
		}
	}
	else if (pytajFullScreen == FALSE)
	{
		fullscreen = FALSE;
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("Program wykonuj¹cy rozne animacjê na grupie szescianów", 640, 480, 16, fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}
	int lastTick = 0;
	while (!done)									// Loop That Runs While done=FALSE
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
			{
				done = TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active)								// Program Active?
			{
				if (keys[VK_ESCAPE])				// Was ESC Pressed?
				{
					done = TRUE;						// ESC Signalled A Quit
				}
				else								// Not Time To Quit, Update Screen
				{
					int tick = GetTickCount();
					if (tick - lastTick >= 20)
					{
						DrawGLScene();					// Draw The Scene
						SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
						lastTick = tick;
					}

				}
			}
			//zmiana trybu okienkowego klawiszem F1
			 
			//if (keys[VK_F1])						// Is F1 Being Pressed?
			//{
			//	keys[VK_F1] = FALSE;					// If So Make Key FALSE
			//	KillGLWindow();						// Kill Our Current Window
			//	fullscreen = !fullscreen;				// Toggle Fullscreen / Windowed Mode
			//	// Recreate Our OpenGL Window
			//	if (!CreateGLWindow("Program wykonuj¹cy animacjê grupy szescianów", 640, 480, 16, fullscreen))
			//	{
			//		return 0;						// Quit If Window Was Not Created
			//	}
			//}
			
			if (keys['1'])						// jeszeli klawisz 1 wcisniety ustaw tryb na 1
			{
				keys['1'] = false;
				mode = 1;
			}
			if (keys['2'])						// jeszeli klawisz 1 wcisniety ustaw tryb na 1
			{
				keys['2'] = false;
				mode = 2;
			}
			if (keys['3'])						// jeszeli klawisz 1 wcisniety ustaw tryb na 1
			{
				keys['3'] = false;
				mode = 3;
			}
			if (keys['4'])						// jeszeli klawisz 1 wcisniety ustaw tryb na 1
			{
				keys['4'] = false;
				mode = 4;
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}

