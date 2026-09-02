/* qt4compat: kcolorbutton.cpp */
#include <kcolorbutton.h>

#include <QtCore/QMimeData>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QColorDialog>
#include <QtGui/QDrag>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionButton>
#include <QtGui/QStylePainter>

KColorButton::KColorButton(QWidget *parent)
    : QPushButton(parent), m_alphaEnabled(false)
{
    init();
}

KColorButton::KColorButton(const QColor &c, QWidget *parent)
    : QPushButton(parent), m_color(c), m_alphaEnabled(false)
{
    init();
}

KColorButton::KColorButton(const QColor &c, const QColor &defaultColor, QWidget *parent)
    : QPushButton(parent), m_color(c), m_defaultColor(defaultColor), m_alphaEnabled(false)
{
    init();
}

KColorButton::~KColorButton()
{
}

void KColorButton::init()
{
    setAcceptDrops(true);
    connect(this, SIGNAL(clicked()), this, SLOT(chooseColor()));
}

QColor KColorButton::color() const
{
    return m_color;
}

void KColorButton::setColor(const QColor &c)
{
    if (m_color == c)
        return;
    m_color = c;
    update();
    emit changed(c);
}

void KColorButton::setAlphaChannelEnabled(bool alpha)
{
    m_alphaEnabled = alpha;
}

bool KColorButton::isAlphaChannelEnabled() const
{
    return m_alphaEnabled;
}

QColor KColorButton::defaultColor() const
{
    return m_defaultColor;
}

void KColorButton::setDefaultColor(const QColor &c)
{
    m_defaultColor = c;
}

QSize KColorButton::sizeHint() const
{
    QStyleOptionButton opt;
    opt.initFrom(this);
    opt.state |= QStyle::State_Enabled;
    opt.features = QStyleOptionButton::None;
    opt.text.clear();
    const int sz = fontMetrics().height() + 2;
    return style()->sizeFromContents(QStyle::CT_PushButton, &opt, QSize(sz * 3, sz), this).expandedTo(QApplication::globalStrut());
}

QSize KColorButton::minimumSizeHint() const
{
    return sizeHint();
}

void KColorButton::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    QStyleOptionButton opt;
    opt.initFrom(this);
    opt.state |= isDown() ? QStyle::State_Sunken : QStyle::State_Raised;
    opt.features = QStyleOptionButton::None;
    if (isDefault())
        opt.features |= QStyleOptionButton::DefaultButton;
    opt.text.clear();
    opt.icon = QIcon();
    painter.drawControl(QStyle::CE_PushButtonBevel, opt);

    QRect labelRect = style()->subElementRect(QStyle::SE_PushButtonContents, &opt, this);
    int shift = style()->pixelMetric(QStyle::PM_ButtonMargin, &opt, this) / 2;
    labelRect.adjust(shift, shift, -shift, -shift);
    int x = labelRect.x();
    int y = labelRect.y();
    int w = labelRect.width();
    int h = labelRect.height();
    if (isChecked() || isDown()) {
        x += style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &opt, this);
        y += style()->pixelMetric(QStyle::PM_ButtonShiftVertical, &opt, this);
    }
    QColor fillCol = isEnabled() ? m_color : palette().color(backgroundRole());
    if (fillCol.isValid()) {
        const QRect rect(x + 1, y + 1, w - 2, h - 2);
        if (fillCol.alpha() < 255) {
            QPixmap chessboard(16, 16);
            QPainter p(&chessboard);
            p.fillRect(0, 0, 8, 8, Qt::black);
            p.fillRect(8, 8, 8, 8, Qt::black);
            p.fillRect(0, 8, 8, 8, Qt::white);
            p.fillRect(8, 0, 8, 8, Qt::white);
            p.end();
            painter.fillRect(rect, QBrush(chessboard));
        }
        painter.fillRect(rect, fillCol);
    }
    painter.setPen(palette().color(QPalette::Shadow));
    painter.drawRect(x, y, w - 1, h - 1);

    if (hasFocus()) {
        QStyleOptionFocusRect focus;
        focus.initFrom(this);
        focus.rect = style()->subElementRect(QStyle::SE_PushButtonFocusRect, &opt, this);
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, focus);
    }
}

void KColorButton::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasColor() || (event->mimeData()->hasText() && QColor(event->mimeData()->text()).isValid()))
        event->acceptProposedAction();
}

void KColorButton::dropEvent(QDropEvent *event)
{
    QColor c;
    if (event->mimeData()->hasColor())
        c = qvariant_cast<QColor>(event->mimeData()->colorData());
    else if (event->mimeData()->hasText())
        c = QColor(event->mimeData()->text());
    if (c.isValid()) {
        setColor(c);
        event->acceptProposedAction();
    }
}

void KColorButton::mousePressEvent(QMouseEvent *e)
{
    m_pressPos = e->pos();
    QPushButton::mousePressEvent(e);
}

void KColorButton::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->buttons() & Qt::LeftButton) && (e->pos() - m_pressPos).manhattanLength() > QApplication::startDragDistance()) {
        QDrag *drag = new QDrag(this);
        QMimeData *mime = new QMimeData;
        mime->setColorData(m_color);
        mime->setText(m_color.name());
        drag->setMimeData(mime);
        QPixmap pm(24, 24);
        pm.fill(m_color);
        drag->setPixmap(pm);
        drag->exec(Qt::CopyAction);
        setDown(false);
    }
}

void KColorButton::keyPressEvent(QKeyEvent *e)
{
    if (e->matches(QKeySequence::Copy)) {
        QMimeData *mime = new QMimeData;
        mime->setColorData(m_color);
        mime->setText(m_color.name());
        QApplication::clipboard()->setMimeData(mime, QClipboard::Clipboard);
    } else if (e->matches(QKeySequence::Paste)) {
        const QMimeData *mime = QApplication::clipboard()->mimeData(QClipboard::Clipboard);
        QColor c;
        if (mime && mime->hasColor())
            c = qvariant_cast<QColor>(mime->colorData());
        else if (mime && mime->hasText())
            c = QColor(mime->text());
        if (c.isValid())
            setColor(c);
    } else {
        QPushButton::keyPressEvent(e);
    }
}

void KColorButton::chooseColor()
{
    QColor initial = m_color.isValid() ? m_color : m_defaultColor;
    QColorDialog::ColorDialogOptions options = 0;
    if (m_alphaEnabled)
        options |= QColorDialog::ShowAlphaChannel;
    const QColor c = QColorDialog::getColor(initial.isValid() ? initial : QColor(Qt::white), this, QString(), options);
    if (c.isValid())
        setColor(c);
}
