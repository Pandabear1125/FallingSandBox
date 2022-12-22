#include "Particle.h"

ParticleManager::ParticleManager(int x, int y, int worldWidth, int worldHeight)
	: m_X(x), m_Y(y), m_Width(worldWidth), m_Height(worldHeight)
{
	m_WorldHash.resize(worldWidth);
	for (int i = 0; i < worldWidth; i++)
	{
		m_WorldHash[i].resize(worldHeight);
		for (int j = 0; j < worldHeight; j++)
		{
			m_WorldHash[i][j] = Particle(i + x, j + x);
		}
	}

	// initialize lookup table
	/*! args:										ID, spread,  r,		g,		b,		melt,		freeze,	boil,		condense,	burn,		explode,  upstate,	downstate	*/
	ParticleType gas = ParticleType(		00, 0.f,		-1,	-1,	-1,	false,	false,	false,	true,		false,	false,	false,	false);
	m_ParticleLookup[gas.ID] = gas;

	ParticleType liquid = ParticleType(	10, 0.f,		0,		0,		1,		false,	true,		true,		false,	false,	false,	false,	false);
	m_ParticleLookup[liquid.ID] = liquid;

	ParticleType solid = ParticleType(	20, 0.1f,	1,		1,		1,		true,		false,	false,	false,	false,	false,	false,	false);
	m_ParticleLookup[solid.ID] = solid;
}

void ParticleManager::Draw(unsigned int scale)
{
	Graphics::BatchPointsPush();

	ParticleType p;
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			if (debugDraw)
			{
				if (IsAwake(i, j))
					if (GetParticleType(i, j) != 0)
						Graphics::Point(m_WorldHash[i][j].x, m_WorldHash[i][j].y, scale, 1, 0, 0);
					else 
						Graphics::Point(m_WorldHash[i][j].x, m_WorldHash[i][j].y, scale, 0, 0, 1);
				else
					if (GetParticleType(i, j) != 0)
						Graphics::Point(m_WorldHash[i][j].x, m_WorldHash[i][j].y, scale, 1, 1, 1);
			}
			else 
			{
				p = m_ParticleLookup[m_WorldHash[i][j].id];
				if (p.isVisible)
				{
					Graphics::Point(m_WorldHash[i][j].x, m_WorldHash[i][j].y, scale, p.red, p.green, p.blue);
				}
			}

		}
	}


	Graphics::BatchPointsPop(scale);

	Graphics::Print("Particle Count / Awake Count: " + std::to_string(m_NumParticles) + " / " + std::to_string(m_AwakePool.size()), 0, 0, 0.1f);
	Graphics::Print("Debug Draw: ", 0, 2.5, 0.1f);
	Graphics::Print((bool)debugDraw, 16.5, 2.5, 0.1f);
	Graphics::Print("Place Mode: ", 0, 5, 0.1f);
	if (isLiquid)
		Graphics::Print("liquid", 16.5, 5, 0.1f);
	else
		Graphics::Print("solid", 16.5, 5, 0.1f);

	Graphics::Rectangle(Graphics::LINE, m_X, m_Y, m_Width, m_Height);

}

void ParticleManager::Update(float dt, int scale)
{
	// check if particles should spawn from mouse
	if (isMouseDown)
	{
		double x, y;
		glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);

		int wx((int)x / scale), wy((int)y / scale);
		ToWorldCoord(wx, wy);

		int placeID;
		if (isLiquid)
			placeID = 10;
		else
			placeID = 20;

		SpawnParticle(wx+1, wy, placeID);
		SpawnParticle(wx, wy, placeID);
		SpawnParticle(wx-1, wy, placeID);
	}

	// handle updating active particles
	int xmove, ymove;
	auto itEnd = m_AwakePool.end();
	for (auto it = m_AwakePool.begin(); it != itEnd;)
	{
		int wx((*it)->x), wy((*it)->y);
		ToWorldCoord(wx, wy);

		if ((*it)->id == 0)
			std::cout << "Invalid Particle In Awake Pool: " << wx << ' ' << wy << std::endl;

		InteractWithNeighbors(it);

		if (TryMoveParticleVertical(it, ymove))
		{
			continue;
		}
		else if (TryMoveParticleHorizontal(it, xmove))
		{
			continue;
		}
		else if (ShouldParticleSleep(it))
		{
			it = m_AwakePool.erase(it);
		}
		else
			it++;
	}

}

bool ParticleManager::TryMoveParticleVertical(std::set<Particle*>::iterator& itr, int& ymove)
{
	int wx((*itr)->x), wy((*itr)->y);
	ToWorldCoord(wx, wy);

	switch ((*itr)->id/10)
	{
	case 0:
		std::cout << "Invalid Particle Type" << std::endl;
		break;
	case 1:
		ymove = 1;
		break;
	case 2:
		ymove = 1;
		// cause cascade if solids are stacked asleep and the one below is awake
		WakeParticle(wx, wy - 1); 
		break;
	default:
		ymove = 0;
		break;
	}

	if (TrySwapParticle(itr, wx, wy + ymove))
	{
		if (itr != m_AwakePool.end())
			itr++;

		return true;
	}
	
	if (IsAwake(wx, wy + ymove))
	{
		int xmove = 0;
		if (TryMoveParticleHorizontal(itr, xmove))
		{
			return true;
		}
		else 
		{
			if ((*itr)->id / 10 == 1)
				return false;
			else
				itr++;
		}
		return true;
	}

	if (IsEmpty(wx, wy + ymove))
	{
		Particle p = Particle(*itr);
		// reset old particle to null
		(*itr)->id = 0;
		// update hash to new position
		m_WorldHash[wx][wy + ymove].id = p.id;
		// update AwakePool
		itr = m_AwakePool.erase(itr);
		AddParticleToPool(m_AwakePool, &m_WorldHash[wx][wy + ymove]);

		return true;
	}

	return false;
}

bool ParticleManager::TryMoveParticleHorizontal(std::set<Particle*>::iterator& itr, int& xmove)
{
	int wx((*itr)->x), wy((*itr)->y);
	ToWorldCoord(wx, wy);

	int ymove = 0;
	switch ((*itr)->id)
	{
	case 0: // gas
		std::cout << "Invalid Particle Type" << std::endl;
		break;
	case 10: // liquid
		ymove = 0;
		break;
	case 20: // solid
		ymove = 1;
		break;
	default:
		ymove = 0;
		break;
	}

	xmove = 0;

	int val = rand();

	if (val >= RAND_MAX * 0.5f)
	{
		xmove = 1;
	}
	else
	{
		xmove = -1;
	}

	if ((*itr)->id / 10 == 1)
	{
		if (IsSurrounded(wx, wy, 10))
		{
			return false;
		}
		else if (IsAwake(wx + xmove, wy))
		{
			itr++;
			return true;
		}
		else if (GetParticleType(wx + xmove, wy) == GetParticleType(wx, wy))
		{
			WakeParticle(wx + xmove, wy);
		}
		else if (GetParticleType(wx, wy - 1) == GetParticleType(wx, wy))
		{
			WakeParticle(wx, wy - 1);
		}
		else if (GetParticleType(wx, wy + 1) == GetParticleType(wx, wy) && !IsAwake(wx, wy + 1))
		{
			WakeParticle(wx, wy + 1);
		}
	}

	if (IsEmpty(wx + xmove, wy + ymove))
	{
		Particle p = Particle(*itr);
		// reset old particle to null
		(*itr)->id = 0;
		// update hash to new position
		m_WorldHash[wx + xmove][wy + ymove].id = p.id;
		// update AwakePool
		itr = m_AwakePool.erase(itr);
		AddParticleToPool(m_AwakePool, &m_WorldHash[wx + xmove][wy + ymove]);

		return true;
	}

	return false;
}

bool ParticleManager::TrySwapParticle(std::set<Particle*>::iterator& itr, int x, int y)
{
	int wx((*itr)->x), wy((*itr)->y);
	ToWorldCoord(wx, wy);

	if (IsValidCoord(x, y) && !IsEmpty(x, y))
	{
		if ((GetParticleType(x, y) != 0 && GetParticleType(wx, wy) != 0 &&
			 (GetParticleType(x, y) / 10 == 1 && GetParticleType(wx, wy) / 10 == 2)))
		{
			// store previous values
			Particle ptop = Particle(*itr);
			Particle pbottom = Particle(&m_WorldHash[x][y]);

			// swap values
			m_WorldHash[wx][wy].id = pbottom.id;
			m_WorldHash[x][y].id = ptop.id;

			// attempts to add the particle below it 
			AddParticleToPool(m_AwakePool, &m_WorldHash[x][y]);

			// replaces the original particle and increments the iterator
			itr = m_AwakePool.erase(itr);
			AddParticleToPool(m_AwakePool, &m_WorldHash[wx][wy]);

			// wake up bottom particle if not awake
			if (!IsAwake(x, y) && !IsEmpty(x, y))
			{
				WakeParticle(x, y);
			}

			return true;
		}
	}

	return false;
}

void ParticleManager::WakeParticle(int x, int y)
{
	if (IsValidCoord(x, y) && GetParticleType(x, y) != 0)
		AddParticleToPool(m_AwakePool, &m_WorldHash[x][y]);
}

bool ParticleManager::ShouldParticleSleep(std::set<Particle*>::iterator& itr)
{
	int wx((*itr)->x), wy((*itr)->y);
	ToWorldCoord(wx, wy);

	switch ((*itr)->id / 10)
	{
	case 0:
		std::cout << "Invalid Particle Type" << std::endl;
		break;
	case 1:
		return IsSurrounded(wx, wy, 10);
		break;
	case 2:
		if (!IsEmpty(wx, wy + 1) || IsSurrounded(wx, wy, 20))
		{
			return true;
		}
		break;
	default:
		return false;
		break;
	}

	return false;
}

void ParticleManager::InteractWithNeighbors(std::set<Particle*>::iterator& itr)
{
	int wx((*itr)->x), wy((*itr)->y);
	ToWorldCoord(wx, wy);

	ParticleType p = m_ParticleLookup[GetParticleType(wx, wy)];
	for (int i = 0; i < 4; i++)
	{
		switch (i)
		{
		case 0: // up
		{
			if (GetParticleType(wx, wy - 1) != -1) // if valid coord and filled
			{
				Interact(wx, wy - 1, wx, wy, p);
			}
			else
				break;

			break;
		}
		case 1: // down
		{
			if (GetParticleType(wx, wy + 1) != -1) // if valid coord and filled
			{
				Interact(wx, wy + 1, wx, wy, p);
			}
			else
				break;
			break;
		}
		case 2: // left
		{
			if (GetParticleType(wx - 1, wy) != -1) // if valid coord and filled
			{
				Interact(wx - 1, wy, wx, wy, p);
			}
			else
				break;

			break;
		}
		case 3: // right
		{
			if (GetParticleType(wx + 1, wy) != -1) // if valid coord and filled
			{
				Interact(wx + 1, wy, wx, wy, p);
			}
			else
				break;

			break;
		}
		default:
			break;
		}
	}
}

void ParticleManager::Interact(int toX, int toY, int fromX, int fromY, ParticleType fromParticle)
{
	ParticleType toParticle = m_ParticleLookup[GetParticleType(toX, toY)];
	if ((toParticle.IsFreezalbe || toParticle.IsCondensable) && fromParticle.CanUpState && rand() <= RAND_MAX * fromParticle.spreadValue)
	{
		if (toParticle.ID == 0)
			m_NumParticles++;
		m_WorldHash[toX][toY].id += 10; // state change upwards
		m_AwakePool.insert(&m_WorldHash[toX][toY]);
		WakeParticle(toX, toY + 1);
		WakeParticle(toX, toY - 1);
		return;
	}
	if ((fromParticle.IsFreezalbe || fromParticle.IsCondensable) && toParticle.CanUpState && rand() <= RAND_MAX * toParticle.spreadValue)
	{
		WakeParticle(fromX, fromY);
		WakeParticle(fromX, fromY + 1);
		WakeParticle(fromX, fromY - 1);
		WakeParticle(fromX + 1, fromY);
		WakeParticle(fromX - 1, fromY);
	}
}

void ParticleManager::SpawnParticle(int x, int y, int id)
{
	if (IsEmpty(x, y))
	{
		m_WorldHash[x][y].id = id;
		AddParticleToPool(m_AwakePool, &m_WorldHash[x][y]);
		m_NumParticles++;
	}
}

bool ParticleManager::IsValidCoord(int x, int y)
{
	return (
		x > 0 && x < m_Width &&
		y > 0 && y < m_Height
		);
}

bool ParticleManager::IsValidCoordScreen(int x, int y)
{
	ToWorldCoord(x, y);
	return IsValidCoord(x, y);
}

bool ParticleManager::IsEmpty(int x, int y)
{
	if (IsValidCoord(x, y))
	{
		return !m_WorldHash[x][y].id;
	}
	else
		return false;
}

bool ParticleManager::IsAwake(int x, int y)
{
	if (IsValidCoord(x, y))
	{
		return m_AwakePool.count(&m_WorldHash[x][y]);
	}

	return false;
}

bool ParticleManager::IsSurrounded(int x, int y, int id)
{
	bool surrounded = true;

	surrounded = (
			(id == GetParticleType(x + 1,	y + 1) || -1 == GetParticleType(x + 1,	y + 1))
		&& (id == GetParticleType(x,		y + 1) || -1 == GetParticleType(x,		y + 1))
		&& (id == GetParticleType(x - 1,	y + 1) || -1 == GetParticleType(x - 1,	y + 1))
		&& (id == GetParticleType(x + 1,	y - 1) || -1 == GetParticleType(x + 1,	y - 1))
		&& (id == GetParticleType(x,		y - 1) || -1 == GetParticleType(x,		y - 1))
		&& (id == GetParticleType(x - 1,	y - 1) || -1 == GetParticleType(x - 1,	y - 1))
		&& (id == GetParticleType(x + 1,	y)		 || -1 == GetParticleType(x + 1,	y)		)
		&& (id == GetParticleType(x - 1,	y)		 || -1 == GetParticleType(x - 1,	y)		)
	);

	return surrounded;
}

int ParticleManager::GetParticleType(int x, int y)
{
	if (IsValidCoord(x, y))
	{
		return m_WorldHash[x][y].id;
	}
	
	return -1;
}

void ParticleManager::ToWorldCoord(int& screenX, int& screenY)
{
	screenX -= m_X;
	screenY -= m_Y;
}

void ParticleManager::ToScreenCoord(int& worldX, int& worldY)
{
	worldX += m_X;
	worldY += m_Y;
}

void ParticleManager::AddParticleToPool(std::set<Particle*>& set, Particle* p)
{
	set.insert(p);
}

//void ParticleManager::TryFreeze(int x, int y, ParticleType parentType)
//{
//	if (IsValidCoord(x, y))
//	{
//		ParticleType toParticle = m_ParticleLookup[m_WorldHash[x][y].id];
//
//		if (toParticle.isFreezable && rand() <= RAND_MAX * parentType.spreadValue)
//		{
//			m_WorldHash[x][y].id -= 10; // state change upwards
//			m_AwakePool.insert(&m_WorldHash[x][y]);
//			WakeParticle(x, y + 1);
//			WakeParticle(x, y - 1);
//		}
//	}
//}
//
//void ParticleManager::TryMelt(int x, int y, ParticleType parentType)
//{
//	if (IsValidCoord(x, y))
//	{
//		ParticleType toParticle = m_ParticleLookup[m_WorldHash[x][y].id];
//
//		if (toParticle.isMeltable && rand() <= RAND_MAX * parentType.spreadValue)
//		{
//			m_WorldHash[x][y].id -= 10; // state change downwards
//			m_AwakePool.insert(&m_WorldHash[x][y]);
//			WakeParticle(x, y + 1);
//			WakeParticle(x, y - 1);
//		}
//	}
//}
