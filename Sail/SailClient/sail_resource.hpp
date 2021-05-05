#ifndef SAIL_RESOURCE_INCLUDED
#define SAIL_RESOURCE_INCLUDED

#include "sokol/sokol_fetch.h"

class SailResource;
extern SailResource s_sail_resource;

class SailResource
{
public:
    void Setup()
    {
        sfetch_desc_t desc{};
        sfetch_setup(desc);
    }

    void Shutdown()
    {
        sfetch_shutdown();
    }

    void Frame()
    {
        sfetch_dowork();
    }

public:
    struct LoadInfo
    {
        char buffer[1024 * 64]{};
        std::vector<char> memory;
        std::vector<std::function<void(void*, size_t)>> callback_list;
    };

    void Load(const std::string& path, std::function<void(void*, size_t)> callback)
    {
        auto it = m_callback_map.find(path);
        if (it == m_callback_map.end())
        {
            auto& info = m_callback_map[path];

            sfetch_request_t request{};
            request.path = path.c_str();
            request.callback = HandleLoad;
            request.buffer_ptr = info.buffer;
            request.buffer_size = sizeof(info.buffer);
            request.chunk_size = sizeof(info.buffer);
            const auto handle = sfetch_send(request);
            if (handle.id == 0)
            {
                m_callback_map.erase(path);
                if (callback) callback(nullptr, 0);
                return;
            }

            info.callback_list.emplace_back(callback);
        }
        else
        {
            it->second.callback_list.emplace_back(callback);
        }
    }

private:
    static void HandleLoad(const sfetch_response_t* response)
    {
        if (response->fetched)
        {
            const std::string path = response->path;

            const auto it = s_sail_resource.m_callback_map.find(path);
            if (it == s_sail_resource.m_callback_map.end()) return;

            auto& info = it->second;
            for (uint32_t i = 0; i < response->fetched_size; ++i)
                info.memory.push_back(it->second.buffer[i]);
        }

        if (response->finished)
        {
            const auto it = s_sail_resource.m_callback_map.find(response->path);
            if (it == s_sail_resource.m_callback_map.end()) return;

            LoadInfo info;
            info.memory = std::move(it->second.memory);
            info.callback_list = std::move(it->second.callback_list);
            s_sail_resource.m_callback_map.erase(it);

            if (response->failed)
            {
                for (auto& callback : info.callback_list)
                {
                    if (callback) callback(nullptr, 0);
                }
            }
            else
            {
                for (auto& callback : info.callback_list)
                {
                    if (callback) callback(info.memory.data(), info.memory.size());
                }
            }
        }
    }

private:
    std::unordered_map<std::string, LoadInfo> m_callback_map;
};

#endif

#ifdef SAIL_RESOURCE_IMPL
#ifndef SAIL_RESOURCE_IMPL_INCLUDE
#define SAIL_RESOURCE_IMPL_INCLUDE
SailResource s_sail_resource;
#endif
#endif