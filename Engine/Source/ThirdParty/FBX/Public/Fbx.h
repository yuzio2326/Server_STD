#pragma once
#include "CoreMinimal.h"
#include "fbxsdk.h"

#if WITH_EDITOR
#pragma comment(lib, "libfbxsdk.lib")
#else
#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")
#endif