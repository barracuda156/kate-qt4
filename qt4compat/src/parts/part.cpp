/* qt4compat: vendored from kdelibs 4.14 kparts/part.cpp, changes: KIO/remote URL support, KParts plugins, KMessageBox/KFileDialog and mime detection removed */
/* This file is part of the KDE project
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
             (C) 1999-2005 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <kparts/part.h>
#include <kparts/event.h>
#include <kparts/plugin.h>
#include <kparts/mainwindow.h>
#include <kparts/partmanager.h>
#include <kparts/browserextension.h>

#include <QtGui/QApplication>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QPainter>
#include <QtCore/QPoint>

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <kcomponentdata.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ktemporaryfile.h>
#include <kxmlguifactory.h>

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <kdebug.h>
#include <kiconloader.h>

using namespace KParts;

namespace KParts
{

class PartBasePrivate
{
public:
    Q_DECLARE_PUBLIC(PartBase)

    PartBasePrivate(PartBase *q): q_ptr(q)
    {
        m_pluginLoadingMode = PartBase::LoadPlugins;
        m_pluginInterfaceVersion = 0;
        m_obj = 0;
    }

    virtual ~PartBasePrivate()
    {
    }

    PartBase *q_ptr;
    PartBase::PluginLoadingMode m_pluginLoadingMode;
    int m_pluginInterfaceVersion;
    QObject *m_obj;
};

class PartPrivate: public PartBasePrivate
{
public:
    Q_DECLARE_PUBLIC(Part)

    PartPrivate(Part *q)
        : PartBasePrivate(q),
          m_iconLoader(0),
          m_bSelectable(true),
          m_autoDeleteWidget(true),
          m_autoDeletePart(true),
          m_manager(0)
    {
    }

    ~PartPrivate()
    {
    }

    KIconLoader* m_iconLoader;
    bool m_bSelectable;
    bool m_autoDeleteWidget;
    bool m_autoDeletePart;
    PartManager * m_manager;
    QPointer<QWidget> m_widget;
};

}

PartBase::PartBase()
    : d_ptr(new PartBasePrivate(this))
{
}

PartBase::PartBase(PartBasePrivate &dd)
    : d_ptr(&dd)
{
}

PartBase::~PartBase()
{
    delete d_ptr;
}

void PartBase::setPartObject( QObject *obj )
{
    Q_D(PartBase);

    d->m_obj = obj;
}

QObject *PartBase::partObject() const
{
    Q_D(const PartBase);

    return d->m_obj;
}

void PartBase::setComponentData(const KComponentData &componentData)
{
    setComponentData(componentData, true);
}

void PartBase::setComponentData(const KComponentData &componentData, bool bLoadPlugins)
{
    Q_D(PartBase);

    KXMLGUIClient::setComponentData(componentData);
    KGlobal::locale()->insertCatalog(componentData.catalogName());
    // install 'instancename'data resource type
    KGlobal::dirs()->addResourceType(QString(componentData.componentName() + "data").toUtf8().constData(),
                                     "data", componentData.componentName());
    if (bLoadPlugins) {
        loadPlugins(d->m_obj, this, componentData);
    }
}

void PartBase::loadPlugins(QObject *parent, KXMLGUIClient *parentGUIClient, const KComponentData &instance)
{
    Q_UNUSED(parent)
    Q_UNUSED(parentGUIClient)
    Q_UNUSED(instance)
    /* qt4compat: no KParts plugins */
}

void PartBase::setPluginLoadingMode( PluginLoadingMode loadingMode )
{
    Q_D(PartBase);

    d->m_pluginLoadingMode = loadingMode;
}

void KParts::PartBase::setPluginInterfaceVersion( int version )
{
    Q_D(PartBase);

    d->m_pluginInterfaceVersion = version;
}

Part::Part( QObject *parent )
    : QObject( parent ), PartBase( *new PartPrivate(this) )
{
    PartBase::setPartObject( this );
}

Part::Part(PartPrivate &dd, QObject *parent)
    : QObject( parent ), PartBase( dd )
{
    PartBase::setPartObject( this );
}

Part::~Part()
{
    Q_D(Part);

    //kDebug(1000) << this;

    if ( d->m_widget )
    {
        // We need to disconnect first, to avoid calling it !
        disconnect( d->m_widget, SIGNAL(destroyed()),
                    this, SLOT(slotWidgetDestroyed()) );
    }

    if ( d->m_manager )
        d->m_manager->removePart(this);

    if ( d->m_widget && d->m_autoDeleteWidget )
    {
        kDebug(1000) << "deleting widget" << d->m_widget << d->m_widget->objectName();
        delete static_cast<QWidget*>(d->m_widget);
    }

    delete d->m_iconLoader;
}

void Part::embed( QWidget * parentWidget )
{
    if ( widget() )
    {
        widget()->setParent( parentWidget, 0 );
        widget()->setGeometry( 0, 0, widget()->width(), widget()->height() );
        widget()->show();
    }
}

QWidget *Part::widget()
{
    Q_D(Part);

    return d->m_widget;
}

void Part::setAutoDeleteWidget(bool autoDeleteWidget)
{
    Q_D(Part);
    d->m_autoDeleteWidget = autoDeleteWidget;
}

void Part::setAutoDeletePart(bool autoDeletePart)
{
    Q_D(Part);
    d->m_autoDeletePart = autoDeletePart;
}



KIconLoader* Part::iconLoader()
{
    Q_D(Part);

    if (!d->m_iconLoader) {
        Q_ASSERT(componentData().isValid());
        d->m_iconLoader = new KIconLoader( componentData().componentName() );
    }
    return d->m_iconLoader;
}

void Part::setManager( PartManager *manager )
{
    Q_D(Part);

    d->m_manager = manager;
}

PartManager *Part::manager() const
{
    Q_D(const Part);

    return d->m_manager;
}

Part *Part::hitTest( QWidget *widget, const QPoint & )
{
    Q_D(Part);

    if ( (QWidget *)d->m_widget != widget )
        return 0;

    return this;
}

void Part::setWidget( QWidget *widget )
{
    Q_D(Part);
    d->m_widget = widget;
    connect( d->m_widget, SIGNAL(destroyed()),
             this, SLOT(slotWidgetDestroyed()), Qt::UniqueConnection );
}

void Part::setSelectable( bool selectable )
{
    Q_D(Part);

    d->m_bSelectable = selectable;
}

bool Part::isSelectable() const
{
    Q_D(const Part);

    return d->m_bSelectable;
}

void Part::customEvent( QEvent *ev )
{
    if ( PartActivateEvent::test( ev ) )
    {
        partActivateEvent( static_cast<PartActivateEvent *>(ev) );
        return;
    }

    if ( PartSelectEvent::test( ev ) )
    {
        partSelectEvent( static_cast<PartSelectEvent *>(ev) );
        return;
    }

    if ( GUIActivateEvent::test( ev ) )
    {
        guiActivateEvent( static_cast<GUIActivateEvent *>(ev) );
        return;
    }

    QObject::customEvent( ev );
}

void Part::partActivateEvent( PartActivateEvent * )
{
}

void Part::partSelectEvent( PartSelectEvent * )
{
}

void Part::guiActivateEvent( GUIActivateEvent * )
{
}

QWidget *Part::hostContainer( const QString &containerName )
{
    if ( !factory() )
        return 0;

    return factory()->container( containerName, this );
}

void Part::slotWidgetDestroyed()
{
    Q_D(Part);

    d->m_widget = 0;
    if (d->m_autoDeletePart) {
        kDebug(1000) << "deleting part" << objectName();
        delete this; // ouch, this should probably be deleteLater()
    }
}

void Part::loadPlugins()
{
    PartBase::loadPlugins(this, this, componentData());
}

//////////////////////////////////////////////////

namespace KParts
{

class ReadOnlyPartPrivate: public PartPrivate
{
public:
    Q_DECLARE_PUBLIC(ReadOnlyPart)

    ReadOnlyPartPrivate(ReadOnlyPart *q): PartPrivate(q)
    {
        m_showProgressInfo = true;
        m_saveOk = false;
        m_waitForSave = false;
        m_duringSaveAs = false;
        m_bTemp = false;
        m_bAutoDetectedMime = false;
    }

    ~ReadOnlyPartPrivate()
    {
    }

    bool openLocalFile();

    KUrl m_originalURL; // for saveAs
    QString m_originalFilePath; // for saveAs
    bool m_showProgressInfo : 1;
    bool m_saveOk : 1;
    bool m_waitForSave : 1;
    bool m_duringSaveAs : 1;

    /**
     * If @p true, @p m_file is a temporary file that needs to be deleted later.
     */
    bool m_bTemp: 1;

    // whether the mimetype in the arguments was detected by the part itself
    bool m_bAutoDetectedMime : 1;

    /**
     * Remote (or local) url - the one displayed to the user.
     */
    KUrl m_url;

    /**
     * Local file - the only one the part implementation should deal with.
     */
    QString m_file;

    OpenUrlArguments m_arguments;
};

class ReadWritePartPrivate: public ReadOnlyPartPrivate
{
public:
    Q_DECLARE_PUBLIC(ReadWritePart)

    ReadWritePartPrivate(ReadWritePart *q): ReadOnlyPartPrivate(q)
    {
        m_bModified = false;
        m_bReadWrite = true;
        m_bClosing = false;
    }

    void prepareSaving();

    bool m_bModified;
    bool m_bReadWrite;
    bool m_bClosing;
};

}

ReadOnlyPart::ReadOnlyPart( QObject *parent )
    : Part( *new ReadOnlyPartPrivate(this), parent )
{
}

ReadOnlyPart::ReadOnlyPart( ReadOnlyPartPrivate &dd, QObject *parent )
    : Part( dd, parent )
{
}

ReadOnlyPart::~ReadOnlyPart()
{
    ReadOnlyPart::closeUrl();
}

KUrl ReadOnlyPart::url() const
{
    Q_D(const ReadOnlyPart);

    return d->m_url;
}

void ReadOnlyPart::setUrl(const KUrl &url)
{
    Q_D(ReadOnlyPart);

    d->m_url = url;
    emit urlChanged( url );
}

QString ReadOnlyPart::localFilePath() const
{
    Q_D(const ReadOnlyPart);

    return d->m_file;
}

void ReadOnlyPart::setLocalFilePath( const QString &localFilePath )
{
    Q_D(ReadOnlyPart);

    d->m_file = localFilePath;
}

#ifndef KDE_NO_DEPRECATED
bool ReadOnlyPart::isLocalFileTemporary() const
{
    Q_D(const ReadOnlyPart);

    return d->m_bTemp;
}
#endif

#ifndef KDE_NO_DEPRECATED
void ReadOnlyPart::setLocalFileTemporary( bool temp )
{
    Q_D(ReadOnlyPart);

    d->m_bTemp = temp;
}
#endif

void ReadOnlyPart::setProgressInfoEnabled( bool show )
{
    Q_D(ReadOnlyPart);

    d->m_showProgressInfo = show;
}

bool ReadOnlyPart::isProgressInfoEnabled() const
{
    Q_D(const ReadOnlyPart);

    return d->m_showProgressInfo;
}

#ifndef KDE_NO_COMPAT
void ReadOnlyPart::showProgressInfo( bool show )
{
    Q_D(ReadOnlyPart);

    d->m_showProgressInfo = show;
}
#endif

bool ReadOnlyPart::openUrl( const KUrl &url )
{
    Q_D(ReadOnlyPart);

    if ( !url.isValid() )
        return false;
    if (d->m_bAutoDetectedMime) {
        d->m_arguments.setMimeType(QString());
        d->m_bAutoDetectedMime = false;
    }
    OpenUrlArguments args = d->m_arguments;
    if ( !closeUrl() )
        return false;
    d->m_arguments = args;
    setUrl(url);

    d->m_file.clear();

    if (d->m_url.isLocalFile()) {
        d->m_file = d->m_url.toLocalFile();
        return d->openLocalFile();
    }
    /* qt4compat: no KIO, remote URLs are not supported */
    QMessageBox::critical(widget() ? widget()->topLevelWidget() : 0, i18n("Error"),
                          i18n("Only local files can be opened: %1", d->m_url.prettyUrl()));
    emit canceled(i18n("Only local files can be opened: %1", d->m_url.prettyUrl()));
    return false;
}

bool ReadOnlyPart::openFile()
{
    kWarning(1000) << "Default implementation of ReadOnlyPart::openFile called!"
                   << metaObject()->className() << "should reimplement either openUrl or openFile.";
    return false;
}

bool ReadOnlyPartPrivate::openLocalFile()
{
    Q_Q(ReadOnlyPart);
    emit q->started( 0 );
    m_bTemp = false;
    /* qt4compat: the mime type is left to the part (KateDocument detects it itself) */
    const bool ret = q->openFile();
    if (ret) {
        emit q->setWindowCaption(m_url.prettyUrl());
        emit q->completed();
    } else {
        emit q->canceled(QString());
    }
    return ret;
}

void ReadOnlyPart::abortLoad()
{
}

bool ReadOnlyPart::closeUrl()
{
    Q_D(ReadOnlyPart);

    abortLoad(); //just in case

    d->m_arguments = KParts::OpenUrlArguments();

    if ( d->m_bTemp )
    {
        QFile::remove( d->m_file );
        d->m_bTemp = false;
    }
    // It always succeeds for a read-only part,
    // but the return value exists for reimplementations
    // (e.g. pressing cancel for a modified read-write part)
    return true;
}

void ReadOnlyPart::guiActivateEvent( GUIActivateEvent * event )
{
    Q_D(ReadOnlyPart);

    if (event->activated())
    {
        if (!d->m_url.isEmpty())
        {
            kDebug(1000) << d->m_url;
            emit setWindowCaption( d->m_url.prettyUrl() );
        } else emit setWindowCaption( "" );
    }
}

bool ReadOnlyPart::openStream( const QString& mimeType, const KUrl& url )
{
    Q_D(ReadOnlyPart);

    OpenUrlArguments args = d->m_arguments;
    if ( !closeUrl() )
        return false;
    d->m_arguments = args;
    setUrl( url );
    return doOpenStream( mimeType );
}

bool ReadOnlyPart::writeStream( const QByteArray& data )
{
    return doWriteStream( data );
}

bool ReadOnlyPart::closeStream()
{
    return doCloseStream();
}

BrowserExtension* ReadOnlyPart::browserExtension() const
{
    return findChild<KParts::BrowserExtension *>();
}

void KParts::ReadOnlyPart::setArguments(const OpenUrlArguments& arguments)
{
    Q_D(ReadOnlyPart);
    d->m_arguments = arguments;
    d->m_bAutoDetectedMime = arguments.mimeType().isEmpty();
}

OpenUrlArguments KParts::ReadOnlyPart::arguments() const
{
    Q_D(const ReadOnlyPart);
    return d->m_arguments;
}

//////////////////////////////////////////////////


ReadWritePart::ReadWritePart( QObject *parent )
    : ReadOnlyPart( *new ReadWritePartPrivate(this), parent )
{
}

ReadWritePart::~ReadWritePart()
{
    // parent destructor will delete temp file
    // we can't call our own closeUrl() here, because
    // "cancel" wouldn't cancel anything. We have to assume
    // the app called closeUrl() before destroying us.
}

void ReadWritePart::setReadWrite( bool readwrite )
{
    Q_D(ReadWritePart);

    // Perhaps we should check isModified here and issue a warning if true
    d->m_bReadWrite = readwrite;
}

void ReadWritePart::setModified( bool modified )
{
    Q_D(ReadWritePart);

    kDebug(1000) << "setModified(" << (modified ? "true" : "false") << ")";
    if ( !d->m_bReadWrite && modified )
    {
        kError(1000) << "Can't set a read-only document to 'modified' !" << endl;
        return;
    }
    d->m_bModified = modified;
}

void ReadWritePart::setModified()
{
    setModified( true );
}

bool ReadWritePart::queryClose()
{
    Q_D(ReadWritePart);

    if ( !isReadWrite() || !isModified() )
        return true;

    QString docName = url().fileName();
    if (docName.isEmpty()) docName = i18n( "Untitled" );

    QWidget *parentWidget=widget();
    if(!parentWidget) parentWidget=QApplication::activeWindow();

    const int res = QMessageBox::warning( parentWidget, i18n( "Close Document" ),
                                          i18n( "The document \"%1\" has been modified.\n"
                                                "Do you want to save your changes or discard them?" ,  docName ),
                                          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save );

    bool abortClose=false;
    bool handled=false;

    switch(res) {
    case QMessageBox::Save :
        sigQueryClose(&handled,&abortClose);
        if (!handled)
        {
            if (d->m_url.isEmpty())
            {
                const QString fileName = QFileDialog::getSaveFileName(parentWidget);
                if (fileName.isEmpty())
                    return false;

                saveAs( KUrl::fromPath(fileName) );
            }
            else
            {
                save();
            }
        } else if (abortClose) return false;
        return waitSaveComplete();
    case QMessageBox::Discard :
        return true;
    default : // case QMessageBox::Cancel :
        return false;
    }
}

bool ReadWritePart::closeUrl()
{
    abortLoad(); //just in case
    if ( isReadWrite() && isModified() )
    {
        if (!queryClose())
            return false;
    }
    // Not modified => ok and delete temp file.
    return ReadOnlyPart::closeUrl();
}

bool ReadWritePart::closeUrl( bool promptToSave )
{
    return promptToSave ? closeUrl() : ReadOnlyPart::closeUrl();
}

bool ReadWritePart::save()
{
    Q_D(ReadWritePart);

    d->m_saveOk = false;
    if ( d->m_file.isEmpty() ) // document was created empty
        d->prepareSaving();
    if( saveFile() )
        return saveToUrl();
    else
        emit canceled(QString());
    return false;
}

bool ReadWritePart::saveAs( const KUrl & kurl )
{
    Q_D(ReadWritePart);

    if (!kurl.isValid())
    {
        kError(1000) << "saveAs: Malformed URL " << kurl.url() << endl;
        return false;
    }
    d->m_duringSaveAs = true;
    d->m_originalURL = d->m_url;
    d->m_originalFilePath = d->m_file;
    d->m_url = kurl; // Store where to upload in saveToURL
    d->prepareSaving();
    bool result = save(); // Save local file and upload local file
    if (result) {
        emit urlChanged( d->m_url );
        emit setWindowCaption( d->m_url.prettyUrl() );
    } else {
        d->m_url = d->m_originalURL;
        d->m_file = d->m_originalFilePath;
        d->m_duringSaveAs = false;
        d->m_originalURL = KUrl();
        d->m_originalFilePath.clear();
    }

    return result;
}

// Set m_file correctly for m_url
void ReadWritePartPrivate::prepareSaving()
{
    // Local file
    if ( m_url.isLocalFile() )
    {
        if ( m_bTemp ) // get rid of a possible temp file first
        {              // (happens if previous url was remote)
            QFile::remove( m_file );
            m_bTemp = false;
        }
        m_file = m_url.toLocalFile();
    }
    else
    { // Remote file
        // We haven't saved yet, or we did but locally - provide a temp file
        if ( m_file.isEmpty() || !m_bTemp )
        {
            KTemporaryFile tempFile;
            tempFile.setAutoRemove(false);
            tempFile.open();
            m_file = tempFile.fileName();
            m_bTemp = true;
        }
        // otherwise, we already had a temp file
    }
}

bool ReadWritePart::saveToUrl()
{
    Q_D(ReadWritePart);

    if ( d->m_url.isLocalFile() )
    {
        setModified( false );
        emit completed();
        // if m_url is a local file there won't be a temp file -> nothing to remove
        assert( !d->m_bTemp );
        d->m_saveOk = true;
        d->m_duringSaveAs = false;
        d->m_originalURL = KUrl();
        d->m_originalFilePath.clear();
        return true; // Nothing to do
    }
    /* qt4compat: no KIO, remote URLs are not supported */
    emit canceled( i18n("Only local files can be saved: %1", d->m_url.prettyUrl()) );
    d->m_saveOk = false;
    d->m_duringSaveAs = false;
    return false;
}

bool ReadWritePart::isReadWrite() const
{
    Q_D(const ReadWritePart);

    return d->m_bReadWrite;
}

bool ReadWritePart::isModified() const
{
    Q_D(const ReadWritePart);

    return d->m_bModified;
}

bool ReadWritePart::waitSaveComplete()
{
    Q_D(ReadWritePart);
    return d->m_saveOk;
}

////

class KParts::OpenUrlArgumentsPrivate : public QSharedData
{
public:
    OpenUrlArgumentsPrivate()
        : reload(false),
          actionRequestedByUser(true),
          xOffset(0),
          yOffset(0),
          mimeType(),
          metaData()
    {}
    bool reload;
    bool actionRequestedByUser;
    int xOffset;
    int yOffset;
    QString mimeType;
    QMap<QString, QString> metaData;
};

KParts::OpenUrlArguments::OpenUrlArguments()
    : d(new OpenUrlArgumentsPrivate)
{
}

KParts::OpenUrlArguments::OpenUrlArguments(const OpenUrlArguments &other)
    : d(other.d)
{
}

KParts::OpenUrlArguments & KParts::OpenUrlArguments::operator=( const OpenUrlArguments &other)
{
    d = other.d;
    return *this;
}

KParts::OpenUrlArguments::~OpenUrlArguments()
{
}

bool KParts::OpenUrlArguments::reload() const
{
    return d->reload;
}

void KParts::OpenUrlArguments::setReload(bool b)
{
    d->reload = b;
}

int KParts::OpenUrlArguments::xOffset() const
{
    return d->xOffset;
}

void KParts::OpenUrlArguments::setXOffset(int x)
{
    d->xOffset = x;
}

int KParts::OpenUrlArguments::yOffset() const
{
    return d->yOffset;
}

void KParts::OpenUrlArguments::setYOffset(int y)
{
    d->yOffset = y;
}

QString KParts::OpenUrlArguments::mimeType() const
{
    return d->mimeType;
}

void KParts::OpenUrlArguments::setMimeType(const QString& mime)
{
    d->mimeType = mime;
}

QMap<QString, QString> & KParts::OpenUrlArguments::metaData()
{
    return d->metaData;
}

const QMap<QString, QString> & KParts::OpenUrlArguments::metaData() const
{
    return d->metaData;
}

bool KParts::OpenUrlArguments::actionRequestedByUser() const
{
    return d->actionRequestedByUser;
}

void KParts::OpenUrlArguments::setActionRequestedByUser(bool userRequested)
{
    d->actionRequestedByUser = userRequested;
}

