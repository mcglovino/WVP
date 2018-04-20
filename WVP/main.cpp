//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <String>
#include <cstdlib>

//for tinyobj loader
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "_Input.h"
#include "_FrameRate.h"
#include "_Vertex.h"

//texture and model paths
const LPCWSTR TEA_TEXTURE_PATH = L"textures/TEA.jpg";
const LPCWSTR EVIL_TEXTURE_PATH = L"textures/EVIL.jpg";
const LPCWSTR SPOTS_TEXTURE_PATH = L"textures/Spots.jpg";
const LPCWSTR DIRT_TEXTURE_PATH = L"textures/dirt.jpg";

const LPCWSTR c0_TEXTURE_PATH = L"textures/0.jpg";
const LPCWSTR c1_TEXTURE_PATH = L"textures/1.jpg";
const LPCWSTR c2_TEXTURE_PATH = L"textures/2.jpg";
const LPCWSTR c3_TEXTURE_PATH = L"textures/3.jpg";
const LPCWSTR c4_TEXTURE_PATH = L"textures/4.jpg";
const LPCWSTR c5_TEXTURE_PATH = L"textures/5.jpg";
const LPCWSTR c6_TEXTURE_PATH = L"textures/6.jpg";
const LPCWSTR c7_TEXTURE_PATH = L"textures/7.jpg";
const LPCWSTR c8_TEXTURE_PATH = L"textures/8.jpg";
const LPCWSTR c9_TEXTURE_PATH = L"textures/9.jpg";

const LPCWSTR WORM_TEXTURE_PATH = L"textures/Worm.jpg";

const std::string SPHERE_MODEL_PATH = "models/sphere16.obj";
const std::string CUBE_MODEL_PATH = "models/Cube.obj";
const std::string TEAPOT_MODEL_PATH = "models/teapot.obj";
const std::string PLAYER_MODEL_PATH = "models/player.obj";
const std::string FLOOR_MODEL_PATH = "models/floorbig.obj";
const std::string CHAR_MODEL_PATH = "models/floorsmall.obj";


//Global Declarations - Interfaces//
IDXGISwapChain* SwapChain; // pointer to swap back and front buffers, preventing scan lines where its bieng drawn (rendering is from back buffer)
ID3D11Device* Dev; // pointer to direct3D device interface
ID3D11DeviceContext* DevCon; // pointer to direct3D device context
ID3D11RenderTargetView* renderTargetView; //back buffer, a 2d texture, this is written to and the sent to be rendered on screen
//ID3D11Buffer* squareIndexBuffer;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;
//ID3D11Buffer* squareVertBuffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;
//ID3D11InputLayout* vertLayout;
ID3D11Buffer* cbPerObjectBuffer;

//Input
_Input* Input;
int prev = 1;

//for frame rate stabilisation
static int TickCount;
static int TickCountT;

_FrameRate framerate;

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
XMMATRIX camView;
XMMATRIX camProjection;

XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;

//for translating, rotating and scaling world objects
XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;

//Function Prototypes
//initialize direct3D
bool InitializeDirect3d11App(HINSTANCE hInstance);
void CleanUp();
bool InitScene();
void UpdateScene();
void BackgroundColour();
void DrawScene();
void Setup();

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

//Create effects constant buffer's structure
struct cbPerObject
{
	XMMATRIX  WVP;
};

cbPerObject cbPerObj;

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
	//constructor
	_Object(LPCWSTR texT, std::string modT) {
		texture = texT;
		MODEL_PATH = modT;
		setRot(0,10,0,0);
		setTranslate(0, 0, 0);
	}
	_Object() {
		texture = SPOTS_TEXTURE_PATH;
		MODEL_PATH = SPHERE_MODEL_PATH;
		setTranslate(0, 0, 0);
		setRot(0, 10, 0, 0);
	}

	void Init()
	{
		//loadModel();

		_Vertex* V = &vertices[0];
		DWORD* I = &indices[0];

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * indices.size();
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA iinitData;

		iinitData.pSysMem = I;
		//iinitData.pSysMem = indices;

		Dev->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

		DevCon->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(_Vertex) * vertices.size();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = V;
		hr = Dev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

		//Set the vertex buffer
		UINT stride = sizeof(_Vertex);
		UINT offset = 0;
		DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &stride, &offset);

		//Create the Input Layout
		hr = Dev->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
			VS_Buffer->GetBufferSize(), &vertLayout);

		//Set the Input Layout
		DevCon->IASetInputLayout(vertLayout);



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
		hr = Dev->CreateSamplerState(&sampDesc, &TexSamplerState);
	}

	void Update()
	{
		if(rotate == 1)
			rot -= rotSpeed;

		XMVECTOR rotaxis = XMVectorSet(rotX, rotY, rotZ, 0.0f);

		//Reset obj
		obj = XMMatrixIdentity();

		//Define obj's world space matrix
		Rotation = XMMatrixRotationAxis(rotaxis, rot);
		Translation = XMMatrixTranslation(transX, transY, transZ);

		//Set obj's world space using the transformations
		obj = Rotation * Translation;
	}

	void Draw()
	{
		//Reset the vertex buffer
		UINT stride = sizeof(_Vertex);
		UINT offset = 0;
		DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &stride, &offset);

		//Set the WVP matrix and send it to the constant buffer in effect file
		WVP = obj * camView * camProjection;
		cbPerObj.WVP = XMMatrixTranspose(WVP);
		DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

		//for textures
		DevCon->PSSetShaderResources(0, 1, &Texture);
		DevCon->PSSetSamplers(0, 1, &TexSamplerState);

		//Draw the obj
		DevCon->DrawIndexed(indices.size() * 3, 0, 0);
	}


	void loadModel() {
		//texture load
		hr = D3DX11CreateShaderResourceViewFromFile(Dev, texture,
			NULL, NULL, &Texture, NULL);

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		vertices.clear();
		indices.clear();

		//load object
		//if it cant, throw error
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, MODEL_PATH.c_str())) {
			throw std::runtime_error(err);
		}


		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				_Vertex vertex = {};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertices.push_back(vertex);


				indices.push_back(indices.size());

			}
		}
	}

	void respawn(_Object &other, _Object &sHead, bool timed) {
		if (timed == 0) {
			setTranslate((rand() % 45 - 22), 0.0f, (rand() % 45 - 22));
			//so it doesnt spawn right next to the player or other teapot
			while ((transX > sHead.getX() - 7.0f && transX < sHead.getX() + 7.0f && transZ > sHead.getZ() - 7.0f && transZ < sHead.getZ() + 7.0f) 
				|| (transX > other.getX() - 2.0f && transX < other.getX() + 2.0f && transZ > other.getZ() - 2.0f && transZ < other.getZ() + 2.0f)){
				setTranslate((rand() % 45 - 22), 0.0f, (rand() % 45 - 22));
			}
		}
		else {
			tickCountT = GetTickCount();
			diffAccumulator += tickCountT - tickCount;
			if (tickCountT - tickCount > 4000)
			{
				setTranslate((rand() % 45 - 22), 0.0f, (rand() % 45 - 22));
				//so it doesnt spawn right next to the player or other teapot
				while ((transX > sHead.getX() - 7.0f && transX < sHead.getX() + 7.0f && transZ > sHead.getZ() - 7.0f && transZ < sHead.getZ() + 7.0f)
					|| (transX > other.getX() - 2.0f && transX < other.getX() + 2.0f && transZ > other.getZ() - 2.0f && transZ < other.getZ() + 2.0f)) {
					setTranslate((rand() % 45 - 22), 0.0f, (rand() % 45 - 22));
				}

				tickCount = GetTickCount();
			}
		}
	}

	void setTexture(LPCWSTR textureT) {

		texture = textureT;
		//texture load
		hr = D3DX11CreateShaderResourceViewFromFile(Dev, texture,
			NULL, NULL, &Texture, NULL);
	}

	void AssignChar(char Char) {
		switch (Char) {
		case '0':
			setTexture(c0_TEXTURE_PATH);
			break;
		case '1':
			setTexture(c1_TEXTURE_PATH);
			break;
		case '2':
			setTexture(c2_TEXTURE_PATH);
			break;
		case '3':
			setTexture(c3_TEXTURE_PATH);
			break;
		case '4':
			setTexture(c4_TEXTURE_PATH);
			break;
		case '5':
			setTexture(c5_TEXTURE_PATH);
			break;
		case '6':
			setTexture(c6_TEXTURE_PATH);
			break;
		case '7':
			setTexture(c7_TEXTURE_PATH);
			break;
		case '8':
			setTexture(c8_TEXTURE_PATH);
			break;
		case '9':
			setTexture(c9_TEXTURE_PATH);
			break;
		}
	}

	void setSpin(float speedT, bool rotateT)
	{
		rotSpeed = speedT;
		rotate = rotateT;

	}

	void setRot(float X, float Y, float Z, float rotT)
	{
		rotX = X;
		rotY = Y;
		rotZ = Z;
		rot = rotT;
	}

	void setTranslate(float X, float Y, float Z)
	{
		transX = X;
		transY = Y;
		transZ = Z;
	}

	float getX() {
		return transX;
	}
	float getY() {
		return transY;
	}
	float getZ() {
		return transZ;
	}

	private:

	XMMATRIX WVP;
	XMMATRIX obj;
	float rot = 0;
	bool rotate = 0;
	float rotSpeed = 0;
	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;

	float rotX  = 0;
	float rotY = 0;
	float rotZ = 0;

	float transX = 0;
	float transY = 0;
	float transZ = 0;

	int tickCount = GetTickCount();
	int tickCountT = GetTickCount();
	int diffAccumulator = 0;

	ID3D11ShaderResourceView* Texture;
	ID3D11SamplerState* TexSamplerState;

	LPCWSTR texture;
	std::string MODEL_PATH;

	std::vector<_Vertex> vertices;
	std::vector<DWORD> indices;
	ID3D11Buffer* vertexBuffer;
	ID3D11Device* vertexBufferMemory;

	ID3D11Buffer* squareIndexBuffer;
	ID3D11Buffer* squareVertBuffer;
	ID3D11InputLayout* vertLayout;
};

class _BodyPart : public _Object{
public:
	using _Object::_Object;
	
	void PreviousLocation(float score) {
		tickCountT2 = GetTickCount();
		diffAccumulator2 += tickCountT2 - tickCount2;
		if (tickCountT2 - tickCount2 > 30 - score / 2)
		{
			tickCountT2 = GetTickCount();
			tickCount2 = GetTickCount();
			diffAccumulator2 = 0;
			transXp = getX();
			transYp = getY();
			transZp = getZ();
		}
	}

	float getXp() {
		return transXp;
	}
	float getYp() {
		return transYp;
	}
	float getZp() {
		return transZp;
	}

private:
	float transXp = 0;
	float transYp = 0;
	float transZp = 0;

	int tickCount2 = GetTickCount();
	int tickCountT2 = GetTickCount();
	int diffAccumulator2 = 0;
};


class _Worm{
public:
	std::vector<_BodyPart> BodyParts{};
	//constructor
	_Worm(int lengthT)
	{
		_BodyPart newPart(SPOTS_TEXTURE_PATH, PLAYER_MODEL_PATH);
		BodyParts.push_back(newPart);
		for (int i = 1; i < totalSize -1; i++) {
			_BodyPart newPart(SPOTS_TEXTURE_PATH, SPHERE_MODEL_PATH);
			BodyParts.push_back(newPart);
		}
		BodyParts.push_back(newPart);

		setLength(lengthT);

		score = 0;
	}

	void addPos(float X, float Y, float Z) {
		BodyParts[0].setTranslate(BodyParts[0].getX() + X, BodyParts[0].getY() + Y, BodyParts[0].getZ() + Z);
	}

	//getters
	int getLength()
	{
		return length;
	}
	float getScore()
	{
		return score;
	}
	int getTotalSize()
	{
		return totalSize;
	}

	//setters
	void setLength(int lengthT)
	{
		length = lengthT;
		if (length > lengthT) {
			if (lengthT > 0) {
				length = lengthT;
			}
			else
				length = 1;
		}
	}

	void Update(_Object &Tea, _Object &Evil) {
		//move
		for (int i = 1; i < length; i++) {
			BodyParts[i].setTranslate(BodyParts[i - 1].getXp(), BodyParts[i - 1].getYp(), BodyParts[i - 1].getZp());
		}
		for (int i = length; i < totalSize; i++) {
			BodyParts[i].setTranslate(0, 100, 0);
		}

		//evil teapot teapot
		if (BodyParts[0].getX() > Evil.getX() - 1.5f && BodyParts[0].getX() < Evil.getX() + 1.5f && BodyParts[0].getZ() > Evil.getZ() - 1.5f && BodyParts[0].getZ() < Evil.getZ() + 1.5f) {
			//end game
			CleanUp();
			std::exit(1);
		}
		//collect teapot
		if (BodyParts[0].getX() > Tea.getX() - 1.5f && BodyParts[0].getX() < Tea.getX() + 1.5f && BodyParts[0].getZ() > Tea.getZ() - 1.5f && BodyParts[0].getZ() < Tea.getZ() + 1.5f) {
			score++;
			length++;
			Tea.respawn(Evil, BodyParts[0], 0);
		}

		if (BodyParts[0].getX() > 25 || BodyParts[0].getX() < -25 || BodyParts[0].getZ() > 25 || BodyParts[0].getZ() < -25 //If outside the area
			|| length == 100) //as more than 100 are not avaliable, and the score caps at 99
		{
			//end game
			CleanUp();
			std::exit(1);
		}
	}

	void InputCont() {
		if (Input != NULL && Input->IsKeyDown(DIK_W))
		{
			addPos(0, 0,( 0.2f + (score/200)));
			prev = 1;
		}
		if (Input != NULL && Input->IsKeyDown(DIK_S))
		{
			addPos(0, 0, -(0.2f + (score / 200)));
			prev = 2;
		}
		if (Input != NULL && Input->IsKeyDown(DIK_D))
		{
			addPos((0.2f + (score / 200)), 0, 0);
			prev = 3;
		}
		if (Input != NULL && Input->IsKeyDown(DIK_A))
		{
			addPos(-(0.2f + (score / 200)), 0, 0);
			prev = 4;
		}
		if (!Input->IsKeyDown(DIK_A) && !Input->IsKeyDown(DIK_D) && !Input->IsKeyDown(DIK_S) && !Input->IsKeyDown(DIK_W))
		{
			switch (prev) {
			case 1:
				addPos(0, 0, (0.2f + (score / 200)));
				break;
			case 2:
				addPos(0, 0, -(0.2f + (score / 200)));
				break;
			case 3:
				addPos((0.2f + (score / 200)), 0, 0);
				break;
			case 4:
				addPos(-(0.2f + (score / 200)), 0, 0);
				break;
			}
		}
	}

private:
	int length;
	int totalSize = 100;
	float score;
};

//array of Objects
_Object Objs[6] = { { DIRT_TEXTURE_PATH,FLOOR_MODEL_PATH }, //floor
				{ TEA_TEXTURE_PATH,TEAPOT_MODEL_PATH },//teapot
				{ EVIL_TEXTURE_PATH, TEAPOT_MODEL_PATH },//evil teapot
				{ c0_TEXTURE_PATH, CHAR_MODEL_PATH },//score
				{ c0_TEXTURE_PATH, CHAR_MODEL_PATH },
				{ WORM_TEXTURE_PATH, FLOOR_MODEL_PATH } };//rules
				

_Worm worm(1);

//Main windows function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
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

	Input = new _Input();
	Input->Initialize(hInstance, hwnd, Width, Height);

	messageloop();
	CleanUp();

	return 0;
}

//create and register windowbclass
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed)
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
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

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
		L"Worm",
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
	//squareVertBuffer->Release();
	//squareIndexBuffer->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	//vertLayout->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
	cbPerObjectBuffer->Release();

}

void Setup() {
	//rotate
	Objs[1].setSpin(0.02f, 1);
	Objs[2].setSpin(-0.02f, 1);
	Objs[3].setRot(0, 1, 0, 3.1415927f);
	Objs[4].setRot(0, 1, 0, 3.1415927f);
	Objs[5].setRot(0, 0.7071068f, -0.7071068f, 3.1415927f);


	//translate
	Objs[1].setTranslate(-5, 0, -10);
	Objs[2].setTranslate(5, 0, -10);
	Objs[3].setTranslate(-20, 0, 35);
	Objs[4].setTranslate(-11, 0, 35);
	Objs[5].setTranslate(23, -13, 80);
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


	Setup();
	//Run scene script for all object
	//size of returns sie in bytes
	//by dividing by the size of one of them, it leaves the size of the array
	for (int i = 0; i < sizeof(Objs)/ sizeof(Objs[0]); i ++)
	{
		Objs[i].loadModel();
		Objs[i].Init();
	}

	for (int i = 0; i < worm.getTotalSize(); i++)
	{
		worm.BodyParts[i].loadModel();
		worm.BodyParts[i].Init();
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
	camPosition = XMVectorSet( 0.0f, 40.0f, -35.0f, 0.0f );
	camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	//Set the View matrix
	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );

	//Set the Projection matrix
	//FOV angle, Aspect ratio, Near cliping, Far cliping
	camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, Width/Height, 1.0f, 1000.0f);


	return true;
}

//anything that changes in scene
void UpdateScene()
{

	TickCount = TickCountT;
	Input->Update();

	worm.InputCont();
	worm.Update(Objs[1], Objs[2]);
	Objs[2].respawn(Objs[1], worm.BodyParts[0], 1);

	//std::string framerateSTR = std::to_string(framerate.CalculateFrameRate());
	//const char *framerateCHAR = framerateSTR.c_str();
	std::string scoreSTR = std::to_string(worm.getScore());
	const char *scoreCHAR = scoreSTR.c_str();
	if (worm.getScore() > 10) {
		Objs[3].AssignChar(scoreCHAR[0]);
		Objs[4].AssignChar(scoreCHAR[1]);
	}
	else
	{
		Objs[4].AssignChar(scoreCHAR[0]);
	}
	
	//Run update script on all Objects
	for (int i = 0; i < sizeof(Objs) / sizeof(Objs[0]); i ++)
	{
		Objs[i].Update();
	}

	for (int i = 0; i < worm.getTotalSize(); i++)
	{
		worm.BodyParts[i].PreviousLocation(worm.getScore());
		worm.BodyParts[i].Update();
	}

	BackgroundColour();
}

void BackgroundColour() {
	//Update the colours of the background
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

	for (int i = 0; i < worm.getTotalSize(); i++)
	{
		worm.BodyParts[i].Draw();
	}

	//Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}

//keeps the programming running
int messageloop(){

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
		//stablises frame rate
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