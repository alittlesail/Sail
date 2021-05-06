#ifndef SAIL_GFX_INCLUDED
#define SAIL_GFX_INCLUDED

#if defined(__APPLE__)
#define SOKOL_METAL
#elif defined(__EMSCRIPTEN__)
#define SOKOL_GLES2
#elif defined(_WIN32)
#define SOKOL_D3D11
#elif defined(__ANDROID__)
#define SOKOL_GLES2
#elif defined(__linux__) || defined(__unix__)
#define SOKOL_GLCORE33
#else
#error "sokol_app.h: Unknown platform"
#endif

#include "sokol/sokol_gfx.h"
#include <vector>

class SailGfx2DBatchRender
{
public:
    enum Gfx2DBatchInfo
    {
        X_0 = 0, Y_0, Z_0,
        R_0, G_0, B_0, A_0,
        U_0, V_0,

        X_1, Y_1, Z_1,
        R_1, G_1, B_1, A_1,
        U_1, V_1,

        X_2, Y_2, Z_2,
        R_2, G_2, B_2, A_2,
        U_2, V_2,

        VERTEX_ALL_FLOAT_COUNT,
        VERTEX_ONE_FLOAT_COUNT = X_1,

        VERTEX_POSITION_OFFSET = X_0,
        VERTEX_COLOR_OFFSET = R_0,
        VERTEX_TEXCOORD_OFFSET = U_0,
    };

    struct Gfx2DBatchCmd
    {
        sg_image texture{ SG_INVALID_ID };
        int vertex_count = 0;
    };

public:
    ~SailGfx2DBatchRender()
    {
        Clear();
    }

    void Clear()
    {
        m_init = false;
        free(m_vertices);
        m_vertices = nullptr;
        m_size = 0;
        m_capacity = 0;

        if (m_quad_texture.id != SG_INVALID_ID)
        {
            sg_destroy_image(m_quad_texture);
            m_quad_texture.id = SG_INVALID_ID;
        }

        if (m_buffer.id != SG_INVALID_ID)
        {
            sg_destroy_buffer(m_buffer);
            m_buffer.id = SG_INVALID_ID;
        }
        m_buffer_size = 0;

        if (m_shader.id != SG_INVALID_ID)
        {
            sg_destroy_shader(m_shader);
            m_shader.id = SG_INVALID_ID;
        }

        if (m_pipeline.id != SG_INVALID_ID)
        {
            sg_destroy_pipeline(m_pipeline);
            m_pipeline.id = SG_INVALID_ID;
        }
        
        m_cmd_list.resize(0);
    }

    const sg_image& GetQuadTexture()
    {
        if (m_quad_texture.id != SG_INVALID_ID) return m_quad_texture;

        m_quad_texture = sg_alloc_image();

        uint32_t pixels[32 * 32] = {0};
        for (auto& pixel : pixels)
            pixel = 0xFFFFFFFF;

        sg_image_desc desc{};
        desc.width = 32;
        desc.height = 32;
        desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        desc.data.subimage[0][0].ptr = pixels;
        desc.data.subimage[0][0].size = sizeof(pixels);
        sg_init_image(m_quad_texture, desc);

        return m_quad_texture;
    }

public:
    void Push(const sg_image& texture, float vertices[VERTEX_ALL_FLOAT_COUNT])
    {
        if (m_cmd_list.empty())
        {
            Gfx2DBatchCmd cmd;
            cmd.texture = texture;
            cmd.vertex_count = VERTEX_ALL_FLOAT_COUNT;
            m_cmd_list.emplace_back(cmd);
        }
        else
        {
            auto& back = m_cmd_list.back();
            if (back.texture.id == texture.id)
            {
                m_cmd_list.back().vertex_count += VERTEX_ALL_FLOAT_COUNT;
            }
            else
            {
                Gfx2DBatchCmd cmd;
                cmd.texture = texture;
                cmd.vertex_count = VERTEX_ALL_FLOAT_COUNT;
                m_cmd_list.emplace_back(cmd);
            }
        }

        AddSize(VERTEX_ALL_FLOAT_COUNT);
        memcpy(m_vertices + m_size, vertices, VERTEX_ALL_FLOAT_COUNT * sizeof(float));
        m_size += VERTEX_ALL_FLOAT_COUNT;
    }

    void Flush()
    {
        Init();

        if (m_size == 0) return;

        if (m_buffer.id != SG_INVALID_ID && m_buffer_size < m_size)
        {
            sg_destroy_buffer(m_buffer);
            m_buffer.id = SG_INVALID_ID;
        }
        
        if (m_buffer.id == SG_INVALID_ID)
        {
            sg_buffer_desc desc{};
            desc.size = m_size * sizeof(float);
            desc.usage = SG_USAGE_STREAM;
            m_buffer = sg_make_buffer(desc);
        }

        sg_range range{};
        range.ptr = m_vertices;
        range.size = m_size * sizeof(float);
        sg_update_buffer(m_buffer, range);

        m_buffer_size = m_size;

        int vertex_offset = 0;
        for (auto& cmd : m_cmd_list)
        {
            sg_bindings bind{};
            bind.vertex_buffers[0] = m_buffer;
            bind.vertex_buffer_offsets[0] = vertex_offset * sizeof(float);
            bind.fs_images[0] = cmd.texture;

            vertex_offset += cmd.vertex_count;

            sg_apply_pipeline(m_pipeline);
            sg_apply_bindings(&bind);
            sg_draw(0, cmd.vertex_count / static_cast<int>(VERTEX_ONE_FLOAT_COUNT), 1);
        }
        
        m_cmd_list.resize(0);
        m_size = 0;
    }

private:
    void Init()
    {
        if (m_init) return;

        {
            sg_shader_desc desc{};
            desc.attrs[0].sem_name = "POSITION";
            desc.attrs[1].sem_name = "COLOR";
            desc.attrs[2].sem_name = "TEXCOORD";
            desc.vs.source =
                "struct vs_in {\n"
                "  float4 pos: POSITION;\n"
                "  float4 color: COLOR0;\n"
                "  float2 uv: TEXCOORD0;\n"
                "};\n"
                "struct vs_out {\n"
                "  float4 color: COLOR0;\n"
                "  float2 uv: TEXCOORD0;\n"
                "  float4 pos: SV_Position;\n"
                    "};\n"
                "vs_out main(vs_in inp) {\n"
                "  vs_out outp;\n"
                "  outp.pos = inp.pos;\n"
                "  outp.color = inp.color;\n"
                "  outp.uv = inp.uv;\n"
                "  return outp;\n"
                "}\n";
            desc.fs.images[0].image_type = SG_IMAGETYPE_2D,
            desc.fs.source =
                "Texture2D<float4> tex: register(t0);\n"
                "sampler smp: register(s0);\n"
                "float4 main(float4 color: COLOR0, float2 uv: TEXCOORD0): SV_Target0 {\n"
                "  return tex.Sample(smp, uv) * color;\n"
                "}\n";
            m_shader = sg_make_shader(desc);
        }

        {
            sg_pipeline_desc desc{};
            desc.shader = m_shader;
            desc.layout.attrs[0].offset = static_cast<int>(VERTEX_POSITION_OFFSET) * static_cast<int>(sizeof(float));
            desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
            desc.layout.attrs[1].offset = static_cast<int>(VERTEX_COLOR_OFFSET) * static_cast<int>(sizeof(float));
            desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4;
            desc.layout.attrs[2].offset = static_cast<int>(VERTEX_TEXCOORD_OFFSET) * static_cast<int>(sizeof(float));
            desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT2;
            m_pipeline = sg_make_pipeline(desc);
        }

        m_init = true;
    }

private:
    inline void AddSize(size_t count)
    {
        const size_t size = m_size + count;
        if (m_capacity >= size) return;

        size_t capacity = m_capacity * 2;
        if (capacity < size) capacity = size;

        auto* vertices = static_cast<float*>(malloc(capacity * sizeof(float)));
        if (m_vertices != nullptr)
        {
            memcpy(vertices, m_vertices, m_size * sizeof(float));
            free(m_vertices);
        }

        m_vertices = vertices;
        m_capacity = capacity;
    }

private:
    float* m_vertices = nullptr; // 当前所有顶点
    size_t m_size = 0;
    size_t m_capacity = 0;

    std::vector<Gfx2DBatchCmd> m_cmd_list;

private:
    bool m_init = false;
    sg_pipeline m_pipeline{ SG_INVALID_ID };
    sg_buffer m_buffer{ SG_INVALID_ID };
    size_t m_buffer_size = 0;
    sg_shader m_shader{ SG_INVALID_ID };

private:
    sg_image m_quad_texture{ SG_INVALID_ID };
};

extern SailGfx2DBatchRender s_sail_gfx_2d_batch_render;

#endif


#ifdef SAIL_GFX_IMPL
#ifndef SAIL_GFX_IMPL_INCLUDE
#define SAIL_GFX_IMPL_INCLUDE
SailGfx2DBatchRender s_sail_gfx_2d_batch_render;
#endif
#endif

