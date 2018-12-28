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

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/JSONValue.h>
#include "Project.h"
#include "SubprocessExec.h"


namespace Urho3D
{

static StringVector subprocessLogMsgBlacklist{
    "ERROR: No texture created, can not set data",  // 2D scenes load texture data. This error is benign.
};

SubprocessExec::SubprocessExec(Context* context)
    : Converter(context)
{
}

void SubprocessExec::RegisterObject(Context* context)
{
    context->RegisterFactory<SubprocessExec>();
    URHO3D_COPY_BASE_ATTRIBUTES(Converter);
    URHO3D_ATTRIBUTE("Exec", String, executable_, String::EMPTY, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Args", StringVector, args_, {}, AM_DEFAULT);
}

void SubprocessExec::Execute(const String& resourcePath, const StringVector& input)
{
    auto* project = GetSubsystem<Project>();
    String executable = executable_;
    if (!IsAbsolutePath(executable))
        executable = GetPath(GetFileSystem()->GetProgramFileName()) + executable;

    for (const String& resourceName : input)
    {
        StringVector args = args_;
        // Insert variables to args
        for (String& arg : args)
        {
            arg = Format(arg,
                fmt::arg("resource_name", resourceName),
                fmt::arg("resource_path", resourcePath + resourceName),
                fmt::arg("project_path", RemoveTrailingSlash(project->GetProjectPath())),
                fmt::arg("cache_path", RemoveTrailingSlash(project->GetCachePath()))
            );
        }

        String output;
        int result = GetFileSystem()->SystemRun(executable, args, output);

        StringVector lines = output.Split('\n');
        for (const String& line : lines)
        {
            if (!line.StartsWith("["))
                // Not a log message.
                continue;

            bool blacklisted = false;
            for (const String& blacklistedMsg : subprocessLogMsgBlacklist)
            {
                if (line.EndsWith(blacklistedMsg))
                {
                    blacklisted = true;
                    break;
                }
            }

            if (!blacklisted)
            {
                bool error = line.Contains("] ERROR: ") || line.StartsWith("ERROR: ");
                Log::WriteRaw(line, error);
                Log::WriteRaw("\n", error);
            }
        }

        if (result != 0)
            URHO3D_LOGERROR("Failed SubprocessExec({}): {} {}", result, executable, String::Joined(args, " "));
    }

    // Outputs of subprocess can not be known.
    Converter::Execute(resourcePath, { });
}

}
