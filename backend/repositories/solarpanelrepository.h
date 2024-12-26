#ifndef SOLARPANELREPOSITORY_H
#define SOLARPANELREPOSITORY_H
#include "../models/solarpanel.h"
#include <qsqlquery.h>

class SolarPanelRepository
{
public:
    SolarPanelRepository();

    std::optional<SolarPanel> fetchById(qint64 id);
    bool updateSolarPanel(const SolarPanel &solarPanel);
    std::optional<SolarPanel> createSolarPanel(const SolarPanel &solarPanel);
    bool deleteSolarPanel(qint64 id);
    QList<SolarPanel> getPanelsByUser(qint64 userId, qint32 page, qint32 limit);
};

#endif // SOLARPANELREPOSITORY_H
