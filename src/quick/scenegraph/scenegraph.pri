!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

# DEFINES += QSG_SEPARATE_INDEX_BUFFER
# DEFINES += QSG_DISTANCEFIELD_CACHE_DEBUG

# Core API
HEADERS += \
    $$PWD/coreapi/qsgbatchrenderer_p.h \
    $$PWD/coreapi/qsggeometry.h \
    $$PWD/coreapi/qsgmaterial.h \
    $$PWD/coreapi/qsgnode.h \
    $$PWD/coreapi/qsgnode_p.h \
    $$PWD/coreapi/qsgnodeupdater_p.h \
    $$PWD/coreapi/qsgabstractrenderer.h \
    $$PWD/coreapi/qsgabstractrenderer_p.h \
    $$PWD/coreapi/qsgrenderer_p.h \
    $$PWD/coreapi/qsgrendernode_p.h \
    $$PWD/coreapi/qsggeometry_p.h \
    $$PWD/coreapi/qsgmaterialshader_p.h

SOURCES += \
    $$PWD/coreapi/qsgabstractrenderer.cpp \
    $$PWD/coreapi/qsgbatchrenderer.cpp \
    $$PWD/coreapi/qsggeometry.cpp \
    $$PWD/coreapi/qsgmaterial.cpp \
    $$PWD/coreapi/qsgnode.cpp \
    $$PWD/coreapi/qsgnodeupdater.cpp \
    $$PWD/coreapi/qsgrenderer.cpp \
    $$PWD/coreapi/qsgrendernode.cpp \
    $$PWD/coreapi/qsgshaderrewriter.cpp

# Util API
HEADERS += \
    $$PWD/util/qsgareaallocator_p.h \
    $$PWD/util/qsgatlastexture_p.h \
    $$PWD/util/qsgdepthstencilbuffer_p.h \
    $$PWD/util/qsgengine.h \
    $$PWD/util/qsgengine_p.h \
    $$PWD/util/qsgflatcolormaterial.h \
    $$PWD/util/qsgsimplematerial.h \
    $$PWD/util/qsgsimplerectnode.h \
    $$PWD/util/qsgsimpletexturenode.h \
    $$PWD/util/qsgtexturematerial.h \
    $$PWD/util/qsgtexturematerial_p.h \
    $$PWD/util/qsgvertexcolormaterial.h \
    $$PWD/util/qsgtexture.h \
    $$PWD/util/qsgtexture_p.h \
    $$PWD/util/qsgtextureprovider.h \
    $$PWD/util/qsgdefaultpainternode_p.h \
    $$PWD/util/qsgdistancefieldutil_p.h \
    $$PWD/util/qsgshadersourcebuilder_p.h

SOURCES += \
    $$PWD/util/qsgareaallocator.cpp \
    $$PWD/util/qsgatlastexture.cpp \
    $$PWD/util/qsgdepthstencilbuffer.cpp \
    $$PWD/util/qsgengine.cpp \
    $$PWD/util/qsgflatcolormaterial.cpp \
    $$PWD/util/qsgsimplerectnode.cpp \
    $$PWD/util/qsgsimpletexturenode.cpp \
    $$PWD/util/qsgtexturematerial.cpp \
    $$PWD/util/qsgvertexcolormaterial.cpp \
    $$PWD/util/qsgtexture.cpp \
    $$PWD/util/qsgtextureprovider.cpp \
    $$PWD/util/qsgdefaultpainternode.cpp \
    $$PWD/util/qsgdistancefieldutil.cpp \
    $$PWD/util/qsgsimplematerial.cpp \
    $$PWD/util/qsgshadersourcebuilder.cpp

# QML / Adaptations API
HEADERS += \
    $$PWD/qsgadaptationlayer_p.h \
    $$PWD/qsgcontext_p.h \
    $$PWD/qsgcontextplugin_p.h \
    $$PWD/qsgdefaultglyphnode_p.h \
    $$PWD/qsgdefaultdistancefieldglyphcache_p.h \
    $$PWD/qsgdistancefieldglyphnode_p.h \
    $$PWD/qsgdistancefieldglyphnode_p_p.h \
    $$PWD/qsgdefaultglyphnode_p_p.h \
    $$PWD/qsgdefaultimagenode_p.h \
    $$PWD/qsgdefaultrectanglenode_p.h \
    $$PWD/qsgrenderloop_p.h \
    $$PWD/qsgthreadedrenderloop_p.h \
    $$PWD/qsgwindowsrenderloop_p.h \
    $$PWD/qsgdefaultlayer_p.h

SOURCES += \
    $$PWD/qsgadaptationlayer.cpp \
    $$PWD/qsgcontext.cpp \
    $$PWD/qsgcontextplugin.cpp \
    $$PWD/qsgdefaultglyphnode.cpp \
    $$PWD/qsgdefaultglyphnode_p.cpp \
    $$PWD/qsgdefaultdistancefieldglyphcache.cpp \
    $$PWD/qsgdistancefieldglyphnode.cpp \
    $$PWD/qsgdistancefieldglyphnode_p.cpp \
    $$PWD/qsgdefaultimagenode.cpp \
    $$PWD/qsgdefaultrectanglenode.cpp \
    $$PWD/qsgrenderloop.cpp \
    $$PWD/qsgthreadedrenderloop.cpp \
    $$PWD/qsgwindowsrenderloop.cpp \
    $$PWD/qsgdefaultlayer.cpp

# Built-in, non-plugin-based adaptations
include(adaptations/adaptations.pri)

RESOURCES += \
    $$PWD/scenegraph.qrc

OTHER_FILES += \
    $$PWD/shaders/24bittextmask.frag \
    $$PWD/shaders/8bittextmask.frag \
    $$PWD/shaders/distancefieldoutlinetext.frag \
    $$PWD/shaders/distancefieldshiftedtext.frag \
    $$PWD/shaders/distancefieldshiftedtext.vert \
    $$PWD/shaders/distancefieldtext.frag \
    $$PWD/shaders/distancefieldtext.vert \
    $$PWD/shaders/flatcolor.frag \
    $$PWD/shaders/flatcolor.vert \
    $$PWD/shaders/hiqsubpixeldistancefieldtext.frag \
    $$PWD/shaders/hiqsubpixeldistancefieldtext.vert \
    $$PWD/shaders/loqsubpixeldistancefieldtext.frag \
    $$PWD/shaders/loqsubpixeldistancefieldtext.vert \
    $$PWD/shaders/opaquetexture.frag \
    $$PWD/shaders/opaquetexture.vert \
    $$PWD/shaders/outlinedtext.frag \
    $$PWD/shaders/outlinedtext.vert \
    $$PWD/shaders/rendernode.frag \
    $$PWD/shaders/rendernode.vert \
    $$PWD/shaders/smoothcolor.frag \
    $$PWD/shaders/smoothcolor.vert \
    $$PWD/shaders/smoothtexture.frag \
    $$PWD/shaders/smoothtexture.vert \
    $$PWD/shaders/stencilclip.frag \
    $$PWD/shaders/stencilclip.vert \
    $$PWD/shaders/styledtext.frag \
    $$PWD/shaders/styledtext.vert \
    $$PWD/shaders/textmask.frag \
    $$PWD/shaders/textmask.vert \
    $$PWD/shaders/texture.frag \
    $$PWD/shaders/vertexcolor.frag \
    $$PWD/shaders/vertexcolor.vert \
    $$PWD/shaders/24bittextmask_core.frag \
    $$PWD/shaders/8bittextmask_core.frag \
    $$PWD/shaders/distancefieldoutlinetext_core.frag \
    $$PWD/shaders/distancefieldshiftedtext_core.frag \
    $$PWD/shaders/distancefieldshiftedtext_core.vert \
    $$PWD/shaders/distancefieldtext_core.frag \
    $$PWD/shaders/distancefieldtext_core.vert \
    $$PWD/shaders/flatcolor_core.frag \
    $$PWD/shaders/flatcolor_core.vert \
    $$PWD/shaders/hiqsubpixeldistancefieldtext_core.frag \
    $$PWD/shaders/hiqsubpixeldistancefieldtext_core.vert \
    $$PWD/shaders/loqsubpixeldistancefieldtext_core.frag \
    $$PWD/shaders/loqsubpixeldistancefieldtext_core.vert \
    $$PWD/shaders/opaquetexture_core.frag \
    $$PWD/shaders/opaquetexture_core.vert \
    $$PWD/shaders/outlinedtext_core.frag \
    $$PWD/shaders/outlinedtext_core.vert \
    $$PWD/shaders/rendernode_core.frag \
    $$PWD/shaders/rendernode_core.vert \
    $$PWD/shaders/smoothcolor_core.frag \
    $$PWD/shaders/smoothcolor_core.vert \
    $$PWD/shaders/smoothtexture_core.frag \
    $$PWD/shaders/smoothtexture_core.vert \
    $$PWD/shaders/stencilclip_core.frag \
    $$PWD/shaders/stencilclip_core.vert \
    $$PWD/shaders/styledtext_core.frag \
    $$PWD/shaders/styledtext_core.vert \
    $$PWD/shaders/textmask_core.frag \
    $$PWD/shaders/textmask_core.vert \
    $$PWD/shaders/texture_core.frag \
    $$PWD/shaders/vertexcolor_core.frag \
    $$PWD/shaders/vertexcolor_core.vert \
    scenegraph/shaders/visualization.frag \
    scenegraph/shaders/visualization.vert

