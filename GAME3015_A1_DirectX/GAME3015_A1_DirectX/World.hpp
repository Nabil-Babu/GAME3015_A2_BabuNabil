#pragma once
#include "SceneNode.hpp"
#include "Aircraft.hpp"
#include "SpriteNode.h"



class World 
{
public:
	explicit World(Game* window);
	void update(const GameTimer& gt);
	void draw();

	void loadTextures(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice, 
					  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GameCommandList,
					  std::unordered_map<std::string, std::unique_ptr<Texture>>& GameTextures);
	
	void loadMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& GameMaterials);
	void buildShapeGeometry(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice,
							Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GameCommandList,
							std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GameGeometries);
	
	/*void buildDescriptorHeaps(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice, 
							  std::unordered_map<std::string, std::unique_ptr<Texture>>& GameTextures,
							  ComPtr<ID3D12DescriptorHeap> GameSrvDescriptorHeap,
							  UINT GameCbvSrvDescriptorSize
							 );*/
	
	void buildScene();

	Camera* GetCamera() { return &_Camera; }


private:
	enum Layer
	{
		Background,
		Air,
		LayerCount
	};


private:
	// Reference to the Game Backend
	Game* mGame;
	// World Holds the Camera
	Camera _Camera;

	SceneNode* mSceneGraph;
	std::array<SceneNode*, LayerCount> mSceneLayers;

	XMFLOAT4 mWorldBounds;
	XMFLOAT2 mSpawnPosition;
	float mScrollSpeed;
	Aircraft* mPlayerAircraft;
	SpriteNode* mBackground;
	Aircraft* mEnemy;
};
