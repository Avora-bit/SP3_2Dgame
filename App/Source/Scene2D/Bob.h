#pragma once
#include "Enemy2D.h"
#include <stdlib.h>

class Bob : public CEnemy2D
{
public:
	Bob();
	Bob(glm::vec2 pos);
	~Bob();
	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);
private:
	float shotInterval;
	float attackTimer;
	
};
