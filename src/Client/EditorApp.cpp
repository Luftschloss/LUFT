#include "Luft.h"

class EditorApp : public Luft::Application
{
public:
	EditorApp()
	{

	}

	~EditorApp()
	{

	}
};

Luft::Application* Luft::CreateApplication()
{
	return new EditorApp();
}
