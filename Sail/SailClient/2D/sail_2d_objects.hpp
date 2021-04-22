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
	// ����ӿؼ�
	virtual void AddChild(const Sail2DObjectPtr& object)
	{
		// ����ǿ�ָ�룬��ô������
		if (!object) return;

		// object�������
		object->SetSelfMatrixDirty();

		// ��ǰ�ڵ�ҲҪ����
		SetSelfMatrixDirty();

		// ���û�нڵ㣬��ô������ͷβ����object
		if (!m_head_node || !m_tail_node)
		{
			m_head_node = object;
			m_tail_node = object;
			return;
		}

		// β����back��object
		m_tail_node->m_back_node = object;
		// object���ײ�����
		object->m_front_node = m_tail_node;
		// β������Ϊobject
		m_tail_node = object;
	}
	// ��ĳ���ڵ�������
	virtual void AddChildAfter(const Sail2DObjectPtr& front_object, const Sail2DObjectPtr& target_object)
	{
		// ����ǿ�ָ�룬��ô������
		if (!target_object || !front_object) return;
		// ����ڵ�һ�£���ô������
		if (target_object == front_object) return;
		// �жϸ��ڵ�
		const auto parent = front_object->m_parent_node.lock();
		if (!parent || parent.get() != this) return;

		// object�������
		target_object->SetSelfMatrixDirty();

		// ��ǰ�ڵ�ҲҪ����
		SetSelfMatrixDirty();

		// ���û�нڵ㣬��ô������ͷβ����object
		if (!m_head_node || !m_tail_node)
		{
			m_head_node = target_object;
			m_tail_node = target_object;
			return;
		}

		// ����back�ڵ�
		Sail2DObjectPtr back_node = front_object->m_back_node;

		// �ѽڵ�ҽ�ȥ
		front_object->m_back_node = target_object;
		target_object->m_front_node = front_object;
		target_object->m_back_node = back_node;
		if (back_node) back_node->m_front_node = target_object;

		// ���tail��front�ڵ㣬��ô������Ϊtarget
		if (m_tail_node == front_object) m_tail_node = target_object;
	}
	// ��ĳ���ڵ�ǰ�����
	virtual void AddChildBefore(const Sail2DObjectPtr& back_object, const Sail2DObjectPtr& target_object)
	{
		// ����ǿ�ָ�룬��ô������
		if (!target_object || !back_object) return;
		// ����ڵ�һ�£���ô������
		if (target_object == back_object) return;
		// �жϸ��ڵ�
		const auto parent = back_object->m_parent_node.lock();
		if (!parent || parent.get() != this) return;

		// object�������
		target_object->SetSelfMatrixDirty();

		// ��ǰ�ڵ�ҲҪ����
		SetSelfMatrixDirty();

		// ���û�нڵ㣬��ô������ͷβ����object
		if (!m_head_node || !m_tail_node)
		{
			m_head_node = target_object;
			m_tail_node = target_object;
			return;
		}

		// ����front�ڵ�
		Sail2DObjectPtr front_node = back_object->m_front_node.lock();

		// �ѽڵ�ҽ�ȥ
		back_object->m_front_node = target_object;
		target_object->m_back_node = back_object;
		target_object->m_front_node = front_node;
		if (front_node) front_node->m_back_node = target_object;

		// ���back��ͷ����ô������Ϊtarget
		if (m_head_node == back_object) m_head_node = target_object;
	}
	// �Ƴ��ӿؼ�
	virtual void RemoveChild(const Sail2DObjectPtr& object)
	{
		if (!object) return;
		// �жϸ��ڵ�
		const auto parent = object->m_parent_node.lock();
		if (!parent || parent.get() != this) return;

		// object�������
		object->SetSelfMatrixDirty();

		// ��ǰ�ڵ�ҲҪ����
		SetSelfMatrixDirty();

		// ȡ��front��back��Ȼ��ɾ������
		Sail2DObjectPtr front_node = object->m_front_node.lock();
		object->m_front_node.reset();
		Sail2DObjectPtr back_node = object->m_back_node;
		object->m_back_node = nullptr;

		// Ȼ���ǰ��ڵ�������
		if (front_node)
			front_node->m_back_node = back_node;
		if (back_node)
			back_node->m_front_node = front_node;

		// �ж�ͷ
		if (object == m_head_node)
			m_head_node = back_node;

		// �ж�β
		if (object == m_tail_node)
			m_tail_node = front_node;
	}
	// �Ƴ����пռ�
	virtual void RemoveAllChild()
	{
		// ��ǰ�ڵ�Ҳ����
		SetSelfMatrixDirty();

		// ѭ��ɾ���ڵ�
		Sail2DObjectPtr node = m_head_node;
		while (node)
		{
			node->SetSelfMatrixDirty();
			const Sail2DObjectPtr next = node->m_back_node;
			node->m_front_node.reset();
			node->m_back_node = nullptr;
			node = next;
		}

		// �ÿ�
		m_head_node = nullptr;
		m_tail_node = nullptr;
	}

public:
	void Render(const CarpMatrix2D& parent, bool parent_changed) override
	{
		// ���˵��ü�
		if (m_clip) return;
		// ���˵�����ʾ
		if (!m_visible) return;
		// ���˵�û���ӿؼ�
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


