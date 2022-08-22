/**
 Hilt2D
 */
#include "Hilt2D.h"

#include <iostream>
CHilt2D::CHilt2D(void)
{
}

CHilt2D::~CHilt2D(void)
{
}

float CHilt2D::getBonusMultiplier()
{
    return bonusMultiplier;
}

unsigned short int CHilt2D::getLight()
{
    return light;
}

unsigned short int CHilt2D::getRavenous()
{
    return ravenous;
}

unsigned short int CHilt2D::getWide()
{
    return wide;
}

unsigned short int CHilt2D::getSharp()
{
    return sharp;
}

void CHilt2D::addLight(int level)
{
    light += level;
}

void CHilt2D::addRavenous(int level)
{
    ravenous += level;
}

void CHilt2D::addWide(int level)
{
    wide += level;
}

void CHilt2D::addSharp(int level)
{
    sharp += level;
}
