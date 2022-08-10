// slang-artifact-representation-impl.cpp
#include "slang-artifact-representation-impl.h"

#include "../core/slang-file-system.h"

#include "../core/slang-type-text-util.h"
#include "../core/slang-io.h"
#include "../core/slang-array-view.h"

#include "slang-artifact-util.h"

namespace Slang {

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FileArtifactRepresentation !!!!!!!!!!!!!!!!!!!!!!!!!!! */

void* FileArtifactRepresentation::getInterface(const Guid& guid)
{
    if (guid == ISlangUnknown::getTypeGuid() ||
        guid == ICastable::getTypeGuid() ||
        guid == IArtifactRepresentation::getTypeGuid() ||
        guid == IFileArtifactRepresentation::getTypeGuid())
    {
        return static_cast<IFileArtifactRepresentation*>(this);
    }
    return nullptr;
}

void* FileArtifactRepresentation::getObject(const Guid& guid)
{
    SLANG_UNUSED(guid);
    return nullptr;
}

ISlangMutableFileSystem* FileArtifactRepresentation::_getFileSystem()
{
    return m_fileSystem ? m_fileSystem : OSFileSystem::getMutableSingleton();
}

void* FileArtifactRepresentation::castAs(const Guid& guid)
{
    if (auto intf = getInterface(guid))
    {
        return intf;
    }
    return getObject(guid);
}

SlangResult FileArtifactRepresentation::writeToBlob(ISlangBlob** blob)
{
    if (m_kind == Kind::NameOnly)
    {
        // If it's referenced by a name only, it's a file that *can't* be loaded as a blob in general.
        return SLANG_E_NOT_AVAILABLE;
    }

    auto fileSystem = _getFileSystem();
    return fileSystem->loadFile(m_path.getBuffer(), blob);
}

bool FileArtifactRepresentation::exists()
{
    // TODO(JS):
    // If it's a name only it's hard to know what exists should do. It can't *check* because it relies on the 'system' doing 
    // the actual location. We could ask the IArtifactUtil, and that could change the behavior.
    // For now we just assume it does.
    if (m_kind == Kind::NameOnly)
    {
        return true;
    }

    auto fileSystem = _getFileSystem();

    SlangPathType pathType;
    const auto res = fileSystem->getPathType(m_path.getBuffer(), &pathType);

    // It exists if it is a file
    return SLANG_SUCCEEDED(res) && pathType == SLANG_PATH_TYPE_FILE;
}

void FileArtifactRepresentation::disown()
{
    if (_isOwned())
    {
        m_kind = Kind::Reference;
    }
}

FileArtifactRepresentation::~FileArtifactRepresentation()
{
    if (_isOwned())
    {
        auto fileSystem = _getFileSystem();
        fileSystem->remove(m_path.getBuffer());
    }
}

} // namespace Slang
