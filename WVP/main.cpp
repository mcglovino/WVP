//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

//Global Declarations - Interfaces//
IDXGISwapChain* SwapChain; // pointer to swap back and front buffers, preventing scan lines where its bieng drawn (rendering is from back buffer)
ID3D11Device* Dev; // pointer to direct3D device interface
ID3D11DeviceContext* DevCon; // pointer to direct3D device context
ID3D11RenderTargetView* renderTargetView; //back buffer, a 2d texture, this is written to and the sent to be rendered on screen
ID3D11Buffer* squareIndexBuffer;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;
ID3D11Buffer* squareVertBuffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;
ID3D11InputLayout* vertLayout;
ID3D11Buffer* cbPerObjectBuffer;
//for textures
ID3D11ShaderResourceView* CubesTexture;
ID3D11SamplerState* CubesTexSamplerState;

//for frame rate stabilisation
static int TickCount;
static int TickCountT;

//for background colours
float red = 1.0f;
float green = 0.2f;
float blue = 0.5f;
int colourmodr = 1;
int colourmodg = 1;
int colourmodb = 1;

//Global Declarations - Others//
LPCTSTR WndClassName = L"firstwindow"; //making a class for the window
HWND hwnd = NULL; //handle to the window
HRESULT hr;

//window size
const int Width  = 1000;
const int Height = 1000;

//world view declarations
XMMATRIX WVP;
//cube objects
//XMMATRIX cube1;
//XMMATRIX cube2;
//XMMATRIX cube3;
//XMMATRIX cube4;
//XMMATRIX World;
XMMATRIX camView;
XMMATRIX camProjection;

XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;

//for translating, rotating and scaling world objects
XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
//float rot = 0.01f;

//Function Prototypes//
//initialize direct3D
bool InitializeDirect3d11App(HINSTANCE hInstance);
//releases objects to prevent memory leaks
void CleanUp();

bool InitScene();
void UpdateScene();
void DrawScene();

//initialize the window
bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed);
//keeps program running
int messageloop();

//initialize windows callack procedure
LRESULT CALLBACK WndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

//Create effects constant buffer's structure//
struct cbPerObject
{
	XMMATRIX  WVP;
};

cbPerObject cbPerObj;

//Vertex Structure and Vertex Layout (Input Layout)//
/*struct Vertex	//Overloaded Vertex Structure
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float cr, float cg, float cb, float ca)
		: pos(x,y,z), colour(cr, cg, cb, ca){}

	XMFLOAT3 pos;
	XMFLOAT4 colour;
};*/

struct Vertex	//Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v)
		: pos(x, y, z), texCoord(u, v) {}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
};
UINT numElements = ARRAYSIZE(layout);

//class
class _Object
{
public:

	//XMMATRIX WVP;
	XMMATRIX obj;
	float rot = 0.01f;
	float rotmod = 0.05f;
	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;

	float rotX  = 1;
	float rotY = 1;
	float rotZ = 0;

	float transX = 0;
	float transY = 0;
	float transZ = 0;


	void Scene()
	{
		//Create the vertex buffer
		Vertex v[] =
		{
			// Front Face
			Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
			Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
			Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
			Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

			// Back Face
			Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
			Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
			Vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
			Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

			// Top Face
			Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
			Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
			Vertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f),
			Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f),

			// Bottom Face
			Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
			Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
			Vertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f),
			Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f),

			// Left Face
			Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
			Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
			Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
			Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

			// Right Face
			Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
			Vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
			Vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
			Vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f),
		};

		//index list, verticies that make up faces
		DWORD indices[] = {
			// Front Face
			0,  1,  2,
			0,  2,  3,

			// Back Face
			4,  5,  6,
			4,  6,  7,

			// Top Face
			8,  9, 10,
			8, 10, 11,

			// Bottom Face
			12, 13, 14,
			12, 14, 15,

			// Left Face
			16, 17, 18,
			16, 18, 19,

			// Right Face
			20, 21, 22,
			20, 22, 23
		};

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA iinitData;

		iinitData.pSysMem = indices;
		Dev->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

		DevCon->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * 24;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = v;
		hr = Dev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

		//Set the vertex buffer
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &stride, &offset);

		//Create the Input Layout
		hr = Dev->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
			VS_Buffer->GetBufferSize(), &vertLayout);

		//Set the Input Layout
		DevCon->IASetInputLayout(vertLayout);

		//load texture
		hr = D3DX11CreateShaderResourceViewFromFile(Dev, L"smile.jpg",
			NULL, NULL, &CubesTexture, NULL);

		
	}
	void Update()
	{
		rot -= rotmod;
		if (rot > 6.26f)
		{
			rot = 0.0f;
		}

		XMVECTOR rotaxis = XMVectorSet(rotX, rotY, rotZ, 0.0f);

		//Reset obj
		obj = XMMatrixIdentity();

		//Define obj's world space matrix
		Rotation = XMMatrixRotationAxis(rotaxis, rot);
		Translation = XMMatrixTranslation(transX, transY, transZ);

		//Set obj's world space using the transformations
		obj = Translation * Rotation;
	}

	void Draw()
	{
		//Set the WVP matrix and send it to the constant buffer in effect file
		WVP = obj * camView * camProjection;
		cbPerObj.WVP = XMMatrixTranspose(WVP);
		DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
		//for textures
		DevCon->PSSetShaderResources(0, 1, &CubesTexture);
		DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

		//Draw the obj
		DevCon->DrawIndexed(36, 0, 0);
	}

	void setRot(float X, float Y, float Z, int rotmodsign)
	{
		rotX = X;
		rotY = Y;
		rotZ = Z;
		rotmod = rotmod * rotmodsign;
	}

	void setTranslate(float X, float Y, float Z)
	{
		transX = X;
		transY = Y;
		transZ = Z;
	}
};

//array of Objects
_Object Objs[9] = {};

//Main windows function
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine,
	int nShowCmd)
{

	if(!InitializeWindow(hInstance, nShowCmd, Width, Height, true))
	{
		MessageBox(0, L"Window Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if(!InitializeDirect3d11App(hInstance))	//Initialize Direct3D
	{
		MessageBox(0, L"Direct3D Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if(!InitScene())	//Initialize our scene
	{
		MessageBox(0, L"Scene Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	messageloop();

	CleanUp();    

	return 0;
}

//create and register windowbclass
bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed)
{
	typedef struct _WNDCLASS {
		UINT cbSize;
		UINT style;
		WNDPROC lpfnWndProc;
		int cbClsExtra;
		int cbWndExtra;
		HANDLE hInstance;
		HICON hIcon;
		HCURSOR hCursor;
		HBRUSH hbrBackground;
		LPCTSTR lpszMenuName;
		LPCTSTR lpszClassName;
	} WNDCLASS;

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	//redraws when window moved or resized
	wc.style = CS_HREDRAW | CS_VREDRAW;
	//pointer to function that processes windows messages
	wc.lpfnWndProc = WndProc;
	//extra bytes allocated after WNDCLASSEX
	wc.cbClsExtra = NULL;
	//extra bytes allocated after windows instance
	wc.cbWndExtra = NULL;
	//handle to curent windows application
	wc.hInstance = hInstance;
	//icon in top left (this isnt doing anything, is just controlled by wc.hIconsm below)
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	//cursor
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	//background colour, ive overwritten this however
	//still needed to register class
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	//name class here
	wc.lpszClassName = WndClassName;
	//controlls icon on taskbar (and top left)
	wc.hIconSm = LoadIcon(NULL, IDI_SHIELD);

	//register clqass, if fails pulls up and error
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error registering class",	
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	//create the wondow
	hwnd = CreateWindowEx(
		NULL,
		//name of class window uses, registered above
		WndClassName,
		//text that will appear in the title bar
		L"Work please",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	//ceck to see if window was created
	if (!hwnd)
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	//shows the window
	ShowWindow(hwnd, ShowWnd);
	//refreshes
	UpdateWindow(hwnd);

	return true;
}

//initializes direct3D
bool InitializeDirect3d11App(HINSTANCE hInstance)
{
	//Describe our SwapChain Buffer
	DXGI_MODE_DESC bufferDesc;
	//ensures is completely cleared out
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	//size
	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	//frame rate
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	//format of display, 8 bits for rgba(each)
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//order in which things are rendered, as using double bufering this doesnt matter, so unspecified is fine
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//image is centered on the screen
	//bufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	//unspecified scaling
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc; 

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	//describes back buffer, use info filled out above
	swapChainDesc.BufferDesc = bufferDesc;
	//smoothes edges of lines (noice)
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	//input what we will render to, as this will be the useage
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//number of back buffers, 2 would be tripple buffering, 3 quadruple etc
	swapChainDesc.BufferCount = 1;
	//handle to window created
	swapChainDesc.OutputWindow = hwnd; 
	//fullscreen or windowed
	swapChainDesc.Windowed = TRUE; 
	//what to do with the front buffer after swapping to back buffer. discard lets display driver decide what most efficient thing is
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


	//Create SwapChain
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &Dev, NULL, &DevCon);

	//Create BackBuffer
	ID3D11Texture2D* BackBuffer;
	hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&BackBuffer );

	//Create Render Target
	hr = Dev->CreateRenderTargetView( BackBuffer, NULL, &renderTargetView );
	BackBuffer->Release();

	//Describe Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width     = Width;
	depthStencilDesc.Height    = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	//Create the Depth/Stencil View
	Dev->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	Dev->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	//Set our Render Target
	DevCon->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );

	return true;
}

void CleanUp()
{
	//Release the COM Objects created
	SwapChain->Release();
	Dev->Release();
	DevCon->Release();
	renderTargetView->Release();
	squareVertBuffer->Release();
	squareIndexBuffer->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	vertLayout->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
	cbPerObjectBuffer->Release();

}

bool InitScene()
{
	//Compile Shaders from shader file
	hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);

	//Create the Shader Objects
	hr = Dev->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = Dev->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);

	//Set Vertex and Pixel Shaders
	DevCon->VSSetShader(VS, 0, 0);
	DevCon->PSSetShader(PS, 0, 0);

	//Run scene script for all object
	//size of returns sie in bytes
	//by dividing by the size of one of them, it leaves the size of the array
	for (int i = 0; i < sizeof(Objs)/ sizeof(Objs[0]); i ++)
	{
		Objs[i].Scene();
	}

	//Set Primitive Topology
	DevCon->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	DevCon->RSSetViewports(1, &viewport);

	//Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;	
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;
	//creating cbuffer
	hr = Dev->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	//Camera information
	camPosition = XMVectorSet( 0.0f, 3.0f, -13.0f, 0.0f );
	camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	//Set the View matrix
	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );

	//Set the Projection matrix
	//FOV angle, Aspect ratio, Near cliping, Far cliping
	camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, Width/Height, 1.0f, 1000.0f);

	//describe sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//create sample state
	hr = Dev->CreateSamplerState(&sampDesc, &CubesTexSamplerState);

	return true;
}

//anything that changes in scene
void UpdateScene()
{

	TickCount = TickCountT;

	//Run update script on all Objects
	for (int i = 0; i < sizeof(Objs) / sizeof(Objs[0]); i ++)
	{
		Objs[i].Update();
	}

	//Update the colours of our scene
	red += colourmodr * 0.0005f;
	green += colourmodg * 0.0002f;
	blue += colourmodb * 0.0002f;

	//when it reaches edge conditions, flips colour movement
	if (red >= 1.0f || red <= 0.0f)
		colourmodr *= -1;
	if (green >= 1.0f || green <= 0.0f)
		colourmodg *= -1;
	if (blue >= 1.0f || blue <= 0.0f)
		colourmodb *= -1;
}

void DrawScene()
{
	//Clear our backbuffer to colour
	D3DXCOLOR bgcolour(red, green, blue, 1.0f);
	DevCon->ClearRenderTargetView(renderTargetView, bgcolour);

	//Refresh the Depth/Stencil view
	DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Run Draw script on all Objects
	for (int i = 0; i < sizeof(Objs) / sizeof(Objs[0]); i ++)
	{
		Objs[i].Draw();
	}

	//Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}

//keeps the programming running
int messageloop(){

	//rotate
	Objs[0].setRot(0, 1, 0, 1);
	Objs[1].setRot(0, 1, 0, 1);
	Objs[2].setRot(0, 1, 0, 1);

	Objs[3].setRot(1, 1, 0, 1);
	Objs[4].setRot(0, 1, 0, 1);
	Objs[5].setRot(1, 0, 0, 1);

	Objs[6].setRot(0.5, 1, 0.5, 1);
	Objs[7].setRot(0, 1, 0, 1);
	Objs[8].setRot(0.5, 1, 0, -1);

	//translate
	Objs[0].setTranslate(2, 2, 5);
	Objs[1].setTranslate(-2, -2, 9);
	Objs[2].setTranslate(0, 0, 7);

	Objs[3].setTranslate(0, 0, -5);
	Objs[4].setTranslate(0, 0, -5);
	Objs[5].setTranslate(0, 0, -5);

	Objs[6].setTranslate(0, 0, 0.1);
	Objs[7].setTranslate(0, 0.1, -0.1);
	Objs[8].setTranslate(0, -0.1, 0);

	//initial setting of tickcount
	TickCount = GetTickCount();
	TickCountT = GetTickCount();

	//structure of windows message
	MSG msg;
	//clears a structure
	ZeroMemory(&msg, sizeof(MSG));
	//whilst there is a message
	while(true)
	{
		//checks there is a message
		BOOL PeekMessageL(
			LPMSG lpMsg,
			HWND hWnd,
			UINT wMsgFilterMin,
			UINT wMsgFilterMax,
			UINT wRemoveMsg
			);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//if there is a quit message close the program
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		}
		//run game code
		else{    
			TickCountT = GetTickCount();
			if (TickCountT - TickCount > 10)
			{
				UpdateScene();
			}
			DrawScene();
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,
	UINT msg,
	//for detecting keyboard presses
	WPARAM wParam,
	LPARAM lParam)
{
	switch( msg )
	{
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE ){
			DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}