#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "Particle.h"

struct ElementBase
{
	ElementBase() = default;
	ElementBase(
		int id, float spread,
		bool explosive, bool burn, 
		bool ignite,
		bool melt = false, bool freeze = false,
		bool boil = false, bool condense = false
	)
		: ID(id), SpreadVal(spread)
		, IsMeltable(melt), IsFreezalbe(freeze)
		, IsBoilable(boil), IsCondensable(condense)
		, IsExplosive(explosive), IsBurnable(burn), CanIgnite(ignite)
	{}
	
	// vitural interact function 
	virtual void Interact(std::set<Particle*>::iterator& itr) = 0;

	// state changes:
	// solid MELT liquid BOIL gas
	// gas CONDENSATE liquid FREEZE solid

	// Solids: melt 
	// Liquids: freeze, boil
	// Gases: condensate

	// matter state interacitons
	bool IsMeltable;		// down state
	bool IsFreezalbe;		// up state
	bool IsBoilable;		// down state
	bool IsCondensable;	// up state

	// extra interaciton possibilities
	bool IsExplosive;
	bool IsBurnable;
	bool CanIgnite;

	// element ID: stores state of matter and general id of element
	// ID: AB - A is state, B is id
	int ID;

	// probability value that the element should try to interact with it's neighbors
	float SpreadVal;
};

struct Air : public ElementBase
{
	Air(
		int id, float spread,
		bool explosive, bool burn,
		bool ignite,
		bool melt = false, bool freeze = false,
		bool boil = false, bool condense = false
	)
		: ElementBase(
			id, spread,
			explosive, burn, ignite,
			melt, freeze, boil, condense
		)
	{}

	void Interact(std::set<Particle*>::iterator& itr);
};

struct Stone : public ElementBase
{
	Stone(
		int id, float spread,
		bool explosive, bool burn,
		bool ignite,
		bool melt = false, bool freeze = false,
		bool boil = false, bool condense = false
	)
		: ElementBase(
			id, spread,
			explosive, burn, ignite,
			melt, freeze, boil, condense
		)
	{}

	void Interact(std::set<Particle*>::iterator& itr);
};

struct Water : public ElementBase
{
	Water(
		int id, float spread,
		bool explosive, bool burn,
		bool ignite,
		bool melt = false, bool freeze = false,
		bool boil = false, bool condense = false
	)
		: ElementBase(
			id, spread,
			explosive, burn, ignite,
			melt, freeze, boil, condense
		)
	{}

	void Interact(std::set<Particle*>::iterator& itr);
};



#endif