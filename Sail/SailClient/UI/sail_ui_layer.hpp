#ifndef SAIL_UI_LAYER_INCLUDED
#define SAIL_UI_LAYER_INCLUDED

#include "sail_ui_object.hpp"
#include "sail_ui_objects.hpp"

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

	void RemoveLayer(const SailUIObjectsPtr& layer)
	{
	    for (auto it = m_normal_groups.begin(); it != m_normal_groups.end(); ++it)
	    {
	        if (*it == layer)
	        {
				m_normal_groups.erase(it);
				return;
	        }
	    }
	}

	void AddToModal(const SailUIObjectPtr& dialog) const
	{
		if (!dialog) return;

		dialog->SetXType(SailUIPosType::POS_ALIGN_CENTER);
		dialog->SetXValue(0);
		dialog->SetYType(SailUIPosType::POS_ALIGN_CENTER);
		dialog->SetYValue(0);
		dialog->SetModal(true);
		dialog->SetVisible(false);
		m_modal_layer->AddChild(dialog);
	}

	void RemoveFromModal(const SailUIObjectPtr& dialog) const
	{
		if (!dialog) return;

		dialog->SetModal(false);
		dialog->SetVisible(true);
		m_modal_layer->RemoveChild(dialog);
	}

	void ShowFromRight(const SailUIObjectPtr& dialog, bool focus = true)
	{
		if (m_right_show == dialog) return;

		if (m_right_show)
		{
			SailUIHideEvent event;
			m_right_show->DispatchEvent(event);
			m_right_layer->RemoveChild(m_right_show);
			m_right_show = nullptr;
		}

		m_right_show = dialog;

		if (m_right_show)
		{
			m_right_layer->AddChild(m_right_show);
			if (focus) m_right_show->SetFocus(true);

			SailUIShowEvent event;
			dialog->DispatchEvent(event);
			dialog->SetVisible(true);
		}
	}

	void HideFromRight(const SailUIObjectPtr& dialog)
	{
		if (m_right_show == dialog) return;
		if (m_right_show == nullptr) return;

		SailUIHideEvent event;
		m_right_show->DispatchEvent(event);
		m_right_layer->RemoveChild(m_right_show);
		m_right_layer->SetFocus(false);
		m_right_show = nullptr;
	}

	bool HideCurrentFromRight()
	{
		if (!m_right_show) return false;
		HideFromRight(m_right_show);
		return true;
	}

	bool IsCurrentRight(const SailUIObjectPtr& dialog) const
	{
		return m_right_show == dialog;
	}

	void AddToTip(const SailUIObjectPtr& dialog)
	{
		m_tip_layer->AddChild(dialog);
	}

	void RemoveFromTip(const SailUIObjectPtr& dialog)
	{
		m_tip_layer->RemoveChild(dialog);
	}

public:
	void HandleViewResized(int width, int height)
	{
		if (width > 0 && height > 0)
		{
			m_root.Scale(2.0f / static_cast<float>(width), -2.0f / static_cast<float>(height));
			m_root.Translation(-1, 1);
		}

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

	void PickUp(int x, int y, SailUIObjectPtr& out_mfc, SailUIObjectPtr& out_mfd, int& out_x, int& out_y)
	{
		out_mfd = nullptr;
		m_right_layer->PickUp(x, y, out_mfc, out_x, out_y);
		if (out_mfc) return;

		m_modal_layer->PickUp(x, y, out_mfc, out_x, out_y);
		if (out_mfc) return;

		for (auto it = m_normal_groups.rbegin(); it != m_normal_groups.rend(); ++it)
		{
			const auto& children = (*it)->GetChildren();
			for (auto sub_it = children.rbegin(); sub_it != children.rend(); ++sub_it)
			{
				LayerPickUp(*sub_it, x, y, out_mfc, out_mfd, out_x, out_y);
				if (out_mfc) return;
			}
		}
	}

	void HandleLRButtonDown(int x, int y)
	{
		if (m_right_show == nullptr) return;

		SailUIObjectPtr v_pick;
		int v_x = 0, v_y = 0;
		m_right_show->PickUpSelf(x, y, v_pick, v_x, v_y);
		if (!v_pick) return;

		SailUIHideEvent event;
		m_right_show->DispatchEvent(event);
		m_right_show->SetVisible(false);
		m_right_show = nullptr;
	}

protected:
	static void LayerPickUp(const SailUIObjectPtr& layer, int x, int y, SailUIObjectPtr& out_mfc, SailUIObjectPtr& out_mfd, int& out_x, int& out_y)
	{
		out_mfc = nullptr;
		out_mfd = nullptr;
		if (!layer->GetVisible() || layer->GetDisabled()) return;

		const auto rel_x = x - layer->GetX();
		const auto rel_y = y - layer->GetY();

		const auto& children = layer->GetChildren();
		for (auto it = children.rbegin(); it != children.rend(); ++it)
		{
			(*it)->PickUp(rel_x, rel_y, out_mfc, out_x, out_y);
			if (out_mfc)
			{
				out_mfd = *it;
				return;
			}
		}
	}

public:
	void Render()
	{
		for (auto& layer : m_normal_groups) layer->GetNativeShow()->Render(m_root, false);
		m_modal_layer->GetNativeShow()->Render(m_root, false);
		m_right_layer->GetNativeShow()->Render(m_root, false);
		m_tip_layer->GetNativeShow()->Render(m_root, false);
	}

private:
	std::vector<SailUIObjectsPtr> m_normal_groups{};
	SailUIObjectsPtr m_modal_layer;
	SailUIObjectsPtr m_right_layer;
	SailUIObjectPtr m_right_show;
	SailUIObjectsPtr m_tip_layer;

private:
	CarpMatrix2D m_root;
};

extern SailUILayer s_sail_ui_layer;

#endif

#ifdef SAIL_UI_LAYER_IMPL
#ifndef SAIL_UI_LAYER_IMPL_INCLUDE
#define SAIL_UI_LAYER_IMPL_INCLUDE
SailUILayer s_sail_ui_layer;
#endif
#endif

