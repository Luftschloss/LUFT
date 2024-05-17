#pragma once

#include "Base.h"
#include "Timestep.h"
#include "Luft/Events/Event.h"

namespace Luft {

	class Layer
	{
	public:
		Layer(const lstr& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		const lstr& GetName() const { return m_DebugName; }
	protected:
		lstr m_DebugName;
	};
}