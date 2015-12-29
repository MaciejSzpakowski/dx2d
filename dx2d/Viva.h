#pragma once

#include "Proto.h"
#include "Types.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Functions.h"
#include "Window.h"
#include "BitmapFont.h"
#include "Keys.h"
#include "InputManager.h"
#include "EventManager.h"
#include "DebugManager.h"
#include "Dynamic.h"
#include "Polygon.h"
#include "Sprite.h"
#include "Animation.h"
#include "BitmapText.h"
#include "Camera.h"
#include "RenderTarget.h"
#include "Core.h"
#include "DrawManager.h"

/*
name = input("name:")
type = input("type:")
primitives = ["int","double","size_t","float","char","byte"]

print(type + " Get" + name[0].upper() + name[1:] + "() const { return " + name + "; }")
if type in primitives:
print("void Set" + name[0].upper() + name[1:] + "(" + type + " _"+name + ")"+ " { "+name+" = "+"_"+name+"; }")
else:
print("void Set" + name[0].upper() + name[1:] + "(const " + type+"&" + " _"+name + ")"+ " { "+name+" = "+"_"+name+"; }")
*/