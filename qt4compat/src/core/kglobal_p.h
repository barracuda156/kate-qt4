/* qt4compat: private hooks between KGlobal and KComponentData */
#ifndef KDECOMPAT_KGLOBAL_P_H
#define KDECOMPAT_KGLOBAL_P_H

class KComponentData;

namespace KGlobal
{
    void setMainComponent(const KComponentData &d);
}

#endif
