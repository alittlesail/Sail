#ifndef SAIL_2D_OBJECT_INCLUDED
#define SAIL_2D_OBJECT_INCLUDED

#include <memory>

#include "Carp/carp_math_2d.hpp"

class Sail2DObject;
using Sail2DObjectPtr = std::shared_ptr<Sail2DObject>;
using Sail2DObjectWeakPtr = std::weak_ptr<Sail2DObject>;

class Sail2DObject : public std::enable_shared_from_this<Sail2DObject>
{
public:
	virtual ~Sail2DObject() { }

public:
	virtual void Render(const CarpMatrix2D& parent, bool parent_changed) { }

public:
	// ��ʾ�����ء��ü�
	virtual void SetVisible(bool visible) { if (visible == false) m_self_matrix_dirty = true; m_visible = visible; }
	virtual bool GetVisible() const { return m_visible; }
	virtual void SetClip(bool clip) { if (clip == true) m_self_matrix_dirty = true; m_clip = clip; }

	// ���úͻ�ȡ����
	virtual void SetX(int x) { if (m_pos_x == x) return; m_pos_x = x; m_self_matrix_dirty = true; }
	virtual int GetX() const { return m_pos_x; }
	virtual void SetY(int y) { if (m_pos_y == y) return; m_pos_y = y; m_self_matrix_dirty = true; }
	virtual int GetY() const { return m_pos_y; }

	// ���úͻ�ȡ���
	virtual void SetWidth(int width) { if (m_width == width) return; m_width = width; m_self_matrix_dirty = true; }
	virtual int GetWidth() { return m_width; }
	virtual void SetHeight(int height) { if (m_height == height) return; m_height = height; m_self_matrix_dirty = true; }
	virtual int GetHeight() { return m_height; }

	// ������ת
	virtual void SetRotate(float rotate) { m_rotate = rotate; m_self_matrix_dirty = true; }

	// ��������
	virtual void SetScaleX(float x) { m_scale_x = x; m_self_matrix_dirty = true; }
	virtual void SetScaleY(float y) { m_scale_y = y; m_self_matrix_dirty = true; }

	// ����ê��
	virtual void SetCenterX(int x) { if (m_center_x == x) return; m_center_x = x; m_self_matrix_dirty = true; }
	virtual void SetCenterY(int y) { if (m_center_y == y) return; m_center_y = y; m_self_matrix_dirty = true; }

	// ������ɫ
	virtual void SetRed(float red) { m_red = red; m_color_dirty = true; }
	virtual void SetGreen(float green) { m_green = green; m_color_dirty = true; }
	virtual void SetBlue(float blue) { m_blue = blue; m_color_dirty = true; }
	virtual void SetAlpha(float alpha) { m_alpha = alpha; m_color_dirty = true; }

public:
	// ���ñ�������Ϊ��
	virtual void SetSelfMatrixDirty() { m_self_matrix_dirty = true; }

	// ���±��ؾ���
	virtual bool UpdateSelfMatrix2D()
	{
		// ������ؾ���û���࣬��ôֱ�ӷ���false
		if (!m_self_matrix_dirty) return false;
		// ���Ϊ����
		m_self_matrix_dirty = false;
		// �������
		m_vertex_dirty = true;

		// ����ê��
		m_self_matrix.SetTranslation(static_cast<float>(-m_center_x), static_cast<float>(-m_center_y));
		// ��������
		m_self_matrix.Scale(m_scale_x, m_scale_y);
		// ������ת
		m_self_matrix.Rotate(m_rotate);
		// ��������
		m_self_matrix.Translation(static_cast<float>(m_pos_x), static_cast<float>(m_pos_y));

		return true;
	}

	// �����������
	virtual bool UpdateGlobalMatrix2D(const CarpMatrix2D& parent, bool parent_changed)
	{
		const bool self_update = UpdateSelfMatrix2D();

		// ���ؾ�����Ҫ���»��߸��ڵ�仯����ô��Ҫ�����������
		if (parent_changed || self_update)
		{
			// ���Ʊ��ؾ���
			m_global_matrix = m_self_matrix;
			// ���Ը��ڵ����
			m_global_matrix.Multiply(parent);
			// �����������
			m_vertex_dirty = true;
			return true;
		}

		return false;
	}

	// ������������
	virtual void UpdateTextureCoordinate() {}
	// ���¶�������
	virtual void UpdateVertexCoordinate() {}
	// ������ɫ
	virtual void UpdateVertexColor() {}

protected:
	bool m_visible = true;				// �Ƿ���ʾ
	bool m_clip = false;				// �Ƿ�ü�

protected:
	int m_pos_x = 0, m_pos_y = 0;								// ����
	float m_scale_x = 1.0f, m_scale_y = 1.0f;					// ����
	float m_red = 1.0f;											// ��ɫ
	float m_green = 1.0f;
	float m_blue = 1.0f;
	float m_alpha = 1.0f;
	int m_width = 0, m_height = 0;								// ���
	int m_center_x = 0, m_center_y = 0;							// ê��
	float m_rotate = 0.0f;										// ��ת����

protected:
	CarpMatrix2D m_self_matrix;			// ���汾�ؾ���
	bool m_self_matrix_dirty = true;	// ���౾���������
	CarpMatrix2D m_global_matrix;		// �����������
	bool m_vertex_dirty = true;			// ���ඥ������
	bool m_texture_dirty = true;		// ��������
	bool m_color_dirty = true;			// ������ɫ

public:
	Sail2DObjectWeakPtr m_front_node;	// ǰһ���ֵܽڵ�
	Sail2DObjectPtr m_back_node;		// ��һ���ֵܽڵ�
	Sail2DObjectWeakPtr m_parent_node;	// ���ڵ�
};

#endif


