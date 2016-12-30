////////////////////////////////////////////////////////////////////////////////
// Filename: Configuration.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <memory>
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class SceneLoader;

	class Configuration {
		friend class SceneLoader;
	public:
		static const unique_ptr<Configuration>& Instance();
		static void Release();

	public:
		virtual ~Configuration();
		
		const string& ApplicationName() const;
		void SetScreenWidth(int);
		const int GetScreenWidth() const;
		void SetScreenHeigth(int);
		const int GetScreenHeigth() const;
		const bool&	IsScreenCentered() const;
		void SetXScreenPosition(int value);
		const int& GetXScreenPosition() const;
		void SetYScreenPosition(int value);
		const int& GetYScreenPosition() const;
		const bool&	IsFullScreen() const;
		const bool&	IsDepthEnabled() const;
		const bool& IsStencilEnabled() const;
		const bool& IsBlendingEnabled() const;
		const bool&	IsResizable() const;
		const bool&	IsDecorated() const;
		const bool& IsFloating() const;
		const int& GetSamples() const;
		const bool& IsCullingEnabled() const;
		const int& GetCullingMode() const;
		const int& GetPolygonMode() const;
		const int& GetRefreshRate() const;

	private:
		Configuration();
		void Initialize();
		void InitializeDefaultValues();

	private:
		static unique_ptr<Configuration>	m_Instance;
		string								m_NameApplication;
		bool								m_FullScreen;
		int									m_ScreenWidth;
		int									m_ScreenHeigth;
		bool								m_Centered;
		int 								m_ScreenXPosition;
		int 								m_ScreenYPosition;
		bool 								m_Resizable;
		bool 								m_Decorated;
		bool 								m_Floating;
		int 								m_Samples;
		bool 								m_DepthBuffer;
		bool 								m_StencilBuffer;
		bool 								m_Culling;
		int 								m_CullingMode;
		int 								m_PolygonMode;
		bool 								m_Blending;
		int 								m_RefreshRate;
	};
}
