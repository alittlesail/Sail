#ifndef SAIL_2D_OBJECTS_INCLUDED
#define SAIL_2D_OBJECTS_INCLUDED

#include <algorithm>
#include <vector>

#include "sail_2d_object.hpp"

class Sail2DObjects : public Sail2DObject
{
public:
	virtual ~Sail2DObjects() { }

public:
	// 添加子控件
	virtual void AddChild(const Sail2DObjectPtr& object)
	{
		// 如果是空指针，那么就跳过
		if (!object) return;

		// object本身标脏
		object->SetSelfMatrixDirty();

		// 当前节点也要标脏
		SetSelfMatrixDirty();

		// 如果没有节点，那么就设置头尾都是object
		if (!m_head_node || !m_tail_node)
		{
			m_head_node = object;
			m_tail_node = object;
			return;
		}

		// 尾部的back是object
		m_tail_node->m_back_node = object;
		// object的首部设置
		object->m_front_node = m_tail_node;
		// 尾部设置为object
		m_tail_node = object;
	}
	// 往某个节点后面添加
	virtual void AddChildAfter(const Sail2DObjectPtr& front_object, const Sail2DObjectPtr& target_object)
	{
		// 如果是空指针，那么就跳过
		if (!target_object || !front_object) return;
		// 如果节点一致，那么就跳过
		if (target_object == front_object) return;
		// 判断父节点
		const auto parent = front_object->m_parent_node.lock();
		if (!parent || parent.get() != this) return;

		// object本身标脏
		target_object->SetSelfMatrixDirty();

		// 当前节点也要标脏
		SetSelfMatrixDirty();

		// 如果没有节点，那么就设置头尾都是object
		if (!m_head_node || !m_tail_node)
		{
			m_head_node = target_object;
			m_tail_node = target_object;
			return;
		}

		// 保存back节点
		Sail2DObjectPtr back_node = front_object->m_back_node;

		// 把节点挂进去
		front_object->m_back_node = target_object;
		target_object->m_front_node = front_object;
		target_object->m_back_node = back_node;
		if (back_node) back_node->m_front_node = target_object;

		// 如果tail是front节点，那么就设置为target
		if (m_tail_node == front_object) m_tail_node = target_object;
	}
	// 往某个节点前面添加
	virtual void AddChildBefore(const Sail2DObjectPtr& back_object, const Sail2DObjectPtr& target_object)
	{
		// 如果是空指针，那么就跳过
		if (!target_object || !back_object) return;
		// 如果节点一致，那么就跳过
		if (target_object == back_object) return;
		// 判断父节点
		const auto parent = back_object->m_parent_node.lock();
		if (!parent || parent.get() != this) return;

		// object本身标脏
		target_object->SetSelfMatrixDirty();

		// 当前节点也要标脏
		SetSelfMatrixDirty();

		// 如果没有节点，那么就设置头尾都是object
		if (!m_head_node || !m_tail_node)
		{
			m_head_node = target_object;
			m_tail_node = target_object;
			return;
		}

		// 保存front节点
		Sail2DObjectPtr front_node = back_object->m_front_node.lock();

		// 把节点挂进去
		back_object->m_front_node = target_object;
		target_object->m_back_node = back_object;
		target_object->m_front_node = front_node;
		if (front_node) front_node->m_back_node = target_object;

		// 如果back是头，那么就设置为target
		if (m_head_node == back_object) m_head_node = target_object;
	}
	// 移除子控件
	virtual void RemoveChild(const Sail2DObjectPtr& object)
	{
		if (!object) return;
		// 判断父节点
		const auto parent = object->m_parent_node.lock();
		if (!parent || parent.get() != this) return;

		// object本身标脏
		object->SetSelfMatrixDirty();

		// 当前节点也要标脏
		SetSelfMatrixDirty();

		// 取出front和back，然后删除关联
		Sail2DObjectPtr front_node = object->m_front_node.lock();
		object->m_front_node.reset();
		Sail2DObjectPtr back_node = object->m_back_node;
		object->m_back_node = nullptr;

		// 然后把前后节点连起来
		if (front_node)
			front_node->m_back_node = back_node;
		if (back_node)
			back_node->m_front_node = front_node;

		// 判断头
		if (object == m_head_node)
			m_head_node = back_node;

		// 判断尾
		if (object == m_tail_node)
			m_tail_node = front_node;
	}
	// 移除所有空间
	virtual void RemoveAllChild()
	{
		// 当前节点也标脏
		SetSelfMatrixDirty();

		// 循环删除节点
		Sail2DObjectPtr node = m_head_node;
		while (node)
		{
			node->SetSelfMatrixDirty();
			const Sail2DObjectPtr next = node->m_back_node;
			node->m_front_node.reset();
			node->m_back_node = nullptr;
			node = next;
		}

		// 置空
		m_head_node = nullptr;
		m_tail_node = nullptr;
	}

public:
	void Render(const CarpMatrix2D& parent, bool parent_changed) override
	{
		// 过滤掉裁剪
		if (m_clip) return;
		// 过滤掉不显示
		if (!m_visible) return;
		// 过滤掉没有子控件
		if (!m_head_node) return;

		const bool global_changed = UpdateGlobalMatrix2D(parent, parent_changed);
		UpdateVertexCoordinate();
		UpdateTextureCoordinate();
		RenderImpl(global_changed);
	}

protected:
	void RenderImpl(bool global_changed)
	{
		Sail2DObjectPtr node = m_head_node;
		while (node)
		{
			node->Render(m_global_matrix, global_changed);
			node = node->m_back_node;
		}
	}

protected:
	Sail2DObjectPtr m_head_node = nullptr;
	Sail2DObjectPtr m_tail_node = nullptr;
};

using Sail2DObjectsPtr = std::shared_ptr<Sail2DObjects>;
using Sail2DObjectsWeakPtr = std::weak_ptr<Sail2DObjects>;

#endif


