#include "Aircraft.hpp"
#include "Game.hpp"

Aircraft::Aircraft(Type type, Game* game) : Entity(game)
	, mType(type)
{
	switch (type)
	{
	case (Eagle):
		mSprite = "Eagle";
		break;
	case (Raptor):
		mSprite = "Raptor";
		break;
	default:
		mSprite = "Eagle";
		break;
	}
}

unsigned int Aircraft::getCategory() const
{
	switch (mType)
	{
	case Eagle:
		return Category::PlayerAircraft;

	default:
		return Category::EnemyAircraft;
	}
}

void Aircraft::drawCurrent() const
{	
}

void Aircraft::updateCurrent()
{
	
}

void Aircraft::buildCurrent()
{
	auto render = std::make_unique<RenderItem>();
	renderer = render.get();
	renderer->World = getTransform();
	renderer->ObjCBIndex = game->getRenderItems().size();
	renderer->Mat = game->getMaterials()[mSprite].get();
	renderer->Geo = game->getGeometries()["boxGeo"].get();
	renderer->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderer->IndexCount = renderer->Geo->DrawArgs["box"].IndexCount;
	renderer->StartIndexLocation = renderer->Geo->DrawArgs["box"].StartIndexLocation;
	renderer->BaseVertexLocation = renderer->Geo->DrawArgs["box"].BaseVertexLocation;

	game->getRenderItems().push_back(std::move(render));
}
