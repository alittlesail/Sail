#ifndef SAIL_RESOURCE_INCLUDED
#define SAIL_RESOURCE_INCLUDED

#include "sokol/sokol_fetch.h"

#include <set>
#include <vector>

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
    struct ProgressInfo
    {
        size_t total_size = 0;  //  如果为0，那么就表示没有下载完成之前不知道大小
        bool finished = false;    // 是否已经完成
        bool failed = false;    // 是否失败

        char buffer[10240 * 64]{};
        std::vector<char> memory;
    };

    // progress(是否全部完成, 正在加载的文件列表)
    void Progress(const std::set<std::string>& path_set, std::function<void(bool, std::unordered_map<std::string, ProgressInfo>&)> progress)
    {
        if (!m_progress_map.empty())
        {
            CARP_ERROR("already in prepare!");

            std::unordered_map<std::string, ProgressInfo> info_map;
            for (const auto& path : path_set)
            {
                auto& info = info_map[path];
                info.failed = true;
                info.finished = true;
            }
            if (progress) progress(true, info_map);
            return;
        }

        m_finished_count = 0;
        m_progress_map.clear();
        m_progress = progress;

        for (const auto& path : path_set)
        {
            auto& info = m_progress_map[path];

            sfetch_request_t request{};
            request.path = path.c_str();
            request.callback = HandleProgress;
            request.buffer_ptr = info.buffer;
            request.buffer_size = sizeof(info.buffer);
            request.chunk_size = sizeof(info.buffer);
            const auto handle = sfetch_send(request);

#ifndef __EMSCRIPTEN__
            CarpFile::PathAttribute attr;
            if (CarpFile::GetPathAttribute(request.path, attr))
            {
                info.total_size = attr.size;
                info.memory.reserve(info.total_size);
            }
#endif

            if (handle.id == 0)
            {
                auto info_map = std::move(m_progress_map);
                m_progress_map.clear();
                if (progress) progress(true, info_map);
                return;
            }
        }
    }

private:
    static void HandleProgress(const sfetch_response_t* response)
    {
        if (response->fetched)
        {
            const std::string path = response->path;

            const auto it = s_sail_resource.m_progress_map.find(path);
            if (it == s_sail_resource.m_progress_map.end()) return;

            auto& info = it->second;
            const auto old_size = info.memory.size();
            info.memory.resize(info.memory.size() + response->fetched_size);
            memcpy(info.memory.data() + old_size, it->second.buffer, response->fetched_size);
            if (s_sail_resource.m_progress) s_sail_resource.m_progress(false, s_sail_resource.m_progress_map);
        }

        if (response->finished)
        {
            ++s_sail_resource.m_finished_count;
            const auto it = s_sail_resource.m_progress_map.find(response->path);
            if (it == s_sail_resource.m_progress_map.end()) return;

            it->second.finished = true;
            it->second.failed = response->failed;

            if (s_sail_resource.m_finished_count >= s_sail_resource.m_progress_map.size())
            {
                auto info_map = std::move(s_sail_resource.m_progress_map);
                s_sail_resource.m_progress_map.clear();
                if (s_sail_resource.m_progress)
                    s_sail_resource.m_progress(true, info_map);
            }
            else
            {
                if (s_sail_resource.m_progress)
                    s_sail_resource.m_progress(false, s_sail_resource.m_progress_map);
            }
        }
    }

public:
    struct LoadInfo
    {
        char buffer[10240 * 64]{};
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
            const auto old_size = info.memory.size();
            info.memory.resize(info.memory.size() + response->fetched_size);
            memcpy(info.memory.data() + old_size, it->second.buffer, response->fetched_size);
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

private:
    std::function<void(bool, std::unordered_map<std::string, ProgressInfo>&)> m_progress;
    size_t m_finished_count = 0;
    std::unordered_map<std::string, ProgressInfo> m_progress_map;
};

#endif

#ifdef SAIL_RESOURCE_IMPL
#ifndef SAIL_RESOURCE_IMPL_INCLUDE
#define SAIL_RESOURCE_IMPL_INCLUDE
SailResource s_sail_resource;
#endif
#endif