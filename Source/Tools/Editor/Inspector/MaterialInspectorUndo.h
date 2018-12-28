//
// Copyright (c) 2018 Rokas Kupstys
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once


#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Toolbox/Common/UndoManager.h>


namespace Urho3D
{

namespace Undo
{

/// Tracks addition, removal and modification of techniques in material
class TechniqueChangedAction
        : public EditAction
{
public:
    struct TechniqueInfo
    {
        String techniqueName_;
        MaterialQuality qualityLevel_;
        float lodDistance_;
    };

    TechniqueChangedAction(const Material* material, unsigned index, const TechniqueEntry* oldEntry,
        const TechniqueEntry* newEntry);
    void RemoveTechnique();
    void AddTechnique(const TechniqueInfo& info);
    void SetTechnique(const TechniqueInfo& info);
    void Undo() override;
    void Redo() override;

private:
    Context* context_;
    String materialName_;
    TechniqueInfo oldValue_;
    TechniqueInfo newValue_;
    unsigned index_;
};


/// Tracks addition, removal and modification of shader parameters in material
class ShaderParameterChangedAction
    : public EditAction
{
public:
    struct ShaderParameterInfo
    {
        String techniqueName_;
        MaterialQuality qualityLevel_;
        float lodDistance_;
    };

    ShaderParameterChangedAction(const Material* material, const String& parameterName, const Variant& oldValue, const Variant& newValue);
    void Undo() override;
    void Redo() override;

private:
    Context* context_;
    String materialName_;
    String parameterName_;
    Variant oldValue_;
    Variant newValue_;
};

}

}
