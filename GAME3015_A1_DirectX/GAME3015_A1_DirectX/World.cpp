#include "World.hpp"

World::World(Game* game)
	: mSceneGraph(new SceneNode(game))
	, mGame(game)
	, mPlayerAircraft(nullptr)
	, mBackground(nullptr)
	, mWorldBounds(-1.5f, 1.5f, 200.0f, 0.0f) //Left, Right, Down, Up
	, mSpawnPosition(0.f, 0.f)
	, mScrollSpeed(1.0f)
{
}

void World::update(const GameTimer& gt)
{
	mSceneGraph->update(gt);

	XMFLOAT3 position = mPlayerAircraft->getWorldPosition();
	XMFLOAT3 velocity = mPlayerAircraft->getVelocity();
	if (position.x < mWorldBounds.x || position.x > mWorldBounds.y)
	{
		velocity.x = -velocity.x;
		mPlayerAircraft->setVelocity(velocity);
	}
}

void World::draw()
{
	mSceneGraph->draw();
}

void World::loadTextures(
						 Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice, 
						 Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GameCommandList,
						 std::unordered_map<std::string, std::unique_ptr<Texture>>& GameTextures
						)
{
	//Eagle
	auto EagleTex = std::make_unique<Texture>();
	EagleTex->Name = "EagleTex";
	EagleTex->Filename = L"../../Textures/Eagle.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GameDevice.Get(),
		GameCommandList.Get(), EagleTex->Filename.c_str(),
		EagleTex->Resource, EagleTex->UploadHeap));

	GameTextures[EagleTex->Name] = std::move(EagleTex);

	//Raptor
	auto RaptorTex = std::make_unique<Texture>();
	RaptorTex->Name = "RaptorTex";
	RaptorTex->Filename = L"../../Textures/Raptor.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GameDevice.Get(),
		GameCommandList.Get(), RaptorTex->Filename.c_str(),
		RaptorTex->Resource, RaptorTex->UploadHeap));

	GameTextures[RaptorTex->Name] = std::move(RaptorTex);

	//Desert
	auto DesertTex = std::make_unique<Texture>();
	DesertTex->Name = "DesertTex";
	DesertTex->Filename = L"../../Textures/Desert.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GameDevice.Get(),
		GameCommandList.Get(), DesertTex->Filename.c_str(),
		DesertTex->Resource, DesertTex->UploadHeap));

	GameTextures[DesertTex->Name] = std::move(DesertTex);
}

void World::loadMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& GameMaterials)
{
	auto Eagle = std::make_unique<Material>();
	Eagle->Name = "Eagle";
	Eagle->MatCBIndex = 0;
	Eagle->DiffuseSrvHeapIndex = 0;
	Eagle->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Eagle->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Eagle->Roughness = 0.2f;

	GameMaterials["Eagle"] = std::move(Eagle);

	auto Raptor = std::make_unique<Material>();
	Raptor->Name = "Raptor";
	Raptor->MatCBIndex = 1;
	Raptor->DiffuseSrvHeapIndex = 1;
	Raptor->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Raptor->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Raptor->Roughness = 0.2f;

	GameMaterials["Raptor"] = std::move(Raptor);

	auto Desert = std::make_unique<Material>();
	Desert->Name = "Desert";
	Desert->MatCBIndex = 2;
	Desert->DiffuseSrvHeapIndex = 2;
	Desert->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Desert->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Desert->Roughness = 0.2f;

	GameMaterials["Desert"] = std::move(Desert);
}

void World::buildShapeGeometry(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice, 
							   Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GameCommandList, 
							   std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GameGeometries
							  )
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1, 0, 1, 1);
	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = 0;
	boxSubmesh.BaseVertexLocation = 0;


	std::vector<Vertex> vertices(box.Vertices.size());

	for (size_t i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].TexC = box.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices = box.GetIndices16();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(GameDevice.Get(),
		GameCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(GameDevice.Get(),
		GameCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;

	GameGeometries[geo->Name] = std::move(geo);
}

void World::buildScene()
{
	std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Eagle, mGame));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(0, 0.1, 0.0);
	mPlayerAircraft->setScale(0.5, 0.5, 0.5);
	mPlayerAircraft->setVelocity(mScrollSpeed, 0.f, 0.f);
	mSceneGraph->attachChild(std::move(player));

	std::unique_ptr<Aircraft> enemy1(new Aircraft(Aircraft::Raptor, mGame));
	auto raptor = enemy1.get();
	raptor->setPosition(0.5, 0, 1);
	raptor->setScale(1.0, 1.0, 1.0);
	raptor->setWorldRotation(0, XM_PI, 0);
	mPlayerAircraft->attachChild(std::move(enemy1));

	std::unique_ptr<Aircraft> enemy2(new Aircraft(Aircraft::Raptor, mGame));
	auto raptor2 = enemy2.get();
	raptor2->setPosition(-0.5, 0, 1);
	raptor2->setScale(1.0, 1.0, 1.0);
	raptor2->setWorldRotation(0, XM_PI, 0);
	mPlayerAircraft->attachChild(std::move(enemy2));

	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(mGame));
	mBackground = backgroundSprite.get();
	//mBackground->setPosition(mWorldBounds.left, mWorldBounds.top);
	mBackground->setPosition(0, 0, 0.0);
	mBackground->setScale(10.0, 1.0, 200.0);
	mBackground->setVelocity(0.f, 0.f,-mScrollSpeed);
	mSceneGraph->attachChild(std::move(backgroundSprite));

	mSceneGraph->build();
}

//void World::buildDescriptorHeaps(
//								 Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice, 
//								 std::unordered_map<std::string, std::unique_ptr<Texture>>& GameTextures,
//								 ComPtr<ID3D12DescriptorHeap> GameSrvDescriptorHeap,
//								 UINT GameCbvSrvDescriptorSize
//								)
//{
//	//
//	// Create the SRV heap.
//	//
//	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
//	srvHeapDesc.NumDescriptors = 3;
//	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//	ThrowIfFailed(GameDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&GameSrvDescriptorHeap)));
//
//	//
//	// Fill out the heap with actual descriptors.
//	//
//	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(GameSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
//
//	auto EagleTex = GameTextures["EagleTex"]->Resource;
//	auto RaptorTex = GameTextures["RaptorTex"]->Resource;
//	auto DesertTex = GameTextures["DesertTex"]->Resource;
//
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//
//	//This mapping enables the shader resource view (SRV) to choose how memory gets routed to the 4 return components in a shader after a memory fetch.
//	//When a texture is sampled in a shader, it will return a vector of the texture data at the specified texture coordinates.
//	//This field provides a way to reorder the vector components returned when sampling the texture.
//	//D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING  will not reorder the components and just return the data in the order it is stored in the texture resource.
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//
//	srvDesc.Format = EagleTex->GetDesc().Format;
//
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MostDetailedMip = 0;
//	//The number of mipmap levels to view, starting at MostDetailedMip.This field, along with MostDetailedMip allows us to
//	//specify a subrange of mipmap levels to view.You can specify - 1 to indicate to view
//	//all mipmap levels from MostDetailedMip down to the last mipmap level.
//
//	srvDesc.Texture2D.MipLevels = EagleTex->GetDesc().MipLevels;
//
//	//Specifies the minimum mipmap level that can be accessed. 0.0 means all the mipmap levels can be accessed.
//	//Specifying 3.0 means mipmap levels 3.0 to MipCount - 1 can be accessed.
//	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
//
//	GameDevice->CreateShaderResourceView(EagleTex.Get(), &srvDesc, hDescriptor);
//
//	//Raptor Descriptor
//	hDescriptor.Offset(1, GameCbvSrvDescriptorSize);
//	srvDesc.Format = RaptorTex->GetDesc().Format;
//	GameDevice->CreateShaderResourceView(RaptorTex.Get(), &srvDesc, hDescriptor);
//
//	//Desert Descriptor
//	hDescriptor.Offset(1, GameCbvSrvDescriptorSize);
//	srvDesc.Format = DesertTex->GetDesc().Format;
//	GameDevice->CreateShaderResourceView(DesertTex.Get(), &srvDesc, hDescriptor);
//}
