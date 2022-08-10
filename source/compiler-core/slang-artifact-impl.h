// slang-artifact-impl.h
#ifndef SLANG_ARTIFACT_IMPL_H
#define SLANG_ARTIFACT_IMPL_H

#include "slang-artifact.h"

#include "../core/slang-lazy-castable-list.h"

#include "../../slang-com-helper.h"
#include "../../slang-com-ptr.h"

#include "../core/slang-com-object.h"

namespace Slang
{

class ArtifactList : public ComBaseObject, public IArtifactList
{
public:
    SLANG_COM_BASE_IUNKNOWN_ALL

    // ICastable
    SLANG_NO_THROW void* SLANG_MCALL castAs(const Guid& guid) SLANG_OVERRIDE;

    // IArtifactList
    SLANG_NO_THROW IArtifact* SLANG_MCALL getParent() SLANG_OVERRIDE { return m_parent; }
    SLANG_NO_THROW void SLANG_MCALL setParent(IArtifact* parent) SLANG_OVERRIDE { _setParent(parent); }

    SLANG_NO_THROW IArtifact* SLANG_MCALL getAt(Index index) SLANG_OVERRIDE { return m_artifacts[index]; }
    SLANG_NO_THROW Count SLANG_MCALL getCount() SLANG_OVERRIDE { return m_artifacts.getCount(); }
    SLANG_NO_THROW void SLANG_MCALL add(IArtifact* artifact) SLANG_OVERRIDE;
    SLANG_NO_THROW void SLANG_MCALL removeAt(Index index) SLANG_OVERRIDE;
    SLANG_NO_THROW void SLANG_MCALL clear() SLANG_OVERRIDE;

        // NOTE! The parent is a weak reference. 
    ArtifactList(IArtifact* parent):
        m_parent(parent)
    {
    }

    virtual ~ArtifactList() { _setParent(nullptr); }

protected:
    void* getInterface(const Guid& guid);
    void* getObject(const Guid& guid);

    void _setParent(IArtifact* artifact);
    
    IArtifact* m_parent;
    List<ComPtr<IArtifact>> m_artifacts;
};

/*
Discussion:

Another issue occurs around wanting to hold multiple kernels within a container. The problem here is that although through the desc
we can identify what target a kernel is for, there is no way of telling what stage it is for.

When discussing the idea of a shader cache, one idea was to use a ISlangFileSystem (which could actually be a zip, or directory or in memory rep)
as the main structure. Within this it can contain kernels, and then a json manifest can describe what each of these actually are.

This all 'works', in that we can add an element of ISlangFileSystem with a desc of Container. Code that uses this can then go through the process 
of finding, and getting the blob, and find from the manifest what it means. That does sound a little tedious though. Perhaps we just have an interface
that handles this detail, such that we search for that first. That interface is just attached to the artifact as an element.
*/

/* Implementation of the IArtifact interface */
class Artifact : public ComBaseObject, public IArtifact
{
public:
    
    SLANG_COM_BASE_IUNKNOWN_ALL
    
        /// IArtifact impl
    virtual SLANG_NO_THROW Desc SLANG_MCALL getDesc() SLANG_OVERRIDE { return m_desc; }
    virtual SLANG_NO_THROW IArtifact* SLANG_MCALL getParent() SLANG_OVERRIDE { return m_parent; }
    virtual SLANG_NO_THROW void SLANG_MCALL setParent(IArtifact* parent) SLANG_OVERRIDE { m_parent = parent; }
    virtual SLANG_NO_THROW bool SLANG_MCALL exists() SLANG_OVERRIDE;
    virtual SLANG_NO_THROW SlangResult SLANG_MCALL loadBlob(Keep keep, ISlangBlob** outBlob) SLANG_OVERRIDE;
    virtual SLANG_NO_THROW SlangResult SLANG_MCALL requireFile(Keep keep, IFileArtifactRepresentation** outFileRep) SLANG_OVERRIDE;
    virtual SLANG_NO_THROW const char* SLANG_MCALL getName() SLANG_OVERRIDE { return m_name.getBuffer(); }
    
    virtual SLANG_NO_THROW void SLANG_MCALL addAssociated(ICastable* castable) SLANG_OVERRIDE;
    virtual void* SLANG_MCALL SLANG_MCALL findAssociated(const Guid& unk) SLANG_OVERRIDE;
    virtual ICastableList* SLANG_MCALL getAssociated() SLANG_OVERRIDE;

    virtual SLANG_NO_THROW void SLANG_MCALL addRepresentation(ICastable* castable) SLANG_OVERRIDE;
    virtual SLANG_NO_THROW void SLANG_MCALL addRepresentationUnknown(ISlangUnknown* rep) SLANG_OVERRIDE;
    virtual void* SLANG_MCALL SLANG_MCALL findRepresentation(const Guid& guid) SLANG_OVERRIDE;
    virtual ICastableList* SLANG_MCALL getRepresentations() SLANG_OVERRIDE;

    virtual IArtifactList* SLANG_MCALL getChildren() SLANG_OVERRIDE;

    /// Ctor
    Artifact(const Desc& desc, const String& name) :
        m_desc(desc),
        m_name(name),
        m_parent(nullptr)
    {}

protected:
    void* getInterface(const Guid& uuid);

    Desc m_desc;                                ///< Description of the artifact
    IArtifact* m_parent;                        ///< Artifact this artifact belongs to

    String m_name;                              ///< Name of this artifact

    LazyCastableList m_associated;              ///< Associated items
    LazyCastableList m_representations;         ///< Representations

    ComPtr<IArtifactList> m_children;           ///< The children to this artifact
};

} // namespace Slang

#endif
