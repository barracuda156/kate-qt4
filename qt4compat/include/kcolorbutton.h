/* qt4compat: kcolorbutton.h - color swatch button opening QColorDialog */
#ifndef KDECOMPAT_KCOLORBUTTON_H
#define KDECOMPAT_KCOLORBUTTON_H

#include <QtGui/QColor>
#include <QtGui/QPushButton>
#include <kdemacros.h>

class KDEUI_EXPORT KColorButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY changed USER true)
    Q_PROPERTY(QColor defaultColor READ defaultColor WRITE setDefaultColor)
    Q_PROPERTY(bool alphaChannelEnabled READ isAlphaChannelEnabled WRITE setAlphaChannelEnabled)
public:
    explicit KColorButton(QWidget *parent = 0);
    explicit KColorButton(const QColor &c, QWidget *parent = 0);
    KColorButton(const QColor &c, const QColor &defaultColor, QWidget *parent = 0);
    virtual ~KColorButton();

    QColor color() const;
    void setColor(const QColor &c);
    void setAlphaChannelEnabled(bool alpha);
    bool isAlphaChannelEnabled() const;
    QColor defaultColor() const;
    void setDefaultColor(const QColor &c);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

Q_SIGNALS:
    void changed(const QColor &newColor);

protected:
    virtual void paintEvent(QPaintEvent *pe);
    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dropEvent(QDropEvent *);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);

private Q_SLOTS:
    void chooseColor();

private:
    void init();
    QColor m_color;
    QColor m_defaultColor;
    bool m_alphaEnabled;
    QPoint m_pressPos;
};

#endif
