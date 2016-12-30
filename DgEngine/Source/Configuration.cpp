////////////////////////////////////////////////////////////////////////////////
// Filename: Configuration.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <irrXML/irrXML.h>
#include <GL/glew.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Configuration.h"
#include "Glossary.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace irr;
using namespace io;
using namespace dg;


#define MAX_MSAA_SAMPLES 16

static IrrXMLReader* xml = nullptr;

unique_ptr<Configuration> Configuration::m_Instance = nullptr;

Configuration::Configuration() {
	InitializeDefaultValues();
	Initialize();
}

const unique_ptr<Configuration>& Configuration::Instance() {
	if(!m_Instance) {
		m_Instance = unique_ptr<Configuration>(new Configuration());

	}
	return m_Instance;
}

void Configuration::Release() {
	m_Instance = nullptr;
}

void Configuration::Initialize() {
	xml = createIrrXMLReader(CONFIG_FILE_PATH);

	bool exit = false;
	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT:{
			if(string(xml->getNodeName())=="NameApplication") {
				string name = xml->getAttributeValue("name");
				m_NameApplication = name;
			} else if(string(xml->getNodeName())=="Window") {
				m_ScreenWidth = std::stoi(xml->getAttributeValue("width"));
				m_ScreenHeigth = std::stoi(xml->getAttributeValue("height"));
				m_FullScreen = string(xml->getAttributeValue("fullScreen"))=="true" ? true : false;
				m_Centered = string(xml->getAttributeValue("centered"))=="true" ? true : false;
				m_ScreenXPosition = std::stoi(xml->getAttributeValue("xPos"));
				m_ScreenYPosition = std::stoi(xml->getAttributeValue("yPos"));
			} else if(string(xml->getNodeName())=="Resize") {
				m_Resizable = string(xml->getAttributeValue("enabled"))=="true" ? true : false;
			} else if(string(xml->getNodeName())=="Decorated") {
				m_Decorated = string(xml->getAttributeValue("enabled"))=="true" ? true : false;
			} else if(string(xml->getNodeName())=="Floating") {
				m_Floating = string(xml->getAttributeValue("enabled"))=="true" ? true : false;
			} else if(string(xml->getNodeName())=="MSAA") {
				string value = string(xml->getAttributeValue("sample"));
				if(value=="max") {
					m_Samples = MAX_MSAA_SAMPLES;
				} else {
					m_Samples = std::stoi(value);
				}
				int sample = std::stoi(xml->getAttributeValue("sample"));
			} else if(string(xml->getNodeName())=="DepthBuffer") {
				m_DepthBuffer = string(xml->getAttributeValue("enabled"))=="true" ? true : false;
			} else if(string(xml->getNodeName())=="StencilBuffer") {
				m_StencilBuffer = string(xml->getAttributeValue("enabled"))=="true" ? true : false;
			} else if(string(xml->getNodeName())=="Culling") {
				m_Culling = string(xml->getAttributeValue("enabled"))=="true" ? true : false;
				string type = string(xml->getAttributeValue("type"));
				if(type=="frontAndBack") {
					m_CullingMode = GL_FRONT_AND_BACK;
				} else if(type=="front") {
					m_CullingMode = GL_FRONT;
				} else {
					m_CullingMode = GL_BACK;
				}
			} else if(string(xml->getNodeName())=="PolygonMode") {
				string type = string(xml->getAttributeValue("type"));
				if(type=="CW") {
					m_PolygonMode = GL_CW;
				} else {
					m_PolygonMode = GL_CCW;
				}
			} else if(string(xml->getNodeName())=="Blending") {
				m_Blending = string(xml->getAttributeValue("enabled"))=="true" ? true : false;
			} else if(string(xml->getNodeName())=="RefreshRate") {
				int rate = std::stoi(xml->getAttributeValue("rate"));
				if(rate>0) {
					m_RefreshRate = rate;
				}
			} 
		} break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())=="Configuration") {
				exit = true;
			}
		} break;
		default: break;
		}
	}

	delete xml;
	xml = nullptr;
}

void Configuration::InitializeDefaultValues() {
	m_NameApplication = "DgEngineApp";
	m_FullScreen = false;
	m_ScreenWidth = 800;
	m_ScreenHeigth = 600;
	m_Centered = false;
	m_ScreenXPosition = 0x10;
	m_ScreenYPosition = 0x10;
	m_Resizable = true;
	m_Decorated = true;
	m_Floating = true;
	m_Samples = 0x0;
	m_DepthBuffer = true;
	m_StencilBuffer = false;
	m_Culling = true;
	m_CullingMode = GL_BACK;
	m_PolygonMode = GL_CCW;
	m_Blending = false;
	m_RefreshRate = 60;
}

Configuration::~Configuration() {

}

const string& Configuration::ApplicationName() const {
	return m_NameApplication;
}

void Configuration::SetScreenWidth(int value) {
	m_ScreenWidth = value;
}

const int Configuration::GetScreenWidth() const {
	return m_ScreenWidth;
}

void Configuration::SetScreenHeigth(int value) {
	m_ScreenHeigth = value;
}

const int Configuration::GetScreenHeigth() const {
	return m_ScreenHeigth;
}

const bool& Configuration::IsScreenCentered() const {
	return m_Centered;
}

void Configuration::SetXScreenPosition(int value) {
	m_ScreenXPosition = value;
}

const int& Configuration::GetXScreenPosition() const {
	return m_ScreenXPosition;
}

void Configuration::SetYScreenPosition(int value) {
	m_ScreenYPosition = value;
}

const int& Configuration::GetYScreenPosition() const {
	return m_ScreenYPosition;
}

const bool& Configuration::IsFullScreen() const {
	return m_FullScreen;
}

const bool& Configuration::IsDepthEnabled() const {
	return m_DepthBuffer;
}

const bool& Configuration::IsStencilEnabled() const {
	return m_StencilBuffer;
}

const bool& Configuration::IsBlendingEnabled() const {
	return m_Blending;
}

const bool& Configuration::IsResizable() const {
	return m_Resizable;
}

const bool& Configuration::IsDecorated() const {
	return m_Decorated;
}

const bool& Configuration::IsFloating() const {
	return m_Floating;
}

const int& Configuration::GetSamples() const {
	return m_Samples;
}

const bool& Configuration::IsCullingEnabled() const {
	return m_Culling;
}

const int& Configuration::GetCullingMode() const {
	return m_CullingMode;
}

const int& Configuration::GetPolygonMode() const {
	return m_PolygonMode;
}

const int& Configuration::GetRefreshRate() const {
	return m_RefreshRate;
}
