#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include "graphics/Graphics.h"

#include <set>
#include <unordered_map>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> // input reg

struct Particle
{
	Particle() = default;
	Particle(int X, int Y, int Type = 0) : x(X), y(Y), id(Type) {}
	Particle(Particle* pcopy) : x(pcopy->x), y(pcopy->y), id(pcopy->id) {}

	int x = -1;
	int y = -1;
	int id = 0;
};

struct ParticleType
{
	ParticleType() = default;
	ParticleType(
		int id, float spread, float r = -1.f, float g = -1.f, float b = -1.f, 
		bool melt = false, bool freeze = false, bool boil = false, bool condense = false, bool burn = false, bool explode = false,
		bool upstate = false, bool downstate = false
	)
		: ID(id), spreadValue(spread)
		, red(r), green(g), blue(b)
		, IsMeltable(melt), IsFreezalbe(freeze), IsBoilable(boil), IsCondensable(condense)
		, IsBurnable(burn), IsExplosive(explode)
		, CanUpState(upstate), CanDownState(downstate)
	{
		if (red < 0.f)
			isVisible = false;
		else
			isVisible = true;
	}

	// probability to spread to a valid particle
	float spreadValue = 0.f;

	// ID which holds state of matter and general id info
	// "xy" : x = matter state, y = id
	int ID = -1;

	// color and drawing information
	bool isVisible = false;
	float red = 0.f;
	float green = 0.f;
	float blue = 0.f;

	/// core interaction info
	// matter state interacitons
	bool IsMeltable;		// down state
	bool IsFreezalbe;		// up state
	bool IsBoilable;		// down state
	bool IsCondensable;	// up state

	// other state interactions
	bool IsBurnable;		// down state
	bool IsExplosive;		// down state
	
	/// up/down state ability
	//! up state: whether this particle can up shift another particle's state
	//! down state: whether this particle can down shift another particle's state
	// - generally, up state is heating, down state is cooling
	// example: melting is a down state, freezing is a up state
	bool CanUpState;		// up state
	bool CanDownState;	// down state

	//todo potentially add a special upstate/downstate int value to be added by the Interact function
};

class ParticleManager
{
public:
	ParticleManager() = default;
	ParticleManager(int x, int y, int worldWidth, int worldHeight);

	void Draw(unsigned int scale = 1);
	void Update(float dt, int scale = 1);

	bool TryMoveParticleVertical(std::set<Particle*>::iterator& itr, int& ymove);
	bool TryMoveParticleHorizontal(std::set<Particle*>::iterator& itr, int& xmove);
	bool TrySwapParticle(std::set<Particle*>::iterator& itr, int x, int y);

	void WakeParticle(int x, int y);
	bool ShouldParticleSleep(std::set<Particle*>::iterator& itr);

	void InteractWithNeighbors(std::set<Particle*>::iterator& itr);
	void Interact(int toX, int toY, int fromX, int fromY, ParticleType fromParticle);

	void SpawnParticle(int x, int y, int id);

public:
	bool IsValidCoord(int worldX, int worldY);
	bool IsValidCoordScreen(int screenX, int screenY);

	// Returns whether the x,y world coord is empty. If x,y world coord is invalid, returns false
	bool IsEmpty(int x, int y);
	bool IsAwake(int x, int y);

	bool IsSurrounded(int x, int y, int id);

	int GetParticleType(int x, int y);

	void ToWorldCoord(int& screenX, int& screenY);
	void ToScreenCoord(int& worldX, int& worldY);

	void AddParticleToPool(std::set<Particle*>& set, Particle* p);

	bool isMouseDown = false;
	bool isLiquid = false;
	bool debugDraw = false;
	bool paused = false;

private:
	int m_Width = 0;
	int m_Height = 0;
	int m_X = 0;
	int m_Y = 0;
	unsigned int m_NumParticles = 0;


	std::set<Particle*> m_AwakePool;
	std::vector<std::vector<Particle>> m_WorldHash;
	std::unordered_map<int, ParticleType> m_ParticleLookup;
};

#endif