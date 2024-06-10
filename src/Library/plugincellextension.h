#ifndef PLUGINCELLEXTENSION_H
#define PLUGINCELLEXTENSION_H

#include <QString>
#include <iostream>

class PluginCellExtension
{
public:
    PluginCellExtension();

    virtual QString getAttributeName() const = 0;
    virtual QString getAttributeValue() const = 0;
};

#endif // PLUGINCELLEXTENSION_H
