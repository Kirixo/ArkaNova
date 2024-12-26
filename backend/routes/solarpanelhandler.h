#ifndef SOLARPANELHANDLER_H
#define SOLARPANELHANDLER_H

#include "../repositories/solarpanelrepository.h"
#include "../utils/responsefactory.h"

class SolarPanelHandler
{
public:
    SolarPanelHandler();

    QHttpServerResponse getSolarPanel(const QHttpServerRequest& request);
    QHttpServerResponse getSolarPanelListByUser(const QHttpServerRequest& request);
    QHttpServerResponse createSolarPanel(const QHttpServerRequest& request);
    QHttpServerResponse updateSolarPanel(const QHttpServerRequest& request);
    QHttpServerResponse deleteSolarPanel(const QHttpServerRequest& request);

private:
    std::shared_ptr<SolarPanelRepository> solarPanelRepository_;
};

#endif // SOLARPANELHANDLER_H
