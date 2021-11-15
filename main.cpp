#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL\gl.h>			// Header File For The OpenGL32 Library

int Running;
HDC hDC;
GLuint shaderProgram = 0;
GLuint VAO = 0;

/*
can get most #defines from https://www.khronos.org/registry/OpenGL/api/GL/glext.h
#define GL_MAJOR_VERSION    0x821B //from OpenGL 3.0 
for doing:
    int ver;
    glGetIntegerv(GL_MAJOR_VERSION, &ver);
instead of:
    int ver = glGetString(GL_VERSION)[0] - '0'; 
*/

//from OpenGL 2.0
#define GL_COMPILE_STATUS   	0x8B81
#define GL_LINK_STATUS      	0x8B82
#define GL_VERTEX_SHADER    	0x8B31
#define GL_FRAGMENT_SHADER  	0x8B30
#define GL_ARRAY_BUFFER     	0x8892
#define GL_ELEMENT_ARRAY_BUFFER	0x8893
#define GL_STATIC_DRAW      	0x88E4

#ifdef _WIN64
typedef signed   long long int khronos_ssize_t;
#else
typedef signed   long  int     khronos_ssize_t;
#endif

typedef khronos_ssize_t GLsizeiptr;
typedef char GLchar;
//for windows need __stdcall for correct calling convention, 
// but can change all functions to be called with __stdcall in project settings, so you don't need to put __stdcall
GLuint (__stdcall *glCreateShader)(GLenum shaderType) = NULL;
void   (__stdcall *glShaderSource)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length) = NULL;
void   (__stdcall *glCompileShader)(GLuint shader) = NULL;
void   (__stdcall *glGetShaderiv)(GLuint shader, GLenum pname, GLint* params) = NULL;
void   (__stdcall *glGetShaderInfoLog)(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog) = NULL;
GLuint (__stdcall *glCreateProgram)(void) = NULL;
void   (__stdcall *glAttachShader)(GLuint program, GLuint shader) = NULL;
void   (__stdcall *glLinkProgram)(GLuint program) = NULL;
void   (__stdcall *glGetProgramiv)(GLuint program, GLenum pname,GLint* params) = NULL;
void   (__stdcall *glGetProgramInfoLog)(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog) = NULL;
void   (__stdcall *glDeleteShader)(GLuint shader) = NULL;
void   (__stdcall *glUseProgram)(GLuint program) = NULL;
void   (__stdcall *glGenVertexArrays)(GLsizei n, GLuint* arrays) = NULL; 
void   (__stdcall *glGenBuffers)(GLsizei n, GLuint* buffers) = NULL;
void   (__stdcall *glBindVertexArray)(GLuint array) = NULL; 
void   (__stdcall *glBindBuffer)(GLenum target, GLuint buffer) = NULL;
void   (__stdcall *glBufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage) = NULL;
void   (__stdcall *glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) = NULL;
void   (__stdcall *glEnableVertexAttribArray)(GLuint index) = NULL;

//void glDisableVertexAttribArray(GLuint index); //used to disable VBO pointed to by active VAO at VAO index

/*


glGetUniformLocation

//be careful with bools https://community.khronos.org/t/boolean-branches-in-shader/74598/3
//for int and bool (cast to int) //https://community.khronos.org/t/problem-with-uniform-boolean-var-to-turn-lighting-on-off/1619/2
glUniform1i
//for float
glUniform1f

*/
    
void WindowsDebugPrintOpenGLVersion()
{
    OutputDebugStringA("OPENGL VERSION: ");
    OutputDebugStringA((const char*)glGetString(GL_VERSION));
    OutputDebugStringA("\n");
}

/* make sure you are in wgl Context!*/
inline void loadGLFuncPtrs()
{
    //load needed version specific opengl funcs
    int ver = glGetString(GL_VERSION)[0] - '0'; //is opengl 2.0 compatible
    if (ver > 4) ver = 4; //future proofing
    switch (ver)
    {
        case 4: 
        {
            //opengl version 4.0 required
        }
        case 3: 
        {
            //opengl version 3.0 required
            glGenVertexArrays = (void(__stdcall*)(GLsizei n, GLuint * arrays))((void*)wglGetProcAddress("glGenVertexArrays"));
            glBindVertexArray = (void(__stdcall*)(GLuint array))((void*)wglGetProcAddress("glBindVertexArray"));
        }
        case 2:
        {
            //opengl version 2.0 required
            glCreateShader = (GLuint(__stdcall *)(GLenum shaderType))((void*)wglGetProcAddress("glCreateShader"));
            glShaderSource = (void (__stdcall*)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length))((void*)wglGetProcAddress("glShaderSource"));
            glCompileShader = (void (__stdcall*)(GLuint shader))((void*)wglGetProcAddress("glCompileShader"));
            glGetShaderiv = (void (__stdcall*)(GLuint shader, GLenum pname, GLint* params))((void*)wglGetProcAddress("glGetShaderiv"));
            glGetShaderInfoLog = (void (__stdcall*)(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog))((void*)wglGetProcAddress("glGetShaderInfoLog"));
            glCreateProgram = (GLuint(__stdcall*)(void))((void*)wglGetProcAddress("glCreateProgram"));
            glAttachShader = (void (__stdcall*)(GLuint program, GLuint shader))((void*)wglGetProcAddress("glAttachShader"));
            glLinkProgram = (void (__stdcall*)(GLuint program))((void*)wglGetProcAddress("glLinkProgram"));
            glGetProgramiv = (void (__stdcall*)(GLuint program, GLenum pname, GLint* params))((void*)wglGetProcAddress("glGetProgramiv"));
            glGetProgramInfoLog = (void (__stdcall*)(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog))((void*)wglGetProcAddress("glGetProgramInfoLog"));
            glDeleteShader = (void (__stdcall*)(GLuint shader))((void*)wglGetProcAddress("glDeleteShader"));
            glUseProgram = (void(__stdcall*)(GLuint program))((void*)wglGetProcAddress("glUseProgram"));
            glGenBuffers = (void(__stdcall*)(GLsizei n, GLuint * buffers))((void*)wglGetProcAddress("glGenBuffers"));
            glBindBuffer = (void(__stdcall*)(GLenum target, GLuint buffer))((void*)wglGetProcAddress("glBindBuffer"));
            glBufferData = (void(__stdcall*)(GLenum target, GLsizeiptr size, const void* data, GLenum usage))((void*)wglGetProcAddress("glBufferData"));
            glVertexAttribPointer = (void(__stdcall*)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer))((void*)wglGetProcAddress("glVertexAttribPointer"));
            glEnableVertexAttribArray = (void(__stdcall*)(GLuint index))((void*)wglGetProcAddress("glEnableVertexAttribArray"));
        } break;
        case 1:
            OutputDebugStringA("Compiled Shaders NOT SUPPORTED!\n");
            break;
        default:
            OutputDebugStringA("UNKNOWN OPENGL MAJOR VERSION");
    }
    return;
}


/* shaderType is one of these GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER or GL_FRAGMENT_SHADER */
/* just give shader a name for error msgs :) */
inline GLuint LoadShaderFromFile(const char* fileName, GLenum shaderType, const char* shaderName)
{
    GLuint shaderDescriptor = 0;
    size_t  numBytes = 0;
    FILE* file;
    errno_t err = fopen_s(&file,fileName, "rb");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        numBytes = ftell(file);
        if (numBytes == 0)
        {
            OutputDebugStringA("FILE IS EMPTY: ");
            OutputDebugStringA(fileName);
            OutputDebugStringA("\n");
            return 0;
        }
        fseek(file, 0, SEEK_SET);

        char * contents = (char*) malloc(numBytes+1); //file size plus null terminator
        if (contents == NULL)
        {
            OutputDebugStringA("ERROR Ran out of memory on file: ");
            OutputDebugStringA(fileName);
            OutputDebugStringA("\n");
            exit(-1);
        }
        fread((char*)contents, numBytes, 1, file);
        contents[numBytes] = '\0'; //add null terminator to end of file
        fclose(file);
        shaderDescriptor = glCreateShader(shaderType);
        glShaderSource(shaderDescriptor, 1, (const GLchar**) &contents, NULL); //copies source code to shader obj, 1 is the number of elements in array of shaders and Null refers to an array of lengths of strings if &contents was more than 1, &contents refers to an array of 1,
        free(contents); //don't need string anymore
        glCompileShader(shaderDescriptor);
        GLint success;
        glGetShaderiv(shaderDescriptor, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[1024];
            glGetShaderInfoLog(shaderDescriptor, 1024, NULL, infoLog);
            OutputDebugStringA("ERROR SHADER_COMPILATION_ERROR of type: ");
            OutputDebugStringA(shaderName);
            OutputDebugStringA("\n");
            OutputDebugStringA(infoLog);
            OutputDebugStringA("\n");
            exit(-1);
        }

    }
    else
    {
        OutputDebugStringA("ERROR Cannot open shader: ");
        OutputDebugStringA(fileName);
        OutputDebugStringA("\n");
        exit(-1);
    }
    return shaderDescriptor;

}

/* don't forget to use after compiling*/
inline GLuint compileShaderProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader); //attach order does matter
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        OutputDebugStringA("ERROR PROGRAM_LINKING_ERROR of type: PROGRAM\n");
        OutputDebugStringA(infoLog);
        OutputDebugStringA("\n");
        exit(-1);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}


void drawScene(GLuint shaderProgram)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    //can do glBindVertexArray(VAO); But it is already bound
    // GL_TRIANGLES is the primitive type
    // 6 is num of vertices in the indicies to draw
    // GL_UNSIGNED_INT type of index
    // 0 is offset into index buffer
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    SwapBuffers(hDC);

}


int logWindowsError(const char* msg)
{
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
    OutputDebugStringA(msg);
    OutputDebugStringA((LPCTSTR)lpMsgBuf);

    LocalFree(lpMsgBuf);
    return -1;
}

LRESULT CALLBACK
Win32MainWindowCallback(
    HWND Window,   //HWND a handle to a window
    UINT Message,  //System defined message, Window will call us and ask us to respond it
    WPARAM WParam, //
    LPARAM LParam) //
{
    LRESULT Result = 0;
    switch (Message)
    {
    case WM_SIZE:
    {
        glViewport(0, 0, LOWORD(LParam), HIWORD(LParam)); //needed for window resizing
    }break;

    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)LParam;
        lpMMI->ptMinTrackSize.x = 300;
        lpMMI->ptMinTrackSize.y = 300;
    }break;

    case WM_CLOSE: //when user clicks on the X button on the window
    {
        Running = 0;
    } break;

    case WM_PAINT: //to allow us to update the window
    {
        drawScene(shaderProgram);
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(Window, &Paint); //will fill out struct with information on where to paint
        EndPaint(Window, &Paint); //closes paint area
    } break;

    default:
        Result = DefWindowProc(Window, Message, WParam, LParam); //call windows to handle default behavior of things we don't handle
    }

    return Result;
}


//This is how it is #define WINAPI __stdcall
//#define CALLBACK __stdcall
//#define APIENTRY WINAPI

/*
https://docs.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point
How does the compiler know to invoke wWinMain instead of the standard main function? 
What actually happens is that the Microsoft C runtime library (CRT) provides an implementation of main that calls either WinMain or wWinMain.
*/
//int main()  // must use for console sub system
int APIENTRY WinMain(
    _In_ HINSTANCE Instance,
    _In_opt_ HINSTANCE PrevInstance,
    _In_ LPSTR CommandLine,
    _In_ int ShowCode)
{
    WNDCLASSEX WindowClass;
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; //https://devblogs.microsoft.com/oldnewthing/20060601-06/?p=31003
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = Instance; //If using int main() and don't have Instance, you can call GetModuleHandle(NULL); instead
    WindowClass.hIcon = LoadIcon(0, IDI_APPLICATION); //IDI_APPLICATION: Default application icon, 0 means use a default Icon
    WindowClass.hCursor = LoadCursor(0, IDC_ARROW); //IDC_ARROW: Standard arrow, 0 means used a predefined Cursor
    WindowClass.hbrBackground = NULL; 
    WindowClass.lpszMenuName = NULL;	// No menu 
    WindowClass.lpszClassName = "WindowTestClass"; //name our class
    WindowClass.hIconSm = NULL; //can also do default Icon here? will NULL be default automatically?
    if (RegisterClassEx(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(0, WindowClass.lpszClassName, "Window Test",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, Instance, NULL);

        if (WindowHandle)
        {
            //  We need to make sure the window create in a suitable DC format

            PIXELFORMATDESCRIPTOR pfd = {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
                PFD_TYPE_RGBA,												// The kind of framebuffer. RGBA or palette.
                32,															// Colordepth of the framebuffer.
                0, 0, 0, 0, 0, 0, 0, 0,
                0,
                0, 0, 0, 0,
                24,															// Number of bits for the depthbuffer
                8,															// Number of bits for the stencilbuffer
                0,															// Number of Aux buffers in the framebuffer.
                PFD_MAIN_PLANE,
                0,
                0, 0, 0
            };

            hDC = GetDC(WindowHandle);				// Get a DC for our window
            int letWindowsChooseThisPixelFormat = ChoosePixelFormat(hDC, &pfd); // Let windows select an appropriate pixel format
            if (letWindowsChooseThisPixelFormat)
            {
                if (SetPixelFormat(hDC, letWindowsChooseThisPixelFormat, &pfd))
                { // Try to set that pixel format
                    HGLRC ourOpenGLRC = wglCreateContext(hDC);
                    if (ourOpenGLRC)
                    {
                        wglMakeCurrent(hDC, ourOpenGLRC); // Make our render context current
                        loadGLFuncPtrs(); // MUST BE AFTER wgkMakeCurrent! for functions to be loaded for correct context
                        GLuint vShad = LoadShaderFromFile("vertexShader.vs", GL_VERTEX_SHADER, "VERTEX SHADER");
                        GLuint fShad = LoadShaderFromFile("fragmentShader.fs", GL_FRAGMENT_SHADER, "FRAGMENT SHADER");
                        shaderProgram = compileShaderProgram(vShad, fShad);


                        float vertices[] = 
                        {
                            // positions         // colors
                             0.5f,  0.5f, 0.0f,  0.5f, 0.5f, 0.5f,
                             0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
                            -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
                            -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
                        };

                        unsigned int indicies[] =
                        {
                        	0, 1, 3,
                        	1, 2, 3
                        };

                        GLuint VBO;
                        glGenVertexArrays(1, &VAO); //we just need 1, if you did a number greater than 1, it will populate an array
                        glGenBuffers(1, &VBO);
                        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
                        glBindVertexArray(VAO); //can only bind one VAO at a time

                        glBindBuffer(GL_ARRAY_BUFFER, VBO);
                        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                        // position attribute
                        //https://stackoverflow.com/questions/14249634/opengl-vaos-and-multiple-buffers
                        // - first arg sets bound vertex buffer to 0th slot of vertex array, meaning in shader (location = 0) is first slot of VAO
                        //Think of it like this. glBindBuffer sets a global variable (GL_ARRAY_BUFFER), then glVertexAttribPointer reads that global variable and stores it in the VAO. 
                        // Changing that global variable after it's been read doesn't affect the VAO. You can think of it that way because that's exactly how it works.
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
                        glEnableVertexAttribArray(0);
                        // color attribute
                        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
                        glEnableVertexAttribArray(1);


                        //FOR EAO's
                        // The index buffer binding is stored within the VAO. If no VAO is bound, then you cannot bind a buffer object to GL_ELEMENT_ARRAY_BUFFER (this might not be true, so verify). Meaning GL_ELEMENT_ARRAY_BUFFER is not global like GL_ARRAY_BUFFER 
                        GLuint EBO;
                        glGenBuffers(1, &EBO);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

                        //glBindVertexArray(NULL); in a state machine unbind here

                        Running = 1;
                        while (Running)
                        {

                            MSG Message;
                            while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) //checks to see if windows wants to do something, returns 0 if no messages are present. PM_REMOVE: remove messages from queue
                            {
                                TranslateMessage(&Message);
                                DispatchMessage(&Message);
                            }

                            drawScene(shaderProgram);

                        }
                        //glDeleteProgram(shaderProgram); //maybe?
                        wglMakeCurrent(NULL, NULL);
                        wglDeleteContext(ourOpenGLRC);
                    }
                }
                else
                    return logWindowsError("Failed to Set Pixel Format for Window:\n");
            }
            else
                return logWindowsError("Windows Failed to choose Pixel Format:\n");
            ReleaseDC(WindowHandle, hDC); //need to close before logError if doing symmetric programming
            DestroyWindow(WindowHandle);
        }
        else
            return logWindowsError("Failed to Instantiate Window Class:\n");
    }
    else
        return logWindowsError("Failed to Register Window Class:\n");
    return 0;
}