#pragma once

#include <vector>
#include <glad/glad.h>

static GLFWwindow* pCurrentWindow = nullptr;
static std::vector<void*> registeredElements;

static GLuint frameBuffer;
static GLuint frameBufferTexture;

static unsigned int viewportWidth = 100;
static unsigned int viewportHeight = 100;

static bool debugWindow{ true };
static bool modelWindow{ true };
static bool profilerWindow{ true };
static bool viewportWindow{ true };
static bool rendererWindow{ true };
static bool demoWindow{ false };