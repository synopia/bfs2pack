/**
BFS2PACK.EXE
 - unpacks flatout2 bfs archives
 - packs flatout2 bfs archives
 - neat gui for all noobs that dont know what a console window is... ;-)




file: main.cpp
the win32 main program, shows gui, processes user input
almost all content of this file is taken from John Edwards' oggdropXP


Copyright (C) 2006 Paul Fritsche <paul.fritsche@gmx.net>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/


#include <windows.h>
#include <tchar.h>
#ifdef _MMGR
#include "mmgr.h"
#endif

#include "resources.h"
#include "XBrowseForFolder.h"
#include "bfs.h"
#include "pack.h"
#include "unpack.h"
using namespace std;
using namespace bfs;

// WHY THE HELL, there are no such defines in the WinAPI???
#define LOSHORT(l)           ((SHORT)(l))
#define HISHORT(l)           ((SHORT)(((DWORD)(l) >> 16) & 0xFFFF))

// oki i really LOVE win api for such calls
#define CREATEFONT(sz) \
	CreateFont((sz), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
		VARIABLE_PITCH | FF_SWISS, "")


// some gui-globals
HMENU menu; // our context menu
HINSTANCE hinst; // our application instance
RECT bar1, bar2, vbrBR; // progressbars (1==current file, 2==global)
HFONT font2; // the smaller font for current file
int width, height; // size of our window
CRITICAL_SECTION mutex; // needed for working thread
int frame = 0;
int frameDir = 0;
HBITMAP hbm[12];

char bfsFile[MAX_PATH]; // the bfs file (source for unpacking, target for packing)
char bfsDir[MAX_PATH]; // the directory (source for packing, target for unpacking)

long work_thread(LPVOID arg ); // the thread itself

/**
  initializes the working thread
*/
void workthread_init()
{
     DWORD thread_id;
     HANDLE thand;
     InitializeCriticalSection(&mutex);
     thand = (HANDLE) CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)work_thread, NULL, 0, &thread_id );
     if( thand ==NULL )
     {
         // error!!
     }
     
 }
 
 void pack_loop()
 {
     string DIR = string(bfsDir);
     string FILE = string(bfsFile);
     action = reading;
     Pack pack( DIR, FILE );
     action = packing;
     pack.readDir();
     pack.open();
     pack.writeHeader();
     pack.writeFilenames();
 	 while( pack.hasNextFile() )
 	 {
 		pack.packFile( zip_all );
	 }
	 pack.finalize();
     action = none;
	 setPgrEnd( "done" );
 }
 
 void unpack_loop()
 {
     string DIR = string(bfsDir);
     string FILE = string(bfsFile);
	 Unpack unpack( FILE, DIR );
	 if( !unpack.test() )
	 {
	  	 unpack.close();
	  	 // error
	  	 return;
	 }
	 unpack.readFilenames();
 	 for( int i=0;i<unpack.getNumberOfFiles();i++ )
 	 {
	  	 unpack.unpackFile( i );
     }

	 action = none;	   	  
	 setPgrEnd( "done" );
 }
 
/**
  the working thread, it just idles if no files are dragged...
*/
long work_thread(LPVOID arg )
{
     while(1==1)
     {
          // default priority
          SetPriorityClass( GetCurrentProcess(), NORMAL_PRIORITY_CLASS );
          
          // do action
          if( action==packing )
              pack_loop();
          else if (action==unpacking)
              unpack_loop();
          
          // finished + sleeping
          action = none;
          Sleep(500);
     }
     DeleteCriticalSection(&mutex);
     return 0;
 }

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
/*  Declare About procedure */
BOOL CALLBACK AboutProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

/*  Make the class name into a global variable  */
char szClassName[ ] = "bfs2packGui";


/**** MAIN 

****/
int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
/*     strcpy(bfsDir,"E:\\Documents and Settings\\synopia\\My Documents\\projects\\private\\workspace\\bfs2pack7\\data");              
     strcpy(bfsFile, "E:\\Documents and Settings\\synopia\\My Documents\\projects\\private\\workspace\\bfs2pack7\\data.bfs");
     ofstream temp("E:\\Documents and Settings\\synopia\\My Documents\\projects\\private\\workspace\\bfs2pack7\\data\\temp", ios::binary|ios::trunc);
     temp.write( bfsDir, strlen(bfsDir) );
     temp.close();
     pack_loop();
     remove("E:\\Documents and Settings\\synopia\\My Documents\\projects\\private\\workspace\\bfs2pack7\\data\\temp");
     strcpy(bfsFile, "E:\\Documents and Settings\\synopia\\My Documents\\projects\\private\\workspace\\bfs2pack7\\data2.bfs");
     pack_loop();
                    */
  //   exit(0);               
                    
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    const int width = 130, 
              height = 130;
    int x,y;
    hinst = hThisInstance;
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_HREDRAW | CS_VREDRAW;
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, MAKEINTRESOURCE(IDI_ICON1));
    wincl.hIconSm = LoadIcon (NULL, MAKEINTRESOURCE(IDI_ICON1));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Karoks bfs2pack",       /* Title Text */
           WS_POPUP|WS_DLGFRAME, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           width,                 /* The programs width */
           height,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);
    UpdateWindow( hwnd );
    
    font2 = CREATEFONT(10);
    
    for( frame = 0; frame<12; frame++ )
    {
         hbm[frame] = (HBITMAP)LoadImage( hinst, MAKEINTRESOURCE(IDB_BM01+frame), IMAGE_BITMAP, 0,0,LR_CREATEDIBSECTION);
    }
    frame = 0;
    
    SetTimer( hwnd, 1, 80, NULL);
    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/** 
  this handles the dropping of files into the window
*/
void HandleDrag(HWND hwnd, HDROP hDrop )
{
     // if we are currently working, no new files are accepted.. iam just to lazy to build a working queue!
     if( action!=none )
     {
         MessageBox(hwnd, "bfs2pack is currently working. Please wait until it is finished!", NULL, MB_OK | MB_ICONEXCLAMATION);
         return;
     }
     int cFiles;
     char szFile[MAX_PATH], *ext;
     // take the files
     cFiles = DragQueryFile( hDrop, 0xFFFFFFFF, NULL, 0 );
     bool error = false;
     // we only accept ONE file or directory
     if( cFiles>1 )
     {
         error = true;
     } else
     {
        DragQueryFile(hDrop, 0, szFile, sizeof(szFile) );
        // we have a file/dir with .bfs at the end
        if( (ext=strrchr(szFile, '.')) && strcasecmp(ext, ".bfs")==0 )
          {
              // unpacking
              TCHAR szTemp[MAX_PATH];
              TCHAR szPath[MAX_PATH];
              // get the path
              strcpy( szPath, szFile );
              if( ext=strrchr(szPath, '\\' )) *ext=0;
              
              // let the user select output dir, initial directory is where the bfs is located
              if( XBrowseForFolder(hwnd, szPath, szTemp, MAX_PATH ) )
              {              
                  // let the working thread begin...
                  strcpy( bfsFile, szFile );
                  strcpy( bfsDir, szTemp );
                  action = unpacking;
                  frameDir=-1;
              } else
              {
                  // user pressed cancel
              }
              
          } 
          else
          {
              // packing
              // HACK: try opening a directory should not work!
              //       i googled for about 1hr to find a better solution...
              FILE *f = fopen(szFile, "rb");
              if( f==NULL ) 
              {
                  // we cannot open the "file" and suggest its a directory...
                  // so show the save dialog
                  OPENFILENAME ofn;
                  TCHAR szFilter[] = _T("Flatout2 archive (*.bfs)\0*.bfs\0");
                  ZeroMemory(&ofn, sizeof(OPENFILENAME) );
                  ofn.lStructSize = sizeof( OPENFILENAME );
                  ofn.hwndOwner = hwnd;
                  ofn.lpstrFilter = szFilter;
                  ofn.nMaxFile = MAX_PATH;
                  ofn.nMaxFileTitle = MAX_PATH;
                  ofn.lpstrDefExt = _T("bfs");
                  char szTemp[MAX_PATH];
                  strcpy( szTemp, szFile);
                  strcat( szTemp, ".bfs" );
                  ofn.lpstrFile = szTemp;
                  ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
                  
                  if( GetSaveFileName( &ofn ) )
                  {
                      strcpy( bfsFile, ofn.lpstrFile );
                      strcpy( bfsDir, szFile );
                      
                      // ask user to use secure mode (dont zip any file
                      zip_all = false;
                      if( MessageBox(hwnd, "Use securemode (no zipping of files) to create a more compatible bfs archive? Choosing NO results in a much smaller bfs, but it may not work with flatout...", NULL, MB_YESNO | MB_ICONQUESTION |MB_APPLMODAL|MB_SETFOREGROUND)==IDNO )
                      {
                          zip_all = true;
                      } 
                      // start thread
                      action = packing;
                      frameDir = 1;
                   } 
              }
              else 
              {
                  error = true;
                  fclose(f);
              }
          }
     }
     
     // show error if some occured
     if( error )
     {
         MessageBox( hwnd, "Drop either a bfs (for unpacking) or a data directory (for packing) onto this window!", NULL, MB_OK|MB_ICONEXCLAMATION);
     }

     DragFinish(hDrop);
 }


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc, hmem;
    static HDC offscreen;
    PAINTSTRUCT ps;
    HDC desktop;
    HBITMAP hbitmap;    
    POINT point;
    static int dragging = 0;
    static POINT start;
    RECT rect;
    HDROP hdrop;
    HFONT dfltFont;
    int dfltBGMode;
    double percomp;
    BITMAP bm;
    
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            // get the context menu 
            menu = LoadMenu(hinst, MAKEINTRESOURCE(IDR_MENU1));
            menu = GetSubMenu(menu, 0 );

            // initialize the offscreen                
            offscreen = CreateCompatibleDC(NULL);
            desktop = GetDC(GetDesktopWindow());
            hbitmap = CreateCompatibleBitmap(desktop, 200, 200 );
            ReleaseDC(GetDesktopWindow(), desktop);
            SelectObject( offscreen, hbitmap );
            
            // initialize the working thread
            workthread_init();
            
            // and accept draging of files
            DragAcceptFiles(hwnd, TRUE);
            return 0;  
        case WM_PAINT:
             // paint loop
             hdc = BeginPaint(hwnd, &ps);
             GetClientRect(hwnd, &rect );
             width = rect.right+1;
             height = rect.bottom+1;
             FillRect( offscreen, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH) );
             // display current action
             char text[128];
             if( action==none )
                 strcpy(text, "Drop Files Here..");
             else if (action==packing )
                 strcpy(text, "Packing..." );
             else if( action==unpacking)
                 strcpy(text, "Unpacking...");
             DrawText(offscreen, text, -1, &rect, DT_SINGLELINE | DT_CENTER );
             
             hmem = CreateCompatibleDC(offscreen);
             SelectObject(hmem, hbm[frame]);
             GetObject(hbm[frame], sizeof( BITMAP ), &bm);
             BitBlt(offscreen, width/2-33, height/2-46, bm.bmWidth, bm.bmHeight,hmem, 0,0,SRCCOPY);
             DeleteDC(hmem);
             
             // calculate current progess
             percomp = ((double)(numfiles) - (1-file_complete))/(double)totalfiles;
             SetRect( &vbrBR, 0, height-45, width, height-19 );
             dfltBGMode = SetBkMode(offscreen, TRANSPARENT );
             dfltFont = (HFONT)SelectObject( offscreen, font2 );
             // display progress bars
             SetRect(&bar1, 0, height-23, (int)(file_complete*width), height-13);
             SetRect(&bar2, 0, height-12, (int)(percomp * width), height-2);
             
             FillRect( offscreen, &bar1, (HBRUSH)GetStockObject(LTGRAY_BRUSH) );
             FillRect( offscreen, &bar2, (HBRUSH)GetStockObject(DKGRAY_BRUSH) );
             
             // display current file
//             if( action!=none )
             {
                 char caption[256];
                 sprintf(caption, "%s", current_file.c_str());
                 DrawText( offscreen, caption, -1, &bar1, DT_SINGLELINE|DT_LEFT );
             } 
             
             // copy offscreen to actual window
             SelectObject( offscreen, dfltFont );
             SetBkMode(offscreen, dfltBGMode);
             BitBlt(hdc, 0,0, width, height, offscreen, 0,0, SRCCOPY );
             EndPaint(hwnd, &ps) ;
             DefWindowProc(hwnd, message, wParam, lParam );
             return 0;
        case WM_TIMER:
             // update timer
             if( frame!=0 || action!=none )
             {
			  	 frame+=frameDir;
                 if( frame>11 ) frame = 0;
                 if( frame<0 ) frame = 11;
                 if( action==none && frame==0 ) frameDir=0;
			 }
			  	 
             GetClientRect(hwnd, &rect);
             InvalidateRect(hwnd, &rect, FALSE);
             return 0; 
        case WM_LBUTTONDOWN:
             // dragging window with left mouse
            start.x = LOWORD(lParam);
            start.y = HIWORD(lParam);
            ClientToScreen(hwnd, &start );
            GetWindowRect( hwnd, &rect );
            start.x -= rect.left;
            start.y -= rect.top;
            dragging = 1;
            SetCapture(hwnd);
            return 0;
        case WM_LBUTTONUP:
            if( dragging ){
                dragging = 0;
                ReleaseCapture();
            } 
            return 0;
        case WM_MOUSEMOVE:
            if( dragging ){
                point.x = LOSHORT( lParam );
                point.y = HISHORT( lParam );
                ClientToScreen(hwnd, &point );
                SetWindowPos( hwnd, 0, point.x - start.x, point.y - start.y, 0, 0, SWP_NOSIZE| SWP_NOZORDER| SWP_SHOWWINDOW );
            }
            return 0;
        case WM_CAPTURECHANGED:
            if( dragging ){
                dragging = 0;
                ReleaseCapture();
            } 
            return 0;
        case WM_CONTEXTMENU:
             // show context menu with right mouse
            point.x = LOWORD(lParam);
            point.y = HIWORD(lParam);
            TrackPopupMenu(menu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL );
            return 0; 
        case WM_COMMAND:
             // the menu items from the context menu
             switch( LOWORD(wParam)){
                     case IDM_QUIT:
                          PostQuitMessage(0);
                          break;
                     case IDM_ABOUT:
                          DialogBox( hinst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutProc);
                          break;
                          
             }
             return 0;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        case WM_DROPFILES:
             // called when files are dropped into the window
             hdrop = (HDROP)wParam;
             HandleDrag(hwnd, hdrop);
             return 0;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

/*
 *  About parameters dialog procedures.
 */

BOOL CALLBACK AboutProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_INITDIALOG: {
 
//			HICON hBgIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_ICON1));
//			PostMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hBgIcon);

			SetDlgItemText(hwndDlg, IDC_COMPILE_DATE, "Compiled: " __DATE__);
		}
		return TRUE;

		case WM_CLOSE:

			// Closing the Dialog behaves the same as Cancel               
			PostMessage(hwndDlg, WM_COMMAND, IDCANCEL, 0);
			break; // End of WM_CLOSE                                      

		case WM_COMMAND: 

			switch (LOWORD(wParam)) {
				case IDCANCEL:
					EndDialog(hwndDlg, FALSE);
					break;
			}
			break;
	}
return FALSE; 
}
