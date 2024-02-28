#pragma once

#include "KeyboardInput.h"
#include "MouseInput.h"
#include "GamePadInput.h"

#include "SceneManager.h"

#include "Camera.h"
#include "FollowCamera.h"

#include "LightingGroup.h"

#include "Geometry/ObjModel.h"
#include "Geometry/Sprite2D.h"
#include "Geometry/Sprite3D.h"


class BaseScene
{
public:
	virtual void Initialize() {};

	virtual void Update() {};

	virtual void Finalize() {};

	virtual void GeometryDraw() {};
	virtual void SpriteDraw() {};
	virtual void ParticleDraw() {};
};

