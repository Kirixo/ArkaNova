import React, { useEffect, useState } from 'react';
import { useTranslation } from 'react-i18next';
import { useNavigate } from 'react-router-dom';
import {
  Box,
  Card,
  CardContent,
  Typography,
  Button,
  Alert,
  Snackbar,
  Skeleton,
} from '@mui/material';
import {
  Thermostat as ThermostatIcon,
  BoltOutlined as BoltIcon,
} from '@mui/icons-material';
import { format } from 'date-fns';
import { enUS, uk } from 'date-fns/locale';
import Navigation from '../components/Navigation';
import { API_CONFIG } from '../config/api';
import type { SolarPanel, SensorListResponse, LatestMeasurement, PanelMeasurements } from '../types/api';

const HomePage: React.FC = () => {
  const { t, i18n } = useTranslation();
  const navigate = useNavigate();
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [panelMeasurements, setPanelMeasurements] = useState<{ [key: number]: PanelMeasurements }>({});
  const [panels, setPanels] = useState<SolarPanel[]>([]);
  const user = JSON.parse(localStorage.getItem('user') || '{}');

  const formatDate = (timestamp: number) => {
    return format(new Date(timestamp), 'PPpp', {
      locale: i18n.language === 'uk-UA' ? uk : enUS,
    });
  };

  const fetchLatestMeasurement = async (sensorId: number): Promise<LatestMeasurement | null> => {
    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/measurement/latest/sensor?sensor_id=${sensorId}`,
        {
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
        }
      );

      if (response.ok) {
        return await response.json();
      }
    } catch (err) {
      console.error(`Failed to fetch measurement for sensor ${sensorId}:`, err);
    }
    return null;
  };

  const fetchPanelSensors = async (panelId: number): Promise<SensorListResponse | null> => {
    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/sensor/list/solarpanel?panel_id=${panelId}&page=1&limit=100`,
        {
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
        }
      );

      if (response.ok) {
        return await response.json();
      }
    } catch (err) {
      console.error(`Failed to fetch sensors for panel ${panelId}:`, err);
    }
    return null;
  };

  useEffect(() => {
    const loadData = async () => {
      setLoading(true);
      setError(null);

      try {
        // 1. Load solar panels
        const response = await fetch(
          `${API_CONFIG.BASE_URL}${API_CONFIG.ENDPOINTS.SOLAR_PANELS}?user_id=${user.id}&page=1&limit=25`,
          {
            headers: {
              'Content-Type': 'application/json',
              'ngrok-skip-browser-warning': 'true',
            },
            credentials: 'include',
          }
        );

        if (!response.ok) {
          setError(t('home.errors.failedMetrics'));
          return;
        }

        const panelsResponse: { panels: SolarPanel[] } = await response.json();
        setPanels(panelsResponse.panels || []);

        if (panelsResponse.panels && panelsResponse.panels.length > 0) {
          const newPanelMeasurements: { [key: number]: PanelMeasurements } = {};

          // 2. For each panel, get its sensors and their measurements
          for (const panel of panelsResponse.panels) {
            const sensorsResponse = await fetchPanelSensors(panel.id);
            
            if (sensorsResponse?.sensors) {
              const measurementsByType: { [key: string]: LatestMeasurement[] } = {};

              // 3. Get latest measurement for each sensor
              for (const sensor of sensorsResponse.sensors) {
                const measurement = await fetchLatestMeasurement(sensor.id);
                if (measurement) {
                  const type = sensor.type.name;
                  if (!measurementsByType[type]) {
                    measurementsByType[type] = [];
                  }
                  measurementsByType[type].push(measurement);
                }
              }

              // 4. Calculate averages for each measurement type
              const panelData: PanelMeasurements = {};
              for (const [type, measurements] of Object.entries(measurementsByType)) {
                if (measurements.length > 0) {
                  const avg = measurements.reduce((sum, m) => sum + m.data, 0) / measurements.length;
                  const latestTimestamp = Math.max(...measurements.map(m => m.recorded_at));
                  panelData[type] = {
                    average: Number(avg.toFixed(2)),
                    timestamp: latestTimestamp,
                  };
                }
              }

              console.log('Panel ID:', panel.id);
              console.log('Measurements by type:', measurementsByType);
              console.log('Processed panel data:', panelData);

              newPanelMeasurements[panel.id] = panelData;
            }
          }

          console.log('All panel measurements:', newPanelMeasurements);
          setPanelMeasurements(newPanelMeasurements);
        }
      } catch (err) {
        console.error('Failed to load data:', err);
        setError(t('home.errors.failedMetrics'));
      } finally {
        setLoading(false);
      }
    };

    if (user?.id) {
      loadData();
    } else {
      setError(t('home.errors.failedMetrics'));
    }
  }, [t, user?.id]);

  return (
    <Box sx={{ display: 'flex' }}>
      <Navigation isAdmin={user.role === 'admin'} />
      <Box
        component="main"
        sx={{
          flexGrow: 1,
          p: 3,
          backgroundColor: '#F1F1F1',
          minHeight: '100vh',
        }}
      >
        {/* Greeting */}
        <Typography
          variant="h1"
          sx={{
            fontSize: '24px',
            fontWeight: 700,
            mb: 4,
            fontFamily: 'Roboto',
          }}
        >
          {t('home.greeting', { email: user.email })}
        </Typography>

        {loading ? (
          // Skeleton loading state
          <Box sx={{ display: 'flex', gap: 2, flexWrap: 'wrap' }}>
            <Skeleton variant="rectangular" width={300} height={200} />
            <Skeleton variant="rectangular" width={300} height={200} />
          </Box>
        ) : panels.length === 0 ? (
          // No panels message
          <Alert severity="info" sx={{ mb: 2 }}>
            {t('home.errors.noPanels')}
          </Alert>
        ) : (
          <>
            {/* Panels Grid */}
            <Box sx={{ display: 'flex', gap: 2, flexWrap: 'wrap', mb: 4 }}>
              {panels.map(panel => {
                const data = panelMeasurements[panel.id] || {};

                return (
                  <Card key={panel.id} sx={{ minWidth: 300, mb: 2 }}>
                    <CardContent>
                      <Typography variant="h6" sx={{ mb: 2 }}>
                        {panel.location}
                      </Typography>

                      {/* Temperature */}
                      <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
                        <ThermostatIcon
                          sx={{ color: '#4CAF50', fontSize: 24, mr: 1 }}
                        />
                        {data.temperature ? (
                          <Box>
                            <Typography>
                              {data.temperature.average}°C
                            </Typography>
                            <Typography variant="caption" color="text.secondary" display="block">
                              {formatDate(data.temperature.timestamp)}
                            </Typography>
                          </Box>
                        ) : (
                          <Typography color="text.secondary">
                            N/A
                          </Typography>
                        )}
                      </Box>

                      {/* Power */}
                      <Box sx={{ display: 'flex', alignItems: 'center' }}>
                        <BoltIcon
                          sx={{ color: '#2196F3', fontSize: 24, mr: 1 }}
                        />
                        {data.power ? (
                          <Box>
                            <Typography>
                              {data.power.average} {t('home.energy.unit')}
                            </Typography>
                            <Typography variant="caption" color="text.secondary" display="block">
                              {formatDate(data.power.timestamp)}
                            </Typography>
                          </Box>
                        ) : (
                          <Typography color="text.secondary">
                            N/A
                          </Typography>
                        )}
                      </Box>
                    </CardContent>
                  </Card>
                );
              })}
            </Box>

            {/* Action Buttons */}
            <Box sx={{ display: 'flex', gap: 2 }}>
              <Button
                variant="contained"
                color="primary"
                onClick={() => navigate('/dashboard')}
                sx={{ minWidth: 200 }}
              >
                {t('home.buttons.viewDashboard')}
              </Button>
              <Button
                variant="outlined"
                color="secondary"
                onClick={() => navigate('/sensors')}
                sx={{ minWidth: 200 }}
              >
                {t('home.buttons.viewSensors')}
              </Button>
            </Box>
          </>
        )}
      </Box>

      {/* Error Snackbar */}
      <Snackbar
        open={!!error}
        autoHideDuration={6000}
        onClose={() => setError(null)}
        anchorOrigin={{ vertical: 'bottom', horizontal: 'center' }}
      >
        <Alert severity="error" onClose={() => setError(null)}>
          {error}
        </Alert>
      </Snackbar>
    </Box>
  );
};

export default HomePage; 