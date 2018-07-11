
#include <windows.h>
#include <cstdlib>

enum color
{
	black		= 0,
	darkgrey	= FOREGROUND_INTENSITY,
	darkgray	= FOREGROUND_INTENSITY,
	grey		= FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE,
	gray		= FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE,
	white		= FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	darkblue	= FOREGROUND_BLUE,
	darkgreen	= FOREGROUND_GREEN,
	darkcyan	= FOREGROUND_GREEN | FOREGROUND_BLUE,
	darkred		= FOREGROUND_RED,
	darkpurple	= FOREGROUND_RED   | FOREGROUND_BLUE,
	darkyellow	= FOREGROUND_RED   | FOREGROUND_GREEN,
	blue		= FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
	green		= FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	cyan		= FOREGROUND_GREEN | FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
	red			= FOREGROUND_RED   | FOREGROUND_INTENSITY,
	purple		= FOREGROUND_RED   | FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
	yellow		= FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
};

void SetColor(color fgColor)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); 
	if(hConsole) 
	    SetConsoleTextAttribute(hConsole, fgColor);
}

void CambiarTam(HWND hwnd, int ancho, int largo){
    RECT rect;
    GetWindowRect(hwnd,&rect);
    MoveWindow(hwnd,rect.left,rect.top,ancho,largo,true);
}

bool Conexion(SOCKET &s, char* ip, int puerto){
    WSADATA wsd;
    WSAStartup(0x0202,&wsd);
    sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(puerto);
    serv.sin_addr.s_addr=inet_addr(ip);
    s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (s == INVALID_SOCKET) return false;
    if (connect(s, (sockaddr*)&serv, sizeof(sockaddr)) == INVALID_SOCKET) return false;
	unsigned long  b = 1;
    ioctlsocket(s,FIONBIO,&b);
    return true;
}

HWND CrearVentana(char* titulo, HINSTANCE hInstance, WNDPROC wndproc){
    char szClassName[ ] = "WindowsApp"; 
    WNDCLASSEX wincl;
    HWND temp_window;
    
    wincl.hInstance = hInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = wndproc;     
    wincl.style = CS_DBLCLKS;                
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                
    wincl.cbClsExtra = 0;                     
    wincl.cbWndExtra = 0;                   
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wincl))
        return 0;
    temp_window = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           titulo,       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    return temp_window;
}
