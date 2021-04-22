#ifndef SAIL_UI_LAYER_INCLUDED
#define SAIL_UI_LAYER_INCLUDED

#include "sail_2d_object.hpp"
#include "sail_2d_objects.hpp"

class SailUILayer
{
public:
	SailUILayer()
	{
		m_modal_layer = std::make_shared<SailUIObjects>();
		m_right_layer = std::make_shared<SailUIObjects>();
		m_tip_layer = std::make_shared<SailUIObjects>();
	}

public:
	void AddLayer(const SailUIObjectsPtr& layer)
	{
		m_normal_groups.push_back(layer);
	}

	void AddToModal(const SailUIObjectPtr& dialog)
	{
		if (!dialog) return;

		dialog.x_type = UIEnumTypes.POS_ALIGN_CENTER;
		dialog.x_value = 0;
		dialog.y_type = UIEnumTypes.POS_ALIGN_CENTER;
		dialog.y_value = 0;
		dialog.modal = true;
		dialog->SetVisible(false);
		m_modal_layer->AddChild(dialog);
	}

public:
	void HandleViewResized(int width, int height)
	{
	    for (auto& layer : m_normal_groups)
	    {
			layer->SetWidth(width);
			layer->SetHeight(height);
	    }

		m_modal_layer->SetWidth(width);
		m_modal_layer->SetHeight(height);

		m_right_layer->SetWidth(width);
		m_right_layer->SetHeight(height);

		m_tip_layer->SetWidth(width);
		m_tip_layer->SetHeight(height);

		HideCurrentFromRight();
	}

	void HideCurrentFromRight()
	{
	    
	}

public:
	void Render()
	{
		const CarpMatrix2D empty;
		for (auto& layer : m_normal_groups) layer->Render(empty, false);
		m_modal_layer->Render(empty, false);
		m_right_layer->Render(empty, false);
		m_tip_layer->Render(empty, false);
	}

private:
	std::vector<SailUIObjectsPtr> m_normal_groups;
	SailUIObjectsPtr m_modal_layer;
	SailUIObjectsPtr m_right_layer;
	SailUIObjectPtr m_right_show;
	SailUIObjectsPtr m_tip_layer;
};

extern SailUILayer s_sail_ui_layer;

#endif

#ifdef SAIL_UI_LAYER_IMPL
SailUILayer s_sail_ui_layer;
#endif

