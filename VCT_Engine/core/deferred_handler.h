#pragma once
#include "scene\texture.h"
#include "types\transform_matrices.h"
#include "scene\material.h"

class DeferredProgram
{
    public:
        void ExtractActiveUniforms();

        void SetUniform(TransformMatrices::TransformMatrixId mId,
                        const glm::mat4x4 &val) const;
        void SetUniform(RawTexture::TextureType tId, const int val) const;
        void SetUniform(Material::MaterialFloat3PropertyId mF3Id,
                        const glm::vec3 &val) const;
        void SetUniform(Material::MaterialFloat1PropertyId mF1Id,
                        const float val)const;

        const std::vector<RawTexture::TextureType>
        &ActiveSamplers() const { return aSamplers; };

        const std::vector<TransformMatrices::TransformMatrixId>
        &ActiveTransformMatrices() const { return aMatrices; };

        const std::vector<Material::MaterialFloat3PropertyId>
        &ActiveMaterialFloat3Properties() const { return aF3Material; };

        const std::vector<Material::MaterialFloat1PropertyId>
        &ActiveMaterialFloat1Properties() const { return aF1Material; };
    private:
        oglplus::FragmentShader sFragment;
        oglplus::VertexShader sVertex;
        oglplus::Program program;

        friend class DeferredHandler;

        std::array<std::unique_ptr<oglplus::UniformSampler>,
            RawTexture::TEXTURE_TYPE_MAX> samplers;

        std::array<std::unique_ptr<oglplus::Uniform<glm::mat4x4>>,
            TransformMatrices::TRANSFORM_MATRIX_ID_MAX >matrices;

        std::array<std::unique_ptr<oglplus::Uniform<glm::vec3>>,
            Material::MATERIAL_FLOAT3_PROPERTY_ID_MAX> materialFloat3;
        std::array<std::unique_ptr<oglplus::Uniform<float>>,
            Material::MATERIAL_FLOAT1_PROPERTY_ID_MAX> materialFloat1;

        std::vector<RawTexture::TextureType> aSamplers;
        std::vector<TransformMatrices::TransformMatrixId> aMatrices;
        std::vector<Material::MaterialFloat3PropertyId> aF3Material;
        std::vector<Material::MaterialFloat1PropertyId> aF1Material;
};

class DeferredHandler
{
    public:
        enum GBufferTextureType
        {
            Position,
            Normal,
            Albedo,
            Specular,
            GBUFFER_TEXTURE_TYPE_MAX
        };
        // deferred shader programs handlers
        DeferredProgram geometryPass;
        DeferredProgram lightPass;
    private:
        static oglplus::Context gl;
        // geometry buffer
        oglplus::Framebuffer geomBuffer;
        oglplus::Texture bDepthTexture;
        std::vector<oglplus::Context::ColorBuffer> openedColorBuffers;
        std::array<oglplus::Texture, GBUFFER_TEXTURE_TYPE_MAX> bTextures;
        // full screen quad
        oglplus::VertexArray fsQuadVertexArray;
        oglplus::Buffer fsQuadVertexBuffer;
        oglplus::Buffer fsQuadElementBuffer;

        void LoadShaders();
        void InitializeGBuffer(unsigned int windowWith, unsigned int windowHeight);
        void CreateFullscreenQuad();
    public:
        DeferredHandler(unsigned int windowWith, unsigned int windowHeight);

        virtual ~DeferredHandler();

        void UseGeometryPass() const { geometryPass.program.Use(); }
        void UseLightPass() const { lightPass.program.Use(); }

        void BindGBuffer(const oglplus::FramebufferTarget &bindingMode);
        void ReadGBuffer(const GBufferTextureType &gBufferTexType);
        // returns texture handlers to gbuffer color buffers
        const std::array<oglplus::Texture, GBUFFER_TEXTURE_TYPE_MAX>
        &GetGBufferTextures() const { return bTextures; }
};
