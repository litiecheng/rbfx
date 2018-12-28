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
#include "FilterResources.h"


namespace Urho3D
{

FilterResources::FilterResources(Context* context)
    : Converter(context)
{
}

void FilterResources::RegisterObject(Context* context)
{
    context->RegisterFactory<FilterResources>();
    URHO3D_COPY_BASE_ATTRIBUTES(Converter);
    URHO3D_ATTRIBUTE("HasTags", StringVector, hasTags_, {}, AM_DEFAULT);
    URHO3D_ATTRIBUTE("!HasTags", StringVector, doesntHaveTags_, {}, AM_DEFAULT);
}

void FilterResources::Execute(const String& resourcePath, const StringVector& input)
{
    StringVector filtered;

    for (const String& resourceName : input)
    {
        // TODO: implement resource tag checking when tagging is implemented
        filtered.Push(resourceName);
    }

    Converter::Execute(resourcePath, input);
}

}
