#include <ChaiGraphicsExport.h>
#include <Core/Containers.h>

namespace CGraphics
{
	//template <typename T>
	//concept DerivedFromParent = std::is_base_of<IRenderer, T>::value;

	enum CHAIGRAPHICS_EXPORT ShaderStage
	{
		VERTEX,
		FRAGMENT,
		NONE
	};

	struct CHAIGRAPHICS_EXPORT ShaderData
	{
		Core::CString shaderSource = "";
		ShaderStage stage = ShaderStage::NONE;
	};

	class CHAIGRAPHICS_EXPORT RenderObject
	{
	public:
		RenderObject();
		~RenderObject();
		void AddShader(std::string dataString, ShaderStage stage);

		Core::CVector<float> m_verts;
		Core::CVector<ShaderData> m_data;
	};
}