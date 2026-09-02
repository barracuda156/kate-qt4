/* qt4compat: ktip.cpp */
#include <ktip.h>

KTipDatabase::KTipDatabase(const QString &) {}
KTipDatabase::KTipDatabase(const QStringList &) {}
KTipDatabase::~KTipDatabase() {}
QString KTipDatabase::tip() const { return QString(); }
void KTipDatabase::nextTip() {}
void KTipDatabase::prevTip() {}

void KTipDialog::showTip(QWidget *, const QString &, bool) {}
void KTipDialog::showMultiTip(QWidget *, const QStringList &, bool) {}
void KTipDialog::showTip(const QString &, bool) {}
void KTipDialog::setShowOnStart(bool) {}
