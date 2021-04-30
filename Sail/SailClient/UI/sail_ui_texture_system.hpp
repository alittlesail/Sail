#ifndef SAIL_UI_TEXTURE_SYSTEM_INCLUDED
#define SAIL_UI_TEXTURE_SYSTEM_INCLUDED

#include <memory>
#include "sokol/sokol_gfx.h"
#include "sail_ui_object.hpp"
#include "sail_ui_texture.hpp"
#include "sail_resource.hpp"
#include "Carp/carp_message.hpp"
#include "stb/stb_image.h"

// 路由机制上的消息包，用于写
CARP_MESSAGE_MACRO(SailUITexturePiece, 0, float, top, float, bottom, float, left, float, right);
using SailUITexturePieceMap = std::map<std::string, SailUITexturePiece>;
CARP_MESSAGE_MACRO(SailUITextureAtlas, 0, SailUITexturePieceMap, map, int, width, int, height);
using SailUITextureAtlasMap = std::map<std::string, SailUITextureAtlas>;
CARP_MESSAGE_MACRO(SailUITextureAtlasFile, 0, SailUITextureAtlasMap, map);

class SailUITextureSystem
{
public:
    struct PieceInfo
    {
        // 字图片的范围
        SailUITexturePiece piece;
        // 主图片的路径
        std::string path;
    };

    void AddAtlas(const SailUITextureAtlasFile& file)
    {
        for (const auto& pair : file.map)
        {
            for (const auto& sub_pair : pair.second.map)
            {
                auto& info = m_piece_map[sub_pair.first];
                info.piece = sub_pair.second;
                info.path = pair.first;
            }
        }
    }

    void ClearAtlas()
    {
        m_piece_map.clear();
    }

    void Shutdown()
    {
        m_texture_map.clear();
    }

public:
    void SetTexture(const SailUIObjectPtr& object, const std::string& texture_path)
    {
        PieceInfo info;

        {
            const auto it = m_piece_map.find(texture_path);
            if (it != m_piece_map.end())
            {
                info = it->second;
            }
            else
            {
                info.path = texture_path;
                info.piece.right = 1.0f;
                info.piece.bottom = 1.0f;
            }
        }

        {
            const auto it = m_texture_map.find(info.path);
            if (it != m_texture_map.end())
            {
                object->SetTexture(it->second);
                object->SetTextureCoordinate(info.piece.top, info.piece.bottom, info.piece.left, info.piece.right);
            }
            else
            {
                auto& vector = m_texture_callback[info.path];

                SailUIObjectWeakPtr weak_object = object;
                vector.emplace_back([weak_object, info](const SailUITexturePtr& texture)
                {
                    if (!texture) return;
                    auto object = weak_object.lock();
                    if (!object) return;

                    object->SetTexture(texture);
                    object->SetTextureCoordinate(info.piece.top, info.piece.bottom, info.piece.left, info.piece.right);
                });

                // 开始加载图片
                s_sail_resource.Load(info.path, [path=info.path, this](void* memory, size_t size)
                {
                    const auto it = m_texture_callback.find(path);
                    if (it == m_texture_callback.end()) return;

                    std::vector<std::function<void(const SailUITexturePtr&)>> callback_list = std::move(it->second);
                    m_texture_callback.erase(it);

                    int png_width = 0, png_height = 0, num_channels = 0;
                    const int desired_channels = 4;

                    stbi_uc* pixels = nullptr;
                    SailUITexturePtr texture;
                    do
                    {
                        if (memory == nullptr) break;

                        pixels = stbi_load_from_memory(static_cast<unsigned char*>(memory), static_cast<int>(size)
                            , &png_width, &png_height, &num_channels, desired_channels);

                        if (pixels == nullptr) break;

                        auto image = sg_alloc_image();
                        if (image.id == SG_INVALID_ID) break;

                        sg_image_desc desc{};
                        desc.width = png_width;
                        desc.height = png_height;
                        desc.pixel_format = SG_PIXELFORMAT_RGBA8;
                        desc.min_filter = SG_FILTER_LINEAR;
                        desc.mag_filter = SG_FILTER_LINEAR;
                        desc.data.subimage[0][0].ptr = pixels;
                        desc.data.subimage[0][0].size = png_width * png_height * desired_channels;
                        sg_init_image(image, desc);
                        texture = std::make_shared<SailUITexture>(png_width, png_height, image);

                    } while (false);

                    if (pixels) stbi_image_free(pixels);
                    
                    for (auto& callback : callback_list)
                    {
                        if (callback) callback(texture);
                    }
                });
            }
        }
    }

private:
    // 子图片的名字映射大图片
    std::unordered_map<std::string, std::vector<std::function<void(const SailUITexturePtr&)>>> m_texture_callback;
    std::unordered_map<std::string, SailUITexturePtr> m_texture_map;

    // 子图片的名字映射大图片
    std::unordered_map<std::string, PieceInfo> m_piece_map;
};

extern SailUITextureSystem s_sail_ui_texture_system;

#endif

#ifdef SAIL_UI_TEXTURE_SYSTEM_IMPL
#ifndef SAIL_UI_TEXTURE_SYSTEM_IMPL_INCLUDE
#define SAIL_UI_TEXTURE_SYSTEM_IMPL_INCLUDE
SailUITextureSystem s_sail_ui_texture_system;
#endif
#endif