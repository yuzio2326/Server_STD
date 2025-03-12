#pragma once
#include "CoreTypes.h"
#include "ModuleInterface.h"

#if !WITH_EDITOR
	// If we're linking monolithically we assume all modules are linked in with the main binary.
	#define IMPLEMENT_MODULE( ModuleImplClass, ModuleName ) \
		/** Global registrant object for this module when linked statically */ \
		static FStaticallyLinkedModuleRegistrant< ModuleImplClass > ModuleRegistrant##ModuleName( TEXT(#ModuleName) ); \
		/* Forced reference to this function is added by the linker to check that each module uses IMPLEMENT_MODULE */ \
		extern "C" void IMPLEMENT_MODULE_##ModuleName() { }
#else
	#define IMPLEMENT_MODULE( ModuleImplClass, ModuleName ) \
	\
	/**/ \
	/* InitializeModule function, called by module manager after this module's DLL has been loaded */ \
	/**/ \
	/* @return	Returns an instance of this module */ \
	/**/ \
	extern "C" DLLEXPORT IModuleInterface* InitializeModule() \
	{ \
		return new ModuleImplClass(); \
	} \
	/* Forced reference to this function is added by the linker to check that each module uses IMPLEMENT_MODULE */ \
	extern "C" void IMPLEMENT_MODULE_##ModuleName() { }
#endif


/**
 * Function pointer type for InitializeModule().
 *
 * All modules must have an InitializeModule() function. Usually this is declared automatically using
 * the IMPLEMENT_MODULE macro below. The function must be declared using as 'extern "C"' so that the
 * name remains undecorated. The object returned will be "owned" by the caller, and will be deleted
 * by the caller before the module is unloaded.
 */
typedef IModuleInterface* (*FInitializeModuleFunctionPtr)(void);

/**
 * A default minimal implementation of a module that does nothing at startup and shutdown
 */
class FDefaultModuleImpl
	: public IModuleInterface
{ };

class FModuleManager
{
private:
	/**
	 * Information about a single module (may or may not be loaded.)
	 */
	class FModuleInfo
	{
	public:
		/** The original file name of the module, without any suffixes added */
		FString OriginalFilename;

		/** File name of this module (.dll file name) */
		FString Filename;

		/** dll 전체 경로 */
		FString FullPath;

		/** Handle to this module (DLL handle), if it's currently loaded */
		void* Handle;

		bool bDynamicLoaded = false;

		/** The module object for this module.  We actually *own* this module, so it's lifetime is controlled by the scope of this shared pointer. */
		std::unique_ptr<IModuleInterface> Module;

	};

	typedef TObjectPtr<FModuleInfo> ModuleInfoPtr;
	/** Type definition for maps of module names to module infos. */
	typedef std::map<FName, ModuleInfoPtr> FModuleMap;

public:
	FModuleManager();
	static CORE_API FModuleManager* Get(const bool bDestroy = false);

public:
	// HBKIM: not thread safe yet. /** Thread safe module finding routine. */
	CORE_API ModuleInfoPtr FindModule(FName InModuleName);

	/**
	 * 지정된 모듈을 로드합니다.
	 *
	 * @param InModuleName 모듈 파일의 기본 이름입니다. 경로, 확장자 또는 플랫폼/구성 정보는 포함하지 않아야 합니다. 이는 모듈 파일 이름의 "모듈 이름" 부분입니다. 이름은 전역적으로 고유해야 합니다.
	 * @param InLoadModuleFlags 모듈 로드 작업을 위한 선택적 플래그입니다.
	 * @return 로드된 모듈을 반환하거나, 로드 작업이 실패한 경우 nullptr을 반환합니다.
	 * @see AbandonModule, IsModuleLoaded, LoadModuleChecked, LoadModulePtr, LoadModuleWithFailureReason, UnloadModule
	 */
	CORE_API IModuleInterface* LoadModule(const FName InModuleName);

	/**
	  * 이름으로 모듈을 로드합니다.
	  *
	  * @param ModuleName 찾고 로드할 모듈의 이름입니다.
	  * @return 모듈에 대한 인터페이스를 반환하며, 모듈을 찾을 수 없으면 nullptr을 반환합니다.
	  * @see GetModulePtr, GetModuleChecked, LoadModuleChecked
	  */
	template<typename TModuleInterface>
	static TModuleInterface* LoadModulePtr(const FName InModuleName)
	{
		return dynamic_cast<TModuleInterface*>(FModuleManager::Get()->LoadModule(InModuleName));
	}

	/**
	 * 이름으로 모듈을 로드하고, 존재하는지 확인합니다.
	 *
	 * 이 메서드는 모듈이 실제로 존재하는지 확인합니다. 모듈이 존재하지 않으면 assertion이 발생합니다.
	 * 모듈이 이미 이전에 로드된 경우, 기존 인스턴스가 반환됩니다.
	 *
	 * @param ModuleName 찾고 로드할 모듈
	 * @return 지정된 typename으로 캐스트된 모듈 인터페이스를 반환합니다.
	 * @see GetModulePtr, LoadModulePtr, LoadModuleChecked
	 */
	template<typename TModuleInterface>
	static TModuleInterface& LoadModuleChecked(const FName InModuleName)
	{
		IModuleInterface& ModuleInterface = FModuleManager::Get()->LoadModuleChecked(InModuleName);
		return static_cast<TModuleInterface&>(ModuleInterface);
	}

	/**
	 * 지정된 모듈을 로드하고, 존재하는지 확인합니다.
	 *
	 * @param InModuleName 모듈 파일의 기본 이름입니다. 경로, 확장자 또는 플랫폼/구성 정보는 포함하지 않아야 합니다.
	 * 이는 모듈 파일 이름의 "모듈 이름" 부분입니다. 이름은 전역적으로 고유해야 합니다.
	 * @return 로드된 모듈 또는 로드 작업이 실패한 경우 nullptr을 반환합니다.
	 * @see AbandonModule, IsModuleLoaded, LoadModuleChecked, LoadModulePtr, LoadModuleWithFailureReason, UnloadModule
	 */
	CORE_API IModuleInterface& LoadModuleChecked(const FName InModuleName);

	/**
	 * Registers an initializer for a module that is statically linked.
	 *
	 * @param InModuleName The name of this module.
	 * @param InInitializerDelegate The delegate that will be called to initialize an instance of this module.
	 */
	void RegisterStaticallyLinkedModule(const FName InModuleName, const function<IModuleInterface*(void)> InInitializerDelegate)
	{
		//PendingStaticallyLinkedModuleInitializers.Emplace(InModuleName, InInitializerDelegate);
		StaticallyLinkedModuleInitializers.insert(make_pair(InModuleName, InInitializerDelegate));
	}

	/**
	 * Adds a module to our list of modules, unless it's already known.
	 *
	 * @param InModuleName The base name of the module file.  Should not include path, extension or platform/configuration info.  This is just the "name" part of the module file name.  Names should be globally unique.
	 */
	CORE_API void AddModule(const FName InModuleName);

private:
	/** Map of module names to a delegate that can initialize each respective statically linked module */
	mutable map<FName, function<IModuleInterface*()>> StaticallyLinkedModuleInitializers;

	FModuleMap Modules;

	map<FName, FString> ModulePathsCache;
};

/**
 * Utility class for registering modules that are statically linked.
 */
template< class ModuleClass >
class FStaticallyLinkedModuleRegistrant
{
public:

	/**
	 * Explicit constructor that registers a statically linked module
	 */
	FStaticallyLinkedModuleRegistrant(FName InModuleName)
	{
		// Register this module
		FModuleManager::Get()->RegisterStaticallyLinkedModule(
			InModuleName,			// Module name
			&FStaticallyLinkedModuleRegistrant<ModuleClass>::InitializeModule);	// Initializer delegate
	}

	/**
	 * Creates and initializes this statically linked module.
	 *
	 * The module manager calls this function through the delegate that was created
	 * in the @see FStaticallyLinkedModuleRegistrant constructor.
	 *
	 * @return A pointer to a new instance of the module.
	 */
	static IModuleInterface* InitializeModule()
	{
		return new ModuleClass();
	}
};