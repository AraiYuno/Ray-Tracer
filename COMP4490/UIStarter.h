#ifndef _UISTARTER_H
#define _UISTARTER_H
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <list>
#include "a1main.h"

#pragma once
class UIStarter
{
public:
	UIStarter();
	~UIStarter();

	int sceneSelector();
	void createScene(list<Mesh*> *meshList, int selector);
	void createLights(list<Light*> *lightList, int selector);
};

#endif

