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
	// 显示、隐藏、裁剪
	virtual void SetVisible(bool visible) { if (visible == false) m_self_matrix_dirty = true; m_visible = visible; }
	virtual bool GetVisible() const { return m_visible; }
	virtual void SetClip(bool clip) { if (clip == true) m_self_matrix_dirty = true; m_clip = clip; }

	// 设置和获取坐标
	virtual void SetX(int x) { if (m_pos_x == x) return; m_pos_x = x; m_self_matrix_dirty = true; }
	virtual int GetX() const { return m_pos_x; }
	virtual void SetY(int y) { if (m_pos_y == y) return; m_pos_y = y; m_self_matrix_dirty = true; }
	virtual int GetY() const { return m_pos_y; }

	// 设置和获取宽高
	virtual void SetWidth(int width) { if (m_width == width) return; m_width = width; m_self_matrix_dirty = true; }
	virtual int GetWidth() { return m_width; }
	virtual void SetHeight(int height) { if (m_height == height) return; m_height = height; m_self_matrix_dirty = true; }
	virtual int GetHeight() { return m_height; }

	// 设置旋转
	virtual void SetRotate(float rotate) { m_rotate = rotate; m_self_matrix_dirty = true; }

	// 设置缩放
	virtual void SetScaleX(float x) { m_scale_x = x; m_self_matrix_dirty = true; }
	virtual void SetScaleY(float y) { m_scale_y = y; m_self_matrix_dirty = true; }

	// 设置锚点
	virtual void SetCenterX(int x) { if (m_center_x == x) return; m_center_x = x; m_self_matrix_dirty = true; }
	virtual void SetCenterY(int y) { if (m_center_y == y) return; m_center_y = y; m_self_matrix_dirty = true; }

	// 设置颜色
	virtual void SetRed(float red) { m_red = red; m_color_dirty = true; }
	virtual void SetGreen(float green) { m_green = green; m_color_dirty = true; }
	virtual void SetBlue(float blue) { m_blue = blue; m_color_dirty = true; }
	virtual void SetAlpha(float alpha) { m_alpha = alpha; m_color_dirty = true; }

public:
	// 设置本地坐标为脏
	virtual void SetSelfMatrixDirty() { m_self_matrix_dirty = true; }

	// 更新本地矩阵
	virtual bool UpdateSelfMatrix2D()
	{
		// 如果本地矩阵没有脏，那么直接返回false
		if (!m_self_matrix_dirty) return false;
		// 标记为不脏
		m_self_matrix_dirty = false;
		// 顶点标脏
		m_vertex_dirty = true;

		// 设置锚点
		m_self_matrix.SetTranslation(static_cast<float>(-m_center_x), static_cast<float>(-m_center_y));
		// 设置缩放
		m_self_matrix.Scale(m_scale_x, m_scale_y);
		// 设置旋转
		m_self_matrix.Rotate(m_rotate);
		// 设置坐标
		m_self_matrix.Translation(static_cast<float>(m_pos_x), static_cast<float>(m_pos_y));

		return true;
	}

	// 更新世界矩阵
	virtual bool UpdateGlobalMatrix2D(const CarpMatrix2D& parent, bool parent_changed)
	{
		const bool self_update = UpdateSelfMatrix2D();

		// 本地矩阵需要更新或者父节点变化，那么就要更新世界矩阵
		if (parent_changed || self_update)
		{
			// 复制本地矩阵
			m_global_matrix = m_self_matrix;
			// 乘以父节点矩阵
			m_global_matrix.Multiply(parent);
			// 顶点坐标标脏
			m_vertex_dirty = true;
			return true;
		}

		return false;
	}

	// 更新纹理坐标
	virtual void UpdateTextureCoordinate() {}
	// 更新顶点坐标
	virtual void UpdateVertexCoordinate() {}
	// 更新颜色
	virtual void UpdateVertexColor() {}

protected:
	bool m_visible = true;				// 是否显示
	bool m_clip = false;				// 是否裁剪

protected:
	int m_pos_x = 0, m_pos_y = 0;								// 坐标
	float m_scale_x = 1.0f, m_scale_y = 1.0f;					// 缩放
	float m_red = 1.0f;											// 颜色
	float m_green = 1.0f;
	float m_blue = 1.0f;
	float m_alpha = 1.0f;
	int m_width = 0, m_height = 0;								// 宽高
	int m_center_x = 0, m_center_y = 0;							// 锚点
	float m_rotate = 0.0f;										// 旋转弧度

protected:
	CarpMatrix2D m_self_matrix;			// 保存本地矩阵
	bool m_self_matrix_dirty = true;	// 标脏本地坐标矩阵
	CarpMatrix2D m_global_matrix;		// 保存世界矩阵
	bool m_vertex_dirty = true;			// 标脏顶点坐标
	bool m_texture_dirty = true;		// 标脏纹理
	bool m_color_dirty = true;			// 标脏颜色

public:
	Sail2DObjectWeakPtr m_front_node;	// 前一个兄弟节点
	Sail2DObjectPtr m_back_node;		// 后一个兄弟节点
	Sail2DObjectWeakPtr m_parent_node;	// 父节点
};

#endif


