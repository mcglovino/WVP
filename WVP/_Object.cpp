#include "_Object.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <String>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "_Vertex.h"
#include "_cbPerObject.h"

//Global Declarations - Interfaces//
//IDXGISwapChain* SwapChain; // pointer to swap back and front buffers, preventing scan lines where its bieng drawn (rendering is from back buffer)
//ID3D11Device* Dev; // pointer to direct3D device interface
//ID3D11DeviceContext* DevCon; // pointer to direct3D device context
//ID3D11RenderTargetView* renderTargetView; //back buffer, a 2d texture, this is written to and the sent to be rendered on screen
//ID3D11Buffer* squareIndexBuffer;
//ID3D11DepthStencilView* depthStencilView;
//ID3D11Texture2D* depthStencilBuffer;
////ID3D11Buffer* squareVertBuffer;
//ID3D11VertexShader* VS;
//ID3D11PixelShader* PS;
//ID3D10Blob* VS_Buffer;
//ID3D10Blob* PS_Buffer;
////ID3D11InputLayout* vertLayout;
ID3D11Buffer* cbPerObjectBuffer;

HRESULT hr;

//world view declarations
XMMATRIX camView;
XMMATRIX camProjection;

XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;


_cbPerObject cbPerObj;

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT numElements = ARRAYSIZE(layout);


//class
//class _Object
//{
//public:

	XMMATRIX WVP;
	XMMATRIX obj;
	float rot = 0.01f;
	float rotspeed = 0.05f;
	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;

	float rotX = 0;
	float rotY = 0;
	float rotZ = 0;

	float transX = 0;
	float transY = 0;
	float transZ = 0;

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

	/*//constructor
	_Object(LPCWSTR texT, std::string modT) {
		texture = texT;
		MODEL_PATH = modT;
	}*/

	//Constructor
	_Object::_Object(LPCWSTR texT, std::string modT)
	{
		texture = texT;
		MODEL_PATH = modT;
	}


	/*_Object::~_Object()
	{
	}*/

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

		/*//Create the Input Layout
		hr = Dev->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
			VS_Buffer->GetBufferSize(), &vertLayout);*/

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
		rot -= rotspeed;

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


	void setRot(float X, float Y, float Z, float rotspeedT)
	{
		rotX = X;
		rotY = Y;
		rotZ = Z;
		rotspeed = rotspeedT;
	}

	void setTranslate(float X, float Y, float Z)
	{
		transX = X;
		transY = Y;
		transZ = Z;
	}
//};
