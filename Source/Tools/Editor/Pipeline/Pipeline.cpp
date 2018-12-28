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

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/WorkQueue.h>
#include <Urho3D/Resource/JSONValue.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include "Converter.h"
#include "Pipeline.h"

namespace Urho3D
{

Pipeline::Pipeline(Context* context)
    : Serializable(context)
{
    SubscribeToEvent(E_ENDFRAME, [this](StringHash, VariantMap&) {
        DispatchChangedAssets();
    });
}

Pipeline::~Pipeline()
{
    GetWorkQueue()->Complete(0);
}

bool Pipeline::LoadJSON(const JSONValue& source)
{
    if (source.GetValueType() != JSON_ARRAY)
        return false;

    for (auto i = 0U; i < source.Size(); i++)
    {
        const JSONValue& converterData = source[i];
        StringHash type = Converter::GetSerializedType(converterData);
        if (type == StringHash::ZERO)
            return false;

        SharedPtr<Converter> converter = DynamicCast<Converter>(context_->CreateObject(type));
        if (converter.Null() || !converter->LoadJSON(converterData))
            return false;

        converters_.Push(converter);
    }

    return true;
}

void Pipeline::SetCachePath(const String& cachePath)
{
    GetFileSystem()->CreateDirsRecursive(cachePath);
    cachePath_ = cachePath;
}

void Pipeline::AddAssetDirectory(const String& path)
{
    SharedPtr<FileWatcher> watcher(new FileWatcher(context_));
    watcher->StartWatching(path, true);
    watchers_.Push(watcher);
}

void Pipeline::BuildCache(ConverterKinds converterKinds)
{
    for (const auto& watcher : watchers_)
    {
        StringVector filesPaths;
        GetFileSystem()->ScanDir(filesPaths, watcher->GetPath(), "*", SCAN_FILES, true);
        ConvertAssets(watcher->GetPath(), filesPaths, converterKinds);
    }
}

void Pipeline::ConvertAssets(const String& resourcePath, const StringVector& resourceNames, ConverterKinds converterKinds)
{
    for (auto& converter : converters_)
    {
        if (converterKinds & converter->GetKind())
        {
            GetWorkQueue()->AddWorkItem([&converter, resourcePath, resourceNames]() {
                converter->Execute(resourcePath, resourceNames);
            });
        }
    }
}

void Pipeline::DispatchChangedAssets()
{
    for (auto& watcher : watchers_)
    {
        StringVector modified;
        String path;
        while (watcher->GetNextChange(path) && GetFileSystem()->FileExists(watcher->GetPath() + path))
            modified.EmplaceBack(std::move(path));
        if (!modified.Empty())
            ConvertAssets(watcher->GetPath(), modified, CONVERTER_ONLINE);
    }
}

}
