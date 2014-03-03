#include "QmlVlcVideoSurface.h"

#include "SGVlcVideoNode.h"

QmlVlcVideoSurface::QmlVlcVideoSurface()
{
    setFlag( QQuickItem::ItemHasContents, true );
}

QmlVlcVideoSurface::~QmlVlcVideoSurface()
{
    setSource( 0 );
}

void QmlVlcVideoSurface::setSource( QmlVlcVideoOutput* source )
{
    if( source == m_source.data() )
        return;

    if( m_source )
        m_source->unregisterVideoSurface( this );

    m_source = source;

    if( m_source )
        m_source->registerVideoSurface( this );

    Q_EMIT sourceChanged();
}

QSGNode* QmlVlcVideoSurface::updatePaintNode( QSGNode* oldNode,
                                              UpdatePaintNodeData* /*data*/ )
{
    if( !m_frame )
        return 0;

    SGVlcVideoNode* node = static_cast<SGVlcVideoNode*>( oldNode );
    if( !node )
        node = new SGVlcVideoNode;

    QRectF outRect( x(), y(), width(), height() );

    const uint16_t fw = m_frame->width;
    const uint16_t fh = m_frame->height;

    const qreal frameAspect = qreal( fw ) / fh;
    const qreal itemAspect = width() / height();
    qreal outWidth = width();
    qreal outHeight = height();

    if( frameAspect > itemAspect )
        outHeight = outWidth / frameAspect;
    else if( frameAspect < itemAspect )
        outWidth = outHeight * frameAspect;

    outRect = QRectF( x() + ( width() - outWidth ) / 2,
                        y() + ( height() - outHeight ) / 2,
                        outWidth, outHeight );

    node->setFrame( m_frame );
    node->setRect( outRect );

    return node;
}

void QmlVlcVideoSurface::presentFrame( const QSharedPointer<const QmlVlcI420Frame>& frame )
{
    m_frame = frame;
    update();
}